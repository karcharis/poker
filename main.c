#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include "cards.h"
#include "deck.h"
#include "eval.h"
#include "future.h"
#include "input.h"

void find_winning_hand(deck_t ** hands, size_t n_hands, unsigned * score) {
  int tie = 0;
  size_t win = 0;
  for (size_t i = 1; i < n_hands; i++) {
    int compare = compare_hands(hands[win], hands[i]);
    if (compare == 0) {
      tie =1;
    }  
    if (compare <  0) {
        win = i;
        tie = 0;
    }
  }
  if (tie) {
    score[n_hands] += 1;
  }
  else {
    score[win] += 1;
  }
}  


int main(int argc, char ** argv) {
  //A. check arguments and report erros
  int num_trials;
  if ( argc == 2 ) {
    num_trials = 10000;
  }
  else if (argc == 3) {
    num_trials = atoi(argv[2]);
    assert(num_trials > 0);
  }
  else {
    fprintf(stderr, "usage error: wrong number of argument");
    return EXIT_FAILURE;
  }

  //B.
  FILE * f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "input file has not opened properly. exit programm..");
    return EXIT_FAILURE;
  }

  //C. read input
  //C.1. create empty variables for 
  //deck_t ** read_input(FILE * f, size_t * n_hands, future_cards_t * fc) 
  size_t n_hands = 0;

  future_cards_t * fc = malloc(sizeof(*fc)); 
  fc->n_decks = 0;
  fc->decks = NULL;

  deck_t ** hands = read_input(f, &n_hands, fc);
  
  //C.2 close file
  if (fclose(f) != 0) {
    fprintf(stderr, "error: input file has not closed properly");
  }

  //C.3.
  if (hands == NULL) {
    free(fc);
    return EXIT_FAILURE;
  }
  
  //D. creat remaining deck
  deck_t * deck = build_remaining_deck(hands, n_hands);

  //E. create the score tamble
  unsigned score[n_hands+1];
  for (size_t i = 0; i < n_hands + 1; i++) {
    score[i] = 0;
  }

  //F. Sampling
  srandom(time(NULL));
  for (int i = 0; i < num_trials; i++) {

    shuffle(deck);

    future_cards_from_deck(deck, fc);

    //takes hands array and increases on score according to who wins
    find_winning_hand(hands, n_hands, score);
  }
  
  //G. printing
  for (size_t i = 0; i < n_hands; i++) {
    float ratio = 100*((float) score[i])/num_trials;
    printf("Hand %zu won %u / %u times (%.2f%%)\n", i, score[i], num_trials, ratio);
  }
  printf("And there were %u ties\n", score[n_hands]);

  //H. exit
  free_deck(deck);
  for(size_t i = 0; i < n_hands; i++) {
    free_deck(hands[i]);
  }
  free(hands);
  for (size_t i = 0; i < fc->n_decks; i++) {
    free(fc->decks[i].cards);
  }
  free(fc->decks);
  free(fc);
 
  return EXIT_SUCCESS;
}
