#include <stdio.h>
#include <stdlib.h>
#include "future.h"


void add_future_card(future_cards_t * fc, size_t index, card_t * ptr) {
  //A. make fc big enough to handle the index  
  if (index >= fc->n_decks) {
    //A.1
    fc->decks = realloc(fc->decks, (index+1)*sizeof(*fc->decks));
    //A.2 initialiaze empty decks 
    for (size_t i = fc->n_decks; i <= index; i++) {
      fc->decks[i].n_cards = 0;
      fc->decks[i].cards = NULL;
    }
    //A.3 update the fc->n_decks
    fc->n_decks = index+1;
  }
  
  //B. add the card to fc->decks[index]
  //
  // fc -> n_decks (size_t)
  //       decks (deck_t *)
  //         |
  //         v
  //       n_cards (size_t)
  //       cards (card_t **)
  //         |
  //         v
  //        ptr (card_t *)
  //
  fc->decks[index].cards = realloc(fc->decks[index].cards, 
      (fc->decks[index].n_cards+1)*sizeof(*fc->decks[index].cards));
  
  fc->decks[index].cards[fc->decks[index].n_cards] = ptr;
  fc->decks[index].n_cards += 1; 
}


void future_cards_from_deck(deck_t * deck, future_cards_t *fc) {
  if (fc->n_decks > deck->n_cards) {
    fprintf(stderr, "not enough cards on deck");
    return;
  }
  for (size_t i = 0; i < fc->n_decks; i++) {
    for (size_t j = 0; j < fc->decks[i].n_cards; j++) {
      *fc->decks[i].cards[j] = *deck->cards[i];
    }
  }
}
