//*****************************************************************************
//* BLACK JACK the clessic card game. Made for the cromebook & Ubuntu OS.     *
//* By Johnny Buckallew Stroud    Project finish date: 6/24/2024              *
//* NOTES: Will have to remake this code in a text based game.                *
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// Define card constants
#define NUM_SUITS 4
#define NUM_VALUES 13
#define DECK_SIZE (NUM_SUITS * NUM_VALUES)
// ANSI color codes for background colors
#define RED_BACKGROUND "\x1b[41m"
#define GREEN_BACKGROUND "\x1b[42m"
#define YELLOW_BACKGROUND "\x1b[43m"
#define BLUE_BACKGROUND "\x1b[44m"
#define MAGENTA_BACKGROUND "\x1b[45m"
#define CYAN_BACKGROUND "\x1b[46m"
#define RESET_COLOR "\x1b[0m"

// Function to print a string with a specific background color
void printStringWithBackground(const char *str, const char *bgColor) {
    printf("%s%s%s", bgColor, str, RESET_COLOR);
}

// Function to print the BLACKJACK banner
void printBlackjackBanner() {
    const char *colors[] = {RED_BACKGROUND, GREEN_BACKGROUND, YELLOW_BACKGROUND, BLUE_BACKGROUND, MAGENTA_BACKGROUND, CYAN_BACKGROUND};
    int numColors = sizeof(colors) / sizeof(colors[0]);

    // ASCII art for BLACKJACK
    const char *banner[] = {
        "  BBBBB    L       A     CCCCC  K   K     J     A     CCCCC  K   K  ",
        "  B    B   L      A A   C       K  K      J    A A   C       K  K   ",
        "  BBBBB    L     AAAAA  C       KKK       J   AAAAA  C       KKK    ",
        "  B    B   L    A     A C       K  K  J   J  A     A C       K  K   ",
        "  BBBBB    LLLL A     A  CCCCC  K   K  JJJ  A     A  CCCCC   K   K  "
    };
   // Print the banner line by line
    for (int i = 0; i < sizeof(banner) / sizeof(banner[0]); i++) {
        for (int j = 0; banner[i][j] != '\0'; j++) {
            char c = banner[i][j];
            if (c != ' ') {
                printStringWithBackground(" ", colors[(i + j) % numColors]);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

// Structure for a single card
struct Card {
    char suit[10]; // Full name of the suit
    char value;
    int points;  // Points value for the card
    char name[20]; // Full name of the card
};

// Structure for the deck of cards
struct Deck {
    struct Card cards[DECK_SIZE];
    int num_cards;
};

// Global variables
struct Deck deck;
int player_score = 0;
int dealer_score = 0;
int player_chips = 10000;
int player_wins = 0;
int dealer_wins = 0;

// Initialize deck of cards
void init_deck() {
    char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    char values[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
    char *names[] = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", 
                     "Nine", "Ten", "Jack", "Queen", "King", "Ace"};

    int points[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};

    int index = 0;
    for (int s = 0; s < NUM_SUITS; s++) {
        for (int v = 0; v < NUM_VALUES; v++) {
            strcpy(deck.cards[index].suit, suits[s]);
            deck.cards[index].value = values[v];
            deck.cards[index].points = points[v];
            sprintf(deck.cards[index].name, "%s of %s", names[v], suits[s]);
            index++;
        }
    }
    deck.num_cards = DECK_SIZE;
}

// Shuffle deck of cards
void shuffle_deck() {
    srand(time(NULL));
    for (int i = deck.num_cards - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        struct Card temp = deck.cards[i];
        deck.cards[i] = deck.cards[j];
        deck.cards[j] = temp;
    }
}

// Deal a card from the deck
struct Card deal_card() {
    if (deck.num_cards <= 0) {
        printf("Deck is empty. Reshuffling...\n");
        init_deck();
        shuffle_deck();
    }

    struct Card card = deck.cards[deck.num_cards - 1];
    deck.num_cards--;
    return card;
}

// Calculate total points of a hand
int calculate_points(struct Card hand[], int num_cards) {
    int points = 0;
    int ace_count = 0;

    for (int i = 0; i < num_cards; i++) {
        points += hand[i].points;
        if (hand[i].value == 'A') {
            ace_count++;
        }
    }

    // Adjust for Aces being 1 or 11
    while (ace_count > 0 && points > 21) {
        points -= 10;
        ace_count--;
    }

    return points;
}

// Display player's and dealer's hands in text format
void display_table(struct Card player_hand[], int player_cards, struct Card dealer_hand[], int dealer_cards, int show_all) {
    printf("\nDealer's hand:\n");
    if (!show_all) {
        printf("Hidden Card\n");
        printf("%s\n", dealer_hand[1].name);
    } else {
        for (int i = 0; i < dealer_cards; i++) {
            printf("%s\n", dealer_hand[i].name);
        }
    }

    printf("\nPlayer's hand:\n");
    for (int i = 0; i < player_cards; i++) {
        printf("%s\n", player_hand[i].name);
    }
    printf("\n");
}

// Game logic for player's turn
void player_turn(struct Card player_hand[], int *player_cards) {
    char choice;
    do {
        printf("Do you want to (h)it or (s)tand? ");
        scanf(" %c", &choice);
        if (choice == 'h') {
            player_hand[*player_cards] = deal_card();
            (*player_cards)++;
            display_table(player_hand, *player_cards, deck.cards, 1, 0);
        }
    } while (choice != 's' && calculate_points(player_hand, *player_cards) < 21);
}

// Game logic for dealer's turn
void dealer_turn(struct Card dealer_hand[], int *dealer_cards) {
    while (calculate_points(dealer_hand, *dealer_cards) < 17) {
        dealer_hand[*dealer_cards] = deal_card();
        (*dealer_cards)++;
        display_table(deck.cards, 1, dealer_hand, *dealer_cards, 0);
    }
}

// Determine winner and display results
void determine_winner(struct Card player_hand[], int player_cards, struct Card dealer_hand[], int dealer_cards, int bet) {
    int player_points = calculate_points(player_hand, player_cards);
    int dealer_points = calculate_points(dealer_hand, dealer_cards);

    printf("\nPlayer's points: %d\n", player_points);
    printf("Dealer's points: %d\n", dealer_points);

    if ((player_points <= 21 && player_points > dealer_points) || (dealer_points > 21 && player_points <= 21)) {
        printf("Player wins!\n");
        player_wins++;
        player_chips += bet;
    } else {
        printf("Dealer wins!\n");
        dealer_wins++;
        player_chips -= bet;
    }
}

// Get a random ending message
void random_ending_message() {
    char *messages[] = {
        "Thanks for playing loser!",
        "Better luck next time loser!",
        "You're such a loser and A jackass!",
        "Come back soon with more money scum!",
        "What a game! See your mark ass on the flip side.",
        "The cards have spoken! You're not a Winner today.",
        "Until next time! Broke ass mark.",
        "Fate is a fickle friend!",
        "Thanks for playing! Remember, it's all in the cards!",
        "Better luck next time! The deck can be a cruel mistress.",
        "Congratulations! Your fortune has been shuffled and dealt.",
        "That was quite the streek of bad luck! Come back soon for another round!",
        "Fate loves a good game of blackjack. Until next time!",
        "The cards have spoken! you're not lucky today."
    };

    int num_messages = sizeof(messages) / sizeof(messages[0]);
    int index = rand() % num_messages;

    printf("%s\n", messages[index]);
}


// Main function to run the game
int main() {
    system("clear");
    printf("  Welcome to,\n");
    printBlackjackBanner();
    sleep(2);

    char play_again;
    do {
        system("clear");
        printBlackjackBanner();
        printf("\n");
        init_deck();
        shuffle_deck();

        struct Card player_hand[10];  // Max 10 cards per hand
        struct Card dealer_hand[10];  // Max 10 cards per hand
        int player_cards = 0;
        int dealer_cards = 0;
        int bet;

        printf("You currently have %d chips.\n", player_chips);

        // Prompt for bet
        do {
            printf("Place your bet (minimum 1 chip, maximum %d chips): ", player_chips);
            scanf("%d", &bet);
        } while (bet < 1 || bet > player_chips);

        // Initial deal
        player_hand[player_cards++] = deal_card();
        dealer_hand[dealer_cards++] = deal_card();
        player_hand[player_cards++] = deal_card();
        dealer_hand[dealer_cards++] = deal_card();

        display_table(player_hand, player_cards, dealer_hand, dealer_cards, 0);

        // Player's turn
        player_turn(player_hand, &player_cards);

        // Dealer's turn
        dealer_turn(dealer_hand, &dealer_cards);

        // Determine winner
        determine_winner(player_hand, player_cards, dealer_hand, dealer_cards, bet);

        printf("\nPlayer wins: %d, Dealer wins: %d\n", player_wins, dealer_wins);

        // Check if player has chips left to continue playing
        if (player_chips <= 0) {
            printf("You have run out of chips. Game over.\n");
            break;
        }

       printf("\nAre you ready to play another hand ? (y/n) ");
       scanf(" %c", &play_again);
       system("clear");
        printBlackjackBanner();
      } while (play_again == 'y' || play_again == 'Y');
    random_ending_message();
    return 0;
}
