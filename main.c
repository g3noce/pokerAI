#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct card{
    int value;
    char suit;
    int already_taken;
};

struct player_attrib{
    int player_num;
    int position;
    struct card hand[2];
    float tokens;
};

int compare_cards_value(const void *a, const void *b) {
    const struct card *card_a = (const struct card*)a;
    const struct card *card_b = (const struct card*)b;
    return card_a->value - card_b->value;
}

int compare_cards_suit(const void *a, const void *b) {
    const struct card *card_a = (const struct card*)a;
    const struct card *card_b = (const struct card*)b;
    return card_a->suit - card_b->suit;
}

struct card* create_deck(){
    struct card* deck = malloc(52 * sizeof(struct card));

    // initialize the cards
    for (int i = 0; i < 52; i++) {
        deck[i].value = (i % 13) + 2;
        deck[i].already_taken = 0;
        if (i < 13)
            deck[i].suit = 'C'; // carreaux
        else if (i < 26)
            deck[i].suit = 'T'; // trefles
        else if (i < 39)
            deck[i].suit = 'H'; // coeurs
        else 
            deck[i].suit = 'P'; // pique
    }
    return deck;
}

double time_ns(){
    static struct timespec start_time = {0, 0};
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    double elapsed_ns = (double)(current_time.tv_sec - start_time.tv_sec) * 1000000000 + (current_time.tv_nsec - start_time.tv_nsec);
    if (elapsed_ns >= 50000000000000) {
        start_time = current_time;
        elapsed_ns = 0;
    }
    return elapsed_ns;
}

struct card take_alt_card(struct card* deck52){
    double seed = time_ns();
    srand(seed);
    struct card selected_card;
    int alt_num = rand() % 52;
    if(alt_num < 0) 
        alt_num = alt_num * -1;
    while(deck52[alt_num].already_taken == 1){
        alt_num = rand() % 52;
    }
    selected_card = deck52[alt_num];
    deck52[alt_num].already_taken = 1;
    return selected_card;
}

struct player_attrib* set_money_player(struct player_attrib* num_player){
    for (int i = 0; i < 8; ++i){
        num_player[i].tokens = 10000.0;
    }
    return num_player;
}

struct player_attrib* utg_positions(struct player_attrib* positions, int n){
    if(n > 7 | n < 0)
        return 0;
    int player_num = n;
    int pos[] = { 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7}; // "Under the gun", "Middle position", "Middle position", "Middle position", "Cut-off", "Button", "Small blind", "Big blind"

    for (int i = 0; i < 8; i++) {
        positions[i].player_num = player_num;
        positions[i].position = pos[i];
        player_num = (player_num + 1) % 8;
    }
    return positions;
}

struct player_attrib* set_hand_cards(struct player_attrib* positions, struct card* deck52){
    for (int i = 0; i < 8; ++i){

        positions[i].hand[0] = take_alt_card(deck52);

        positions[i].hand[1] = take_alt_card(deck52);
    }
    return positions;
}

struct card* pair(struct card* seven_cards){
    struct card* pair = malloc(2 * sizeof(struct card));
    int i = 0;
    pair[0].value = -1;
    pair[0].suit = 'n';
    pair[1].value = -1;
    pair[1].suit = 'n';

    int found = 0;

    for (i = 0; i < 6; ++i){
        if (seven_cards[i].value == seven_cards[i+1].value && seven_cards[i].value > pair[0].value ){
            pair[0] = seven_cards[i];
            pair[1] = seven_cards[i+1];
            found = 1;
        }
    }
    if (found == 1){
        return pair;
    }
    else{
        free(pair);
        return NULL;
    }
}

struct card* two_pair(struct card* seven_cards){
    struct card* pairs = malloc(2 * sizeof(struct card));
    pairs[0].value = -1;
    pairs[0].suit = 'n';
    pairs[1].value = -1;
    pairs[1].suit = 'n';

    struct card* first_pair_check = malloc(2 * sizeof(struct card));
    first_pair_check = pair(seven_cards);

    if (first_pair_check == NULL) {
        free(first_pair_check);
        free(pairs);
        return NULL;
    }

    struct card* five_cards = malloc(5 * sizeof(struct card));
    int j = 0;

    for (int i = 0; i < 7; ++i) {
        if (seven_cards[i].value != first_pair_check[0].value) {
            five_cards[j] = seven_cards[i];
            j++;
        }
    }

    int found = 0;

    for (int i = 0; i < 4; ++i) {
        if (five_cards[i].value == five_cards[i + 1].value && five_cards[i].value > first_pair_check[0].value) {
            pairs[0] = five_cards[i];
            pairs[1] = five_cards[i + 1];
            found = 1;
            break;
        }
    }

    free(first_pair_check);
    free(five_cards);

    if (found == 1)
        return pairs;
    else {
        free(pairs);
        return NULL;
    }
}

struct card* three_of_a_kind(struct card* seven_cards){
    struct card* three;
    three = malloc(3 * sizeof(struct card));
    int i,found = 0;

    three[0].value = -1;
    three[0].suit = 'n';
    three[1].value = -1;
    three[1].suit = 'n';
    three[2].value = -1;
    three[2].suit = 'n';

    for (i = 0; i < 5; ++i){
        if (seven_cards[i].value == seven_cards[i+1].value && seven_cards[i].value == seven_cards[i+2].value && seven_cards[i].value > three[0].value ){
            three[0] = seven_cards[i];
            three[1] = seven_cards[i+1];
            three[2] = seven_cards[i+2];
            found = 1;
        }
    }

    if (found == 1)
        return three;
    else{
        free(three);
        return NULL;
    }
}

struct card* straight(struct card* seven_cards) {
    struct card* seven_cards_loc = malloc(7 * sizeof(struct card));
    int i,j,found = 0;
    for (i = 0; i < 7; ++i) seven_cards_loc[i] = seven_cards[i];
    struct card* straight = malloc(5*sizeof(struct card));

    i,j = 0;
    for (i = 0; i < 5; ++i){
        straight[i].value = -1;//if it not straight
        straight[i].suit = 'n';
    }

    i,j = 0;
    int has_2 = 0, has_3 = 0, has_4 = 0, has_14 = 0;
    for (i = 0; i < 7; ++i) {
        if (seven_cards_loc[i].value == 2) has_2 = 1;
        if (seven_cards_loc[i].value == 3) has_3 = 1;
        if (seven_cards_loc[i].value == 4) has_4 = 1;
        if (seven_cards_loc[i].value == 14) has_14 = 1;
    }

    i,j = 0;
    if (has_2 && has_3 && has_4 && has_14) {
        for (i = 0; i < 7; ++i) {
            if (seven_cards_loc[i].value == 14) {
                seven_cards_loc[i].value = 1;
                break;
            }
        }
    }

    i,j = 0;
    for (i = 0; i < 6; ++i){
        if (seven_cards_loc[i].value == seven_cards_loc[i+1].value){
            seven_cards_loc[i+1].value = 16;
        }
    }

    qsort(seven_cards_loc, 7, sizeof(struct card), compare_cards_value);

    i,j = 0;
    for (i = 0; i < 3; ++i){
        if (seven_cards_loc[i].value+1 == seven_cards_loc[i+1].value && seven_cards_loc[i].value+2 == seven_cards_loc[i+2].value && seven_cards_loc[i].value+3 == seven_cards_loc[i+3].value && seven_cards_loc[i].value+4 == seven_cards_loc[i+4].value){
            straight[0] = seven_cards_loc[i];
            straight[1] = seven_cards_loc[i+1];
            straight[2] = seven_cards_loc[i+2];
            straight[3] = seven_cards_loc[i+3];
            straight[4] = seven_cards_loc[i+4];
            found = 1;
        }
    }

    if (found == 1){
        free(seven_cards_loc);
        return straight;
    }
    else{
        free(seven_cards_loc);
        free(straight);
        return NULL;
    }
}

struct card* flush(struct card* seven_cards){
    struct card* flush = malloc(5 * sizeof(struct card));
    struct card* seven_cards_loc = malloc(7 * sizeof(struct card));
    int flushCount = 0;
    int i,j,k = 0;
    for (i = 0; i < 7; ++i) seven_cards_loc[i] = seven_cards[i];
    for (i = 0; i < 5; ++i){
        flush[i].value = -1;//if it not flush
        flush[i].suit = 'n';
    }


    for (int i = 0; i < 7; ++i) {
        int suitCount = 1; // Number of cards with the same suit
        flush[0] = seven_cards_loc[i];
        flushCount = 1;
        for (int j = i + 1; j < 7; ++j) {
            if (seven_cards_loc[j].suit == flush[0].suit) {
                flush[flushCount] = seven_cards_loc[j];
                flushCount++;
                if (flushCount == 5) {
                    free(seven_cards_loc);
                    return flush;
                }
            }
        }
    }

    // No flush found
    free(seven_cards_loc);
    free(flush);
    return NULL;
}

struct card* full(struct card* seven_cards){
    struct card* full = malloc(5 * sizeof(struct card));
    int i,j,found = 0;
    for (i = 0; i < 5; ++i){
        full[i].value = -1;
        full[i].suit = 'n';
    }

    struct card* three_of_a_kind_check = malloc(3 * sizeof(struct card));

    three_of_a_kind_check = three_of_a_kind(seven_cards);

    if (three_of_a_kind_check == NULL) {
        free(three_of_a_kind_check);
        free(full);
        return NULL;
    }

    full[0] = three_of_a_kind_check[0];
    full[1] = three_of_a_kind_check[1];
    full[2] = three_of_a_kind_check[2];

    free(three_of_a_kind_check);

    struct card* four_cards = malloc(4 * sizeof(struct card));

    for (i = 0; i < 7; ++i){
        if(seven_cards[i].value != full[0].value){
            four_cards[j] = seven_cards[i];
            j++;
        }
    }

    i = 0;
    for (i = 0; i < 4; ++i){
        if (four_cards[i].value == four_cards[i+1].value && four_cards[i].value > full[3].value ){
            full[3] = four_cards[i];
            full[4] = four_cards[i+1];
            found = 1;
        }
    }

    if (found == 1){
        free(four_cards);
        return full;
    }
    else{
        free(four_cards);
        free(full);
        return NULL;
    }
}

struct card* square(struct card* seven_cards){
    struct card* square;
    square = malloc(4 * sizeof(struct card));
    int i,found = 0;
    for (i = 0; i < 4; ++i){
        square[i].value = -1;//if it not square
        square[i].suit = 'n';
    }
    for (i = 0; i < 4; ++i){
        if (seven_cards[i].value == seven_cards[i+1].value && seven_cards[i].value == seven_cards[i+2].value
            && seven_cards[i].value == seven_cards[i+3].value && seven_cards[i].value > square[0].value){
            square[0] = seven_cards[i];
            square[1] = seven_cards[i+1];
            square[2] = seven_cards[i+2];
            square[3] = seven_cards[i+3];
            found = 1;
        }
    }
    if (found == 1)
        return square;
    else{
        free(square);
        return NULL;
    }
}

struct card* straight_flush(struct card* seven_cards){
    struct card* straight_flush = malloc(5 * sizeof(struct card));
    int i,j = 0;
    straight_flush = straight(seven_cards);
    if (straight_flush == NULL) {
        free(straight_flush);
        return NULL;
    }
    for (i = 0; i < 5; ++i){
        if(straight_flush[i].suit == straight_flush[0].suit){
            j++;
            if (j==5)
                return straight_flush;
        }
        else{
            free(straight_flush);
            return NULL;
        }
    }
}

struct card* find_best_combo(struct card* seven_cards) {
    struct card* combo = NULL;

    // Check for different combinations in descending order of hand rankings
    combo = straight_flush(seven_cards);
    if(combo != NULL) return combo;

    combo = square(seven_cards);
    if(combo != NULL) return combo;

    combo = full(seven_cards);
    if(combo != NULL) return combo;

    combo = flush(seven_cards);
    if(combo != NULL) return combo;

    combo = straight(seven_cards);
    if(combo != NULL) return combo;

    combo = three_of_a_kind(seven_cards);
    if(combo != NULL) return combo;

    combo = two_pair(seven_cards);
    if(combo != NULL) return combo;

    combo = pair(seven_cards);
    if(combo != NULL) return combo;

    return NULL;  // No valid combination found
}

int main(int argc, char const *argv[]){
    double current_time_ns = time_ns();
    time_t t;
    int i = 0;
    for (i = 0; i < t % 10000000; ++i) i=i;
    struct card* deck52 = malloc(52 * sizeof(struct card));
    struct player_attrib* players = malloc(8 * sizeof(struct player_attrib));
    deck52 = create_deck();
    players = set_money_player(players);
    players = utg_positions(players, 0);
    players = set_hand_cards(players, deck52);

    struct card* visibles_cards = malloc(5 * sizeof(struct card));
    
    for (i = 0; i < 5; ++i){
        visibles_cards[i] = take_alt_card(deck52);
    }

    /*visibles_cards[0].value = 6;
    visibles_cards[1].value = 6;
    visibles_cards[2].value = 6;
    visibles_cards[3].value = 2;
    visibles_cards[4].value = 2;
    players[0].hand[0].value = 4;
    players[0].hand[1].value = 4;
    visibles_cards[0].suit = 'C';
    visibles_cards[1].suit = 'H';
    visibles_cards[2].suit = 'P';
    visibles_cards[3].suit = 'C';
    visibles_cards[4].suit = 'C';
    players[0].hand[0].suit = 'T';
    players[0].hand[1].suit = 'H';*/

    struct card* seven_cards = malloc(7 * sizeof(struct card));

    for (i = 0; i < 7; ++i){
        seven_cards[i] = visibles_cards[i];
        if(i == 5)
            seven_cards[i] = players[0].hand[0];
        else if(i == 6)
            seven_cards[i] = players[0].hand[1];
    }

    qsort(seven_cards, 7, sizeof(struct card),compare_cards_value);

    struct card* pair_combo = malloc(2 * sizeof(struct card));
    struct card* two_pair_combo = malloc(2 * sizeof(struct card));
    struct card* three_of_a_kind_combo = malloc(3 * sizeof(struct card));
    struct card* square_combo = malloc(4 * sizeof(struct card));
    struct card* flush_combo = malloc(5 * sizeof(struct card));
    struct card* straight_combo = malloc(5 * sizeof(struct card));
    struct card* full_combo = malloc(5 * sizeof(struct card));
    struct card* straight_flush_combo = malloc(5 * sizeof(struct card));

    pair_combo = pair(seven_cards);
    two_pair_combo = two_pair(seven_cards);
    three_of_a_kind_combo = three_of_a_kind(seven_cards);
    square_combo = square(seven_cards);
    flush_combo = flush(seven_cards);
    straight_combo = straight(seven_cards);
    full_combo = full(seven_cards);
    straight_flush_combo = straight_flush(seven_cards);

    for (i = 0; i < 7; ++i)
        printf("%d%c:", seven_cards[i].value, seven_cards[i].suit);
    printf("\n");

    if (pair_combo != NULL) {
        printf("first pair      : %d%c%c\n",pair_combo[0].value, pair_combo[0].suit, pair_combo[1].suit);
        free(pair_combo);
    } else {
        printf("No pair found.\n");
    }

    if (two_pair_combo != NULL) {
        printf("scd pair        : %d%c%c\n",two_pair_combo[0].value, two_pair_combo[0].suit, two_pair_combo[1].suit);
        free(two_pair_combo);
    } else {
        printf("No scd pair found.\n");
    }

    if (three_of_a_kind_combo != NULL) {
        printf("three of a kind : %d%c%c%c\n",three_of_a_kind_combo[0].value,three_of_a_kind_combo[0].suit,three_of_a_kind_combo[1].suit,three_of_a_kind_combo[2].suit);
        free(three_of_a_kind_combo);
    } else {
        printf("No three of a kind found.\n");
    }
    
    if (square_combo != NULL) {
        printf("square          : %d%c%c%c%c\n",square_combo[0].value,square_combo[0].suit,square_combo[1].suit,square_combo[2].suit,square_combo[3].suit);
        free(square_combo);
    } else {
        printf("No square found.\n");
    }

    if (flush_combo != NULL) {
        printf("flush           : %d:%d:%d:%d:%d/%c\n",flush_combo[0].value, flush_combo[1].value, flush_combo[2].value, flush_combo[3].value, flush_combo[4].value, flush_combo[0].suit);
        free(flush_combo);
    } else {
        printf("No flush found.\n");
    }

    if (straight_combo != NULL) {
        printf("straight        : ");
        for (i = 0; i < 5; ++i)
            printf("%d%c:", straight_combo[i].value, straight_combo[i].suit);
        printf("\n");
        free(straight_combo);
    } else {
        printf("No straight found.\n");
    }

    if (full_combo != NULL) {
        printf("full            : ");
        for (i = 0; i < 5; ++i)
            printf("%d%c:", full_combo[i].value, full_combo[i].suit);
        printf("\n");
        free(full_combo);
    } else {
        printf("No full found.\n");
    }

    if (straight_flush_combo != NULL) {
        printf("straight flush  : ");
        for (i = 0; i < 5; ++i)
            printf("%d%c:", straight_flush_combo[i].value, straight_flush_combo[i].suit);
        printf("\n");
        free(straight_flush_combo);
    } else {
        printf("No straight flush found.\n");
    }
    
    for (i = 0; i < 8; ++i)
        printf("player %d ; pos %d ; card1 %d%c card2 %d%c ; tokens player:%f\n",players[i].player_num, players[i].position, players[i].hand[0].value, players[i].hand[0].suit, players[i].hand[1].value, players[i].hand[1].suit, players[i].tokens);

    free(straight_flush_combo);
    free(full_combo);
    free(straight_combo);
    free(three_of_a_kind_combo);
    free(square_combo);
    free(two_pair_combo);
    free(seven_cards);
    free(visibles_cards);
    free(players);
    free(deck52);
    return 0;
}
