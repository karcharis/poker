#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int card_ptr_comp(const void * vp1, const void * vp2) {
  
  const card_t * const * cp1 = vp1; // const card_t * const * ~ const (const card_t *) *
  const card_t * const * cp2 = vp2;
  
  if ( (*cp2)->value - (*cp1)->value != 0 ) {
    return (*cp2)->value - (*cp1)->value;
  }
  else {
    if ( (*cp2)->suit - (*cp1)->suit != 0 ) {
      return (*cp2)->suit - (*cp1)->suit;
    }
    return 0;
  }
}

suit_t flush_suit(deck_t * hand) {

  int occur[4] = {0, 0, 0 ,0};

  for (size_t i = 0; i < (hand->n_cards); i++) {
    switch (hand->cards[i]-> suit) {
        case SPADES:
          occur[0]++;
          break;
        case HEARTS:
          occur[1]++;
          break;
        case DIAMONDS:
          occur[2]++;
          break;
        case CLUBS:
          occur[3]++;
          break;
        case NUM_SUITS: 
          break;
     }
  }
  
  if (occur[0]>4) {
      return SPADES;
  }
  else if (occur[1]>4) {
    return HEARTS;
  }
  else if (occur[2]>4) {
    return DIAMONDS;
  }
  else if (occur[3]>4) {
    return CLUBS;
  }
  else {
    return NUM_SUITS;
  }
}

unsigned get_largest_element(unsigned * arr, size_t n) {
  
  unsigned max = arr[0];
  for (size_t i = 1; i < n; i++) {
    if (max < arr[i]) {
      max = arr[i];
    }
  }
  return max;
}

size_t get_match_index(unsigned * match_counts, size_t n,unsigned n_of_akind){
  
  unsigned flag = 0;
  size_t index;
  for (size_t i = 0; i < n; i++) {
    if (match_counts[i] == n_of_akind) {
      flag = 1;
      index = i;
      break;
     }
  }
  assert(flag);
  return index;
}

ssize_t find_secondary_pair(deck_t * hand, unsigned * match_counts, size_t match_idx) {
  for (ssize_t i = 0; i < hand->n_cards; i++) {
    if (match_counts[i] > 1 && hand->cards[i]->value != hand->cards[match_idx]->value) {
      return i;
    }
  }
  return -1;
}
  
int step_to_straight(deck_t * hand, size_t index, unsigned val, suit_t fs) {
  if (fs == NUM_SUITS) {
    return val - hand->cards[index]->value;
  }
  else if (fs == hand->cards[index]->suit) {
    return 1;
  }
  else {
    return 0;
  }
}

/*
 * In the next function we may assume, as always that the hand is sorted.
 * In case fs != NUM_SUIT, we are looking for a flush straight.
*/

int is_nlength_straight_at(deck_t * hand, size_t index, suit_t fs, unsigned n) {
  // Return 0 if fs != NUM_SUITS and fs != hand->cards[index]->suit.
  if (fs != NUM_SUITS && fs != hand->cards[index]->suit) {
    return 0;
  }
  // next two conditions exclude:
  //  1. The null case of 0-straight
  //  2. The boundary case of 1-straight
  assert(n > 0);
  if (n == 1) {
    return 1;
  }
  // Check if there are enough cards for a n-straight,
  if (index + n > hand->n_cards) {
    return 0;
  }
  // if there is proceed. 

  // unsigned val keeps the value of the last member of the straight
  unsigned val = hand->cards[index]->value;
  // int count counts the hands currently in straight, we begin to count from 1 since the card pointed by hand->cards[index] is counted to the straight
  int count = 1;
  
  while (index < hand->n_cards - 1) {
    // We increase index at the start of the loop, since the first card is already counted
    // Note, the boundary case of 1-straight is already handled, at the beginning of the function
    index++; 
    // We check if there is no possibility of straight,
    if (val - hand->cards[index]->value > 1) { // since the hand is sorted there should be no gap in case of a straight
        return 0;
    }
      
    // In case we have not returned check if the current  card should be count into the  straight  
    if (step_to_straight(hand, index, val, fs)) { 
      val--;
      count++;
    } 
    // Return if we have found a n-straight. 
    // It is crucial to check this here at the very end of the loop and NOT at the vary start of the loop.
    // Note, if the last card of the straight has index hand->n_cards -1,
    // then the condition (index < hand->card - 1) will prevent prevent from returning into the loop.
    // Hence, in this case, we counted a n-straight, but never returned 1.
    // This is also the reason that treated the 1-straight separately.  
    if (count == n) {
      return 1;
    } 
  }
  return 0;
}

int is_straight_at(deck_t * hand, size_t index, suit_t fs) {
  if (is_nlength_straight_at(hand, index, fs, 5)) {
    return 1;
  }
  // If a straight is not found, check the possibility of a low ace straight 
  if (hand->cards[index]->value == VALUE_ACE) { 
    // Run through the cards
    for (size_t i = index + 1; i <= hand->n_cards - 4; i++) { 
      // when a card of value 5 is found
      if (hand->cards[i]->value == 5) { 
        // Check for a 4-straight beginning at this index if:
        // fs = NUM_SUITS or fs = hand->cards[i]->suit
        if ( fs == NUM_SUITS || (fs == hand->cards[i]->suit && fs == hand->cards[index]->suit) ) {
          return -1*is_nlength_straight_at(hand, i, fs, 4);
        }
      }
      else if (hand->cards[i]->value < 5) {
        return 0;
      }
    } // for end
  }
  return 0;
}


// note that the input (hand, 0 NOTHING, 0) should build a hand with the first 5 cards
// this case should be handled separately
hand_eval_t build_hand_from_match(deck_t * hand,
				  unsigned n,
				  hand_ranking_t what,
				  size_t idx) {
  // assert that we are going to write 5 cards on ans.cards
  assert(n<6);
  // assert that there are enough cards in the hand to write on ans.cards
  assert(hand->n_cards > 4);
  // assert that built_hand_from_match is used only for 
  // NOTHING, PAIR, TWO_PAIR,THREE_OF_A_KIND, FULL_HOUSE, FOUR_OF_A_KIND
  assert(what == NOTHING || what == TWO_PAIR || what == PAIR ||
      what == THREE_OF_A_KIND || what == FULL_HOUSE || what == FOUR_OF_A_KIND);

  hand_eval_t ans;
  ans.ranking = what;

  if (what == NOTHING) {
    for (size_t i = 0; i < 5; i++) {
      ans.cards[i] = hand->cards[i];
    }
  }
  else {
    // Write to ans.cards[0]..ans.cards[n-1]
    // the hand->cards[idx].. hand->cards[idx+ n-1].
    for (size_t i = 0; i < n; i++) {
      ans.cards[i] = hand->cards[idx + i];
    }
    // Fill the positions ans.cards[n]..ans.cards[5]
    // with the first 5-n cards not in the n_of_a_kind.
    unsigned j = 0;
    for (size_t i = 0; i < hand->n_cards; i++) {
      if (n+j > 4) {
        break;
      }
      // write card from hand if it is not in n_of_a_kind
      if (hand->cards[idx]->value != hand->cards[i]->value) {
        ans.cards[n+j] = hand->cards[i];
        j++;
      }
    }
    assert(n+j == 5);
  }
  return ans;
}

// Hand sort
void sort_hand(deck_t * hand) {
  qsort(hand->cards, hand->n_cards, sizeof(card_t *), card_ptr_comp);
}



int compare_hands(deck_t * hand1, deck_t * hand2) {

  sort_hand(hand1); 
  sort_hand(hand2);


  hand_eval_t hand1_eval = evaluate_hand(hand1);
  hand_eval_t hand2_eval = evaluate_hand(hand2);

  if (hand1_eval.ranking != hand2_eval.ranking) { 
    return hand2_eval.ranking - hand1_eval.ranking; //rankings in enum are in descending order
  }
  else {
    for (unsigned i = 0; i < 5; i++) {
      if (hand1_eval.cards[i]->value != hand2_eval.cards[i]->value) {
        return hand1_eval.cards[i]->value - hand2_eval.cards[i]->value;
      }
    }
  } 
 return 0;
}

//You will write this function in Course 4.
//For now, we leave a prototype (and provide our
//implementation in eval-c4.o) so that the
//other functions we have provided can make
//use of get_match_counts.
unsigned * get_match_counts(deck_t * hand) {
  
  unsigned * match_counts = malloc(hand->n_cards*sizeof(*match_counts));
  size_t i = 0;
  while ( i < hand->n_cards) {
    unsigned c_value = hand->cards[i]->value;
    unsigned count = 1;
    size_t j = i+1;
    while (j < hand->n_cards) {
      if (c_value == hand->cards[j]->value) {
        count++;
      }
      else {
        break;
      }
      j++;
    }
    for (size_t x = i; x<j; x++) {
      match_counts[x] = count;
    }
    i = j;
  }
  return match_counts;  
}




// We provide the below functions.  You do NOT need to modify them
// In fact, you should not modify them!


//This function copies a straight starting at index "ind" from deck "from".
//This copies "count" cards (typically 5).
//into the card array "to"
//if "fs" is NUM_SUITS, then suits are ignored.
//if "fs" is any other value, a straight flush (of that suit) is copied.
void copy_straight(card_t ** to, deck_t *from, size_t ind, suit_t fs, size_t count) {
  assert(fs == NUM_SUITS || from->cards[ind]->suit == fs);
  unsigned nextv = from->cards[ind]->value;
  size_t to_ind = 0;
  while (count > 0) {
    assert(ind < from->n_cards);
    assert(nextv >= 2);
    assert(to_ind <5);
    if (from->cards[ind]->value == nextv &&
	(fs == NUM_SUITS || from->cards[ind]->suit == fs)){
      to[to_ind] = from->cards[ind];
      to_ind++;
      count--;
      nextv--;
    }
    ind++;
  }
}


//This looks for a straight (or straight flush if "fs" is not NUM_SUITS)
// in "hand".  It calls the student's is_straight_at for each possible
// index to do the work of detecting the straight.
// If one is found, copy_straight is used to copy the cards into
// "ans".
int find_straight(deck_t * hand, suit_t fs, hand_eval_t * ans) {
  if (hand->n_cards < 5){
    return 0;
  }
  for(size_t i = 0; i <= hand->n_cards -5; i++) {
    int x = is_straight_at(hand, i, fs);
    if (x != 0){
      if (x < 0) { //ace low straight
	ans->cards[4] = hand->cards[i];
	size_t cpind = i+1;
	while(hand->cards[cpind]->value != 5 ||
	      !(fs==NUM_SUITS || hand->cards[cpind]->suit ==fs)){
	  cpind++;
	  assert(cpind < hand->n_cards);
	}
	copy_straight(ans->cards, hand, cpind, fs,4) ;
      }
      else {
	copy_straight(ans->cards, hand, i, fs,5);
      }
      return 1;
    }
  }
  return 0;
}


//This function puts all the hand evaluation logic together.
//This function is longer than we generally like to make functions,
//and is thus not so great for readability :(
hand_eval_t evaluate_hand(deck_t * hand) {
  suit_t fs = flush_suit(hand);
  hand_eval_t ans;
  if (fs != NUM_SUITS) {
    if(find_straight(hand, fs, &ans)) {
      ans.ranking = STRAIGHT_FLUSH;
      return ans;
    }
  }
  unsigned * match_counts = get_match_counts(hand);
  unsigned n_of_a_kind = get_largest_element(match_counts, hand->n_cards);
  assert(n_of_a_kind <= 4);
  size_t match_idx = get_match_index(match_counts, hand->n_cards, n_of_a_kind);
  ssize_t other_pair_idx = find_secondary_pair(hand, match_counts, match_idx);
  free(match_counts);
  if (n_of_a_kind == 4) { //4 of a kind
    return build_hand_from_match(hand, 4, FOUR_OF_A_KIND, match_idx);
  }
  else if (n_of_a_kind == 3 && other_pair_idx >= 0) {     //full house
    ans = build_hand_from_match(hand, 3, FULL_HOUSE, match_idx);
    ans.cards[3] = hand->cards[other_pair_idx];
    ans.cards[4] = hand->cards[other_pair_idx+1];
    return ans;
  }
  else if(fs != NUM_SUITS) { //flush
    ans.ranking = FLUSH;
    size_t copy_idx = 0;
    for(size_t i = 0; i < hand->n_cards;i++) {
      if (hand->cards[i]->suit == fs){
	ans.cards[copy_idx] = hand->cards[i];
	copy_idx++;
	if (copy_idx >=5){
	  break;
	}
      }
    }
    return ans;
  }
  else if(find_straight(hand,NUM_SUITS, &ans)) {     //straight
    ans.ranking = STRAIGHT;
    return ans;
  }
  else if (n_of_a_kind == 3) { //3 of a kind
    return build_hand_from_match(hand, 3, THREE_OF_A_KIND, match_idx);
  }
  else if (other_pair_idx >=0) {     //two pair
    assert(n_of_a_kind ==2);
    ans = build_hand_from_match(hand, 2, TWO_PAIR, match_idx);
    ans.cards[2] = hand->cards[other_pair_idx];
    ans.cards[3] = hand->cards[other_pair_idx + 1];
    if (match_idx > 0) {
      ans.cards[4] = hand->cards[0];
    }
    else if (other_pair_idx > 2) {  //if match_idx is 0, first pair is in 01
      //if other_pair_idx > 2, then, e.g. A A K Q Q
      ans.cards[4] = hand->cards[2];
    }
    else {       //e.g., A A K K Q
      ans.cards[4] = hand->cards[4];
    }
    return ans;
  }
  else if (n_of_a_kind == 2) {
    return build_hand_from_match(hand, 2, PAIR, match_idx);
  }
  return build_hand_from_match(hand, 0, NOTHING, 0);
}
