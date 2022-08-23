#include <stdio.h>
#include <stdlib.h>
#include "future.h"

#define IS_DIGIT(c) ('0' <= c && c <= '9')


ssize_t find_index(const char * str, size_t * p) {

  //A. create a buff
  //   to store the digits found from str[*p+1] until the first non digit
  char * num_str = NULL;
  ssize_t digits = 0;

  //B. Repeat until break
  while (1) {
    //B.1 Read str[*p+1] and decide:
    if (IS_DIGIT(str[*p + 1])) { //a. to read one more digit
     *p += 1;
      num_str = realloc(num_str, (digits+1)*sizeof(*num_str));
      num_str[digits] = str[*p];
      digits++;
    }
    else if ( (str[*p+1] == ' '|| str[*p+1] == '\n') && digits > 0) { //b. to break and return what is read
      break;
    }   
    else { //c. to return error
      return -1;
    }
  }

  //C. return
  //C.1. make num_str a valid string
  num_str = realloc(num_str, (digits+1)*sizeof(*num_str));
  num_str[digits] = '\0';
  
  //C.2 atoi num_str 
  size_t index  = (size_t) atoi(num_str);
  //C.3 free and return
  free(num_str);
  return index; 
  // NOTE: on return *p points to the last digit not to ' '
}


deck_t * hand_from_string(const char * str, future_cards_t * fc) {
  
  //A. initialize the return variable
  deck_t * hand = malloc(sizeof(*hand));
  hand->n_cards = 0;
  hand->cards = NULL;

  /* recall the format of a line should be of the form 
   *      
   *      v1s1 v2s2 v3s3 ... vnsn
   *
   * where:
   *       - vi is a valid card value, si is a valid suit value
   *       - for a unknown card vi=? and si=0.
   *       - value-suit pairs are seperate by a space
   *
   */
  //1. irretate until the end of the line
  //   recall: 'c1''c2'.. '\n''\0'
  size_t i = 0;
  while (str[i] != '\0') {

    if (str[i] == ' ' || str[i] == '\n') {
      i++;
    }
    else if (str[i] == '?') {
      ssize_t index = find_index(str, &i); //this moves also i to the last digit
      i += 1; //move to the separator
      add_future_card(fc, (size_t) index, add_empty_card(hand));     
    }
    else {
      add_card_to(hand, card_from_letters(str[i], str[i+1]));
      i += 2; //put i on separator
    }
  }
  
  return hand;
}


deck_t ** read_input(FILE * f, size_t * n_hands, future_cards_t * fc) {
  
  //A. initialize the rueturn variable
  deck_t ** hands = NULL;
  *n_hands = 0;
  //B. read file by line
  char * curr = NULL;
  size_t sz = 0;
  while (getline(&curr, &sz, f) > 1) {
    //B.1. save on c_hand the current line as a hand
    //B.1.1 initialiaze a pointer to an empty deck_t
    deck_t * c_hand = hand_from_string(curr, fc);
    
    //B.2 if c_hand = NULL or has less than 5 cards exit!..
    //    note: to avvoid segfault we check first if c_hand = NULL
    if (c_hand->n_cards < 5) {
      fprintf(stderr, "input file has line with less than 5 cards\n");
      continue;
    }    
    //..else add c_hand to hands
    hands = realloc(hands, (*n_hands+1)*sizeof(*hands));
    hands[*n_hands] = c_hand;
    *n_hands += 1;
  }
  free(curr);
  return hands;
}

