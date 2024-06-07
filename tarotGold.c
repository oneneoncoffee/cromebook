// Based on the classic Tarot Gold board game deck of 78 cards.  
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Struct to hold a tarot card
typedef struct {
    char *name;
    char *description;
    int drawn; // Flag to mark if the card has been drawn
} TarotCard;

// Function to clear the screen (works on Unix/Linux/Mac)
void clearScreen() {
    printf("\e[1;1H\e[2J");
}

// Function to print a random welcome message
void printWelcomeMessage() {
    char *messages[] = {       
        "Welcome to your tarot card reading!", 
        "Welcome to your tarot card reading! Let's discover the secrets of the universe together.",
        "Greetings! May the wisdom of the tarot illuminate your path.",
        "Hello! Prepare to embark on a journey of self-discovery with the tarot as your guide.",
        "Welcome! The tarot cards await, ready to reveal the mysteries of your destiny.",
        "Salutations! May the cards unveil the truths hidden within your soul.",
        "Greetings, seeker! Let the ancient wisdom of the tarot reveal what the future holds for you.",
        "Hello, friend! Get ready to explore the depths of your subconscious through the wisdom of the tarot.",
        "Welcome, traveler! The tarot cards are poised to unravel the threads of fate that bind us all.",
        "Greetings, fellow adventurer! Prepare to delve into the realms of possibility with the tarot as your map.",       
        "Greetings! Let's unveil your destiny.",
        "What do the cards have in store for you today?",
        "Hello! What do the cards have in store for you today?",
        "Let's discover what the future may hold today.",
        "Welcome! Let's discover what the future holds."
    };
    int numMessages = sizeof(messages) / sizeof(messages[0]);
    printf("%s\n\n", messages[rand() % numMessages]);
}

// Function to shuffle the tarot deck
void shuffleDeck(TarotCard deck[], int size) {
    for (int i = 0; i < size; i++) {
        int j = rand() % size;
        TarotCard temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// Function to draw tarot cards
void drawTarotCards(TarotCard deck[], int numCards) {
    char *positions[] = { "upright", "reversed" };

    printf("Your Tarot Card Reading:\n\n");
    for (int i = 0; i < numCards; i++) {
        int index;
        do {
            index = rand() % 78; // Random index within the deck size
        } while (deck[index].drawn); // Repeat until an undrawn card is found

        if (deck[index].drawn == 1) {
            printf("The dealer fumbles and drops some cards on the table.\n\n");
            // Reset all drawn flags
            for (int j = 0; j < 78; j++) {
                deck[j].drawn = 0;
            }
            // Reshuffle the deck
            shuffleDeck(deck, 78);
            // Redraw the current card
            i--;
            continue;
        }

        TarotCard card = deck[index];
        card.drawn = 1; // Mark the card as drawn
        char *position = positions[rand() % 2];
        printf("Card %d: %s (%s)\n", i + 1, card.name, position);
        printf("Description: %s\n\n", card.description);
    }
}

int main() {
    srand((unsigned int)time(NULL));

    clearScreen();
    printWelcomeMessage();

// Complete tarot deck with 78 cards
TarotCard deck[78] = {
    {"The Fool", "Beginnings, innocence, spontaneity, a free spirit"},
    {"The Magician", "Power, skill, concentration, action, resourcefulness"},
    {"The High Priestess", "Intuition, higher powers, mystery, subconscious mind"},
    {"The Empress", "Fertility, femininity, beauty, nature, abundance"},
    {"The Emperor", "Authority, father-figure, structure, solid foundation"},
    {"The Hierophant", "Religion, group identification, conformity, tradition, beliefs"},
    {"The Lovers", "Love, union, relationships, values alignment, choices"},
    {"The Chariot", "Control, willpower, victory, assertion, determination"},
    {"Strength", "Strength, courage, patience, control, compassion"},
    {"The Hermit", "Soul-searching, introspection, being alone, inner guidance"},
    {"Wheel of Fortune", "Good luck, karma, life cycles, destiny, a turning point"},
    {"Justice", "Justice, fairness, truth, cause and effect, law"},
    {"The Hanged Man", "Suspension, restriction, letting go, sacrifice"},
    {"Death", "Endings, beginnings, change, transformation, transition"},
    {"Temperance", "Balance, moderation, patience, purpose, meaning"},
    {"The Devil", "Bondage, addiction, sexuality, materialism"},
    {"The Tower", "Disaster, upheaval, sudden change, revelation"},
    {"The Star", "Hope, spirituality, renewal, inspiration, serenity"},
    {"The Moon", "Illusion, fear, anxiety, insecurity, subconscious"},
    {"The Sun", "Fun, warmth, success, positivity, vitality"},
    {"Judgement", "Judgement, rebirth, inner calling, absolution"},
    {"The World", "Completion, integration, accomplishment, travel"},
    {"Ace of Wands", "Inspiration, power, creation, beginnings, potential"},
    {"Two of Wands", "Future planning, progress, decisions, discovery"},
    {"Three of Wands", "Preparation, foresight, enterprise, expansion"},
    {"Four of Wands", "Celebration, harmony, marriage, home, community"},
    {"Five of Wands", "Disagreement, competition, strife, tension, conflict"},
    {"Six of Wands", "Public recognition, victory, progress, self-confidence"},
    {"Seven of Wands", "Challenge, competition, protection, perseverance"},
    {"Eight of Wands", "Speed, action, air travel, movement, swift change"},
    {"Nine of Wands", "Courage, persistence, test of faith, resilience"},
    {"Ten of Wands", "Burden, responsibility, hard work, stress, achievement"},
    {"Page of Wands", "Enthusiasm, exploration, discovery, free spirit"},
    {"Knight of Wands", "Energy, passion, inspired action, adventure, impulsiveness"},
    {"Queen of Wands", "Exuberance, warmth, vibrancy, determination"},
    {"King of Wands", "Natural-born leader, vision, entrepreneur, honor"},
    {"Ace of Cups", "Love, compassion, creativity, overwhelming emotion"},
    {"Two of Cups", "Unified love, partnership, attraction, relationships"},
    {"Three of Cups", "Celebration, friendship, creativity, community"},
    {"Four of Cups", "Meditation, contemplation, apathy, reevaluation"},
    {"Five of Cups", "Loss, regret, disappointment, despair, bereavement"},
    {"Six of Cups", "Reunion, nostalgia, childhood memories, innocence"},
    {"Seven of Cups", "Fantasy, illusion, wishful thinking, choices, imagination"},
    {"Eight of Cups", "Disappointment, abandonment, withdrawal, escapism"},
    {"Nine of Cups", "Contentment, satisfaction, gratitude, wish come true"},
    {"Ten of Cups", "Harmony, marriage, happiness, alignment, blissful relationships"},
    {"Page of Cups", "Creative opportunities, intuitive messages, curiosity, possibility"},
    {"Knight of Cups", "Romance, charm, imagination, beauty"},
    {"Queen of Cups", "Compassion, calm, comfort, emotional security"},
    {"King of Cups", "Emotional balance, control, generosity"},
    {"Ace of Swords", "Breakthrough, clarity, sharp mind"},
    {"Two of Swords", "Indecision, choices, stalemate, truce"},
    {"Three of Swords", "Heartbreak, emotional pain, sorrow, grief, hurt"},
    {"Four of Swords", "Rest, relaxation, meditation, contemplation, recuperation"},
    {"Five of Swords", "Conflict, disagreements, defeat, winning at all costs, betrayal"},
    {"Six of Swords", "Transition, change, rite of passage, releasing baggage"},
    {"Seven of Swords", "Betrayal, deception, getting away with something, stealth"},
    {"Eight of Swords", "Isolation, self-imposed restriction, imprisonment"},
    {"Nine of Swords", "Nightmares, anxiety, worry, fear, depression"},
    {"Ten of Swords", "Painful endings, deep wounds, betrayal, loss, crisis"},
    {"Page of Swords", "New ideas, curiosity, thirst for knowledge, new ways of communicating"},
    {"Knight of Swords", "Ambitious, action-oriented, driven to succeed, fast-thinking"},
    {"Queen of Swords", "Independent, unbiased judgement, clear boundaries, direct communication"},
    {"King of Swords", "Mental clarity, intellectual power, authority, truth"},
    {"Ace of Pentacles", "Manifestation, new financial opportunity, prosperity"},
    {"Two of Pentacles", "Balance, adaptability, time management, prioritization"},
    {"Three of Pentacles", "Teamwork, collaboration, learning, implementation"},
    {"Four of Pentacles", "Saving money, security, conservatism, scarcity, control"},
    {"Five of Pentacles", "Financial loss, poverty, lack mindset, isolation, worry"},
    {"Six of Pentacles", "Giving, receiving, sharing wealth, generosity, charity"},
    {"Seven of Pentacles", "Long-term view, sustainable results, perseverance, investment"},
    {"Eight of Pentacles", "Apprenticeship, repetitive tasks, mastery, skill development"},
    {"Nine of Pentacles", "Abundance, luxury, self-sufficiency, financial independence"},
    {"Ten of Pentacles", "Wealth, inheritance, family, establishment, retirement"},
    {"Page of Pentacles", "Manifestation, financial opportunity, skill development"},
    {"Knight of Pentacles", "Hard work, productivity, routine, conservatism"},
    {"Queen of Pentacles", "Practical, homely, motherly, down-to-earth, security"},
    {"King of Pentacles", "Wealth, business, leadership, security, discipline, abundance"}
};
    int numCards;
    printf("Would you like to draw a standard 3 or 5 card draw? \n(Enter 3 or 5, or any other number for a custom draw): ");
    scanf("%d", &numCards);
        // Check if the user wants a standard draw
    if (numCards == 3 || numCards == 5) {
        printf("Drawing a standard %d card draw:\n", numCards);
    } else {
    printf("Just to be clear now %d are you really sure?\nHow many tarot cards would you like to draw then? \n(1-78): ", numCards);
    scanf("%d", &numCards);
    printf("Very well then %d cards I will now draw.\n\n", numCards);
    }
     // Validate the user input
    if (numCards < 1 || numCards > 78) {
        printf("Invalid number of cards. Please enter a number between 1 and 78.\n");
        return 1;
    }
    // Shuffle and draw cards
    shuffleDeck(deck, 78);
    drawTarotCards(deck, numCards);

    return 0;
}
