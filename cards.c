#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cards.h"


unsigned is_card(card_t c) {
 return HAS_VALID_VAL(c.value) && HAS_VALID_SUIT(c.suit);
}

void assert_card_valid(card_t c) {
	assert(is_card(c));
}

const char * ranking_to_string(hand_ranking_t r) {
	switch(r) {
	case STRAIGHT_FLUSH:
		return "STRAIGHT_FLUSH";
	case FOUR_OF_A_KIND:
		return "FOUR_OF_A_KIND";
	case FULL_HOUSE:
		return "FULL_HOUSE";
	case FLUSH:
		return "FLUSH";
	case STRAIGHT:
		return "STRAIGHT";
	case THREE_OF_A_KIND:
		return "THREE_OF_A_KIND";
	case TWO_PAIR:
		return "TWO_PAIR";
	case PAIR:
		return "PAIR";
	case NOTHING:
		return "NOTHING";
	}
  return "?";
}

char value_letter(card_t c) {
  if (1 < c.value && c.value < 10) {
 // the digits '0'..'9' have ascii code 48+digit, in particular '0' has ascii code 48.
    return c.value + 48;
  }
  switch (c.value) {
  case 10: 
    return '0';
  case VALUE_JACK:
    return 'J';
  case VALUE_QUEEN:
    return 'Q';
  case VALUE_KING:
    return 'K';
  case VALUE_ACE:
    return 'A';
  default :
    return '?';
  }
}

char suit_letter(card_t c) {
  switch(c.suit){
  case SPADES:
    return 's';
  case HEARTS:
    return 'h';
  case DIAMONDS:
    return 'd';
  case CLUBS:
    return 'c';
  case NUM_SUITS:
    return '?';
  default:
    return '?';
  }
}

void print_card(card_t c) {
  char value_let;
  char suit_let;
  value_let = value_letter(c);
  suit_let = suit_letter(c);
  printf("%c%c", value_let, suit_let);
}

card_t card_from_letters(char value_let, char suit_let) {
  card_t temp;
  switch(value_let) {
  case '0':
    temp.value = 10;
    break;
  case 'J':
    temp.value = VALUE_JACK;
    break;
  case 'Q':
    temp.value = VALUE_QUEEN;
    break;
  case 'K':
    temp.value = VALUE_KING;
    break;
  case 'A':
    temp.value = VALUE_ACE;
    break;
  default:
    temp.value = value_let - '0';
    break;
  }

  switch(suit_let) {
  case 's':
    temp.suit = SPADES;
    break;
  case 'h':
    temp.suit = HEARTS;
    break;
  case 'd':
    temp.suit = DIAMONDS;
    break;
  case 'c':
    temp.suit = CLUBS;
    break;
  default :
    temp.suit = NUM_SUITS; 
    break;
  }
  
  assert_card_valid(temp);
  return temp;
}

card_t card_from_num(unsigned c) {
  
  assert(0 <= c && c < 52);


  unsigned suit = c/13;
  card_t new_card;
 
  new_card.value = c%13 + 2;
  switch(suit) {
  case 0:
    new_card.suit = SPADES;
    break;
  case 1:
    new_card.suit = HEARTS;
    break;
  case 2:
    new_card.suit = DIAMONDS;
    break;
  case 3: 
    new_card.suit =  CLUBS;
    break;
  }
  return new_card;
}

const char * suit_string_from_suit_t(suit_t suit) {
  switch (suit) {
    case SPADES:
      return "spades";
    case DIAMONDS:
      return "diamonds";
    case CLUBS:
      return "clubs";
    case NUM_SUITS:
      return "no suit has enough  representatives";
    default: return "?";
  }
}
