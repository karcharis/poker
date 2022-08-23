This a program to compute the wining probabilities
in a poker game. The probabilities are computed
using the Monte Carlo Method.  The program takes 
two arguments. The first is required, and is the
name of the input file to read. The second is
optional and is the number of Monte Carlo trials
to perform. If it is not given, 10,000 is used as
a default value. the second argument is not mandatory.
Plenty examples of valid input file may be found
in `./provided-tests` directory. Let us describe a
valid input file. Every line of a valid input file
should consist of a valid hand. A valid hand is a
sequence of cards separated by a space. A card may
be known or unknown, an unknown card is a card that
is going to be drawn from the deck. A known card
is represented by two characters. First character
is the value of the card, second character is the
suit of the card. The list of the valid card value
characters is:
 
'2', '3', '4', '5', '6', '7', '8', '9', '0', 'J', 'Q', 'K', 'A',

counting from 2 to ace. Note 0 represents the value
10\. The list of valid suit characters is: 

's', 'c', 'd', 'h',

for spades, clubs, diamonds, hearts respectively.
An unknown card is represented by the symbol '?' 
followed by the decimal representation of an integer.
The integer indicates the order that the unknown
card will be drawn from the deck. Next, non empty,
line is an example of a valid hand:

2d 5h Ac 4c ?0 ?2
