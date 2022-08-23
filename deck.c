#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "deck.h"

void print_hand(deck_t * hand) {
  for (size_t i = 0; i < (hand->n_cards); i++) {
    print_card( *(hand->cards[i]) ); 
    printf(" ");
  }
}

int isEq_card_t(card_t c1, card_t c2) {
  if (c1.suit == c2.suit && c1.value == c2.value) {
    return 1;
  }
  return 0;
}

int deck_contains(deck_t * d, card_t c) {
  for (size_t i = 0 ; i < (d->n_cards); i++) {
    if ( isEq_card_t(*(d->cards[i]), c) ) {
      return 1;
    }
  }
  return 0;
}

// this implimintaion returns the occurances of c in *(d->cards)
int deck_contains_my(deck_t * d, card_t c) {
  int occur = 0;
  for (size_t i = 0 ; i < (d->n_cards); i++) {
    if ( isEq_card_t(*(d->cards[i]), c) ) {
      occur++;
    }
  }
  return occur;
}

// For random(), RAND_MAX

/* 
 * This function returns a random long int in the closed interval [0, bound],
 * 
 * using the random() function from stdlib.h
 *
 * random() returns a long int in the closed iterval [0, RAND_MAX]
 */

long random_bounded(long bound) {
/*
 * Programmer should make sure to be
 *   bound <= RAND_MAX 
 * 
 * Since
 *   RAND_MAX < ULONG_MAX,
 * we may work with unsigned long 
 *
 * Algorithm description:
 * ---------------------
 *
 * - We create (bound + 1) equally sized buckets
 * of size 
 *  bucket_size =  (RAND_MAX +1) / (bound + 1)
 *
 * - We may need to creat and a defect_bucket 
 * of size (RAND_MAX + 1) % (bound + 1)
 *
 * - A long int random = random()
 * fits in exactly one bucket, according to the result 
 *   random / bucket_size (as long int division) 
 * 
 * - Return 
 *     random_bounded = random / bucket_size 
 *
 */

 unsigned long buckets = (unsigned long) bound + 1;
 unsigned long range = (unsigned long) RAND_MAX + 1;
 unsigned long bucket_size = range / buckets;
 unsigned long defect = range % buckets;

 long r_num;
  do {
   r_num = random();
  } // This is carefully written not to overflow
  while (range - defect <= (unsigned long)random);

  // Truncated division is intentional
  return r_num/bucket_size;
}

void suffle_helper(deck_t * d) {
// this function will implement the Fisher-Yates shuffle algorithm

  card_t * temp;
  for (size_t i = (d->n_cards)-1; i > 0; i--) {

    size_t r_index = (size_t) random_bounded((long)i);

    temp = d->cards[i];
    d->cards[i] = d->cards[r_index];
    d->cards[r_index] = temp;
  }
}  
  
void shuffle(deck_t * d){
  // suffle deck 10 times
  for (int times = 0; times  < 10; times++) {
    suffle_helper(d);    
  }
}

void assert_full_deck_my(deck_t * d) {
  if (d->n_cards != 52) {
    printf("The deck has not 52 cards\n");
    printf("INVALID_DECK\nabort\n");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < 52; i++) {
    card_t c = card_from_num(i);
    if (  (d->cards[i])->value < 2 || (d->cards[i])->value > VALUE_ACE || (d->cards[i])->suit < SPADES || (d->cards[i])->suit >  CLUBS ) {
      printf("The deck contains an invalid card\n");
      printf("INVALID_DECK\nabort\n");
      exit(EXIT_FAILURE);
    }
    int times = deck_contains_my(d,c);
    if (times == 0) {
      printf("The card ");
      print_card(c);
      printf(" is not found inside the deck\n");
      printf("INVALID_DECK\nabort\n");
      exit(EXIT_FAILURE);
    }
    if (times > 1) {
      printf("The card ");
      print_card(c);
      printf(" found %d times inside the deck\n", times);
      printf("INVALID_DECK\nabort\n");
      exit(EXIT_FAILURE);
    }
  }
  printf("valid deck\n");
}

void assert_full_deck(deck_t * d) {
  // printf("running assert_full_deck");
  assert(d->n_cards == 52);
  for (size_t i=0; i < d->n_cards; i++){
    assert_card_valid(*(d->cards[i]));
    assert(deck_contains(d, card_from_num(i))==1);
  }
}

/* The next functions are for the course 4 of the specialization */

void add_card_to(deck_t * deck, card_t c) {

  //allocate space for a card_t and assign the value c 
  card_t * card = malloc(sizeof(*card));
  card->value=c.value;
  card->suit=c.suit;

  //allocate extra space for deck->cards
  deck->cards = realloc(deck->cards, (deck->n_cards+1)*sizeof(*deck->cards));
  /* assuming the allocation was successful */
  //add card to deck
  deck->cards[deck->n_cards] = card;
  deck->n_cards += 1;
}


card_t * add_empty_card(deck_t * deck) {
  card_t c = { .value = -1, .suit = -1};
  add_card_to(deck, c);
  return deck->cards[deck->n_cards-1];
} 


deck_t * make_deck_exclude(deck_t * excluded_cards) {
  
  //A. Initialize an empty deck
  deck_t * d = malloc(sizeof(*d));
  d->cards = NULL;
  d->n_cards = 0;
  
  //B. fill the deck with any card not in excluded_cards
  for (size_t i = 0; i < 52; i++) {
    card_t c = card_from_num(i);
    int is_excluded = 0;
    for (size_t j = 0; j < excluded_cards->n_cards; j++) {
      if ( isEq_card_t(*excluded_cards->cards[j], c) ) {
        is_excluded = 1;
        break;
      }
    }
    if (is_excluded == 0) {
      add_card_to(d,c);
    }
  }
  return d; 
}


deck_t * build_remaining_deck(deck_t ** hands, size_t n_hands) {
  //A. Make the excluded
  //A.1 initialize to empty
  deck_t * excluded = malloc(sizeof(*excluded));
  excluded->cards = NULL;
  excluded->n_cards = 0;
  //A.2 add every card from each hand to excluded
  //    note: - it doesn't matter if there are duplicates,
  //            since we cannot exclude a card twice.
  //          - it does not matter if a hand has an empty card
  //            since empty cards are not members of a full set
  //          - it would be good to exclude those for performance issues
  
  for (size_t i = 0; i < n_hands; i++) {
    for (size_t j = 0; j < hands[i]->n_cards; j++) {
      add_card_to(excluded, *hands[i]->cards[j]);
    }
  }

  //B. Built remaining
  deck_t * remaining = make_deck_exclude(excluded);
 
  free_deck(excluded); 

  return remaining;
}


void free_deck(deck_t * deck) {
  for (size_t i = 0; i < deck->n_cards; i++) {
    free(deck->cards[i]);
  }
  free(deck->cards); 
  free(deck);
}
