#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct card{
    int value;
    char suit;
    int already_taken;
};

struct player_def{
    int player_id;
    int position;
    struct card hand[2];
    int tokens;
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
    struct card* deck = (struct card*)malloc(52 * sizeof(struct card));

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

struct card take_alt_card(struct card* deck52){
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

struct player_def* set_tokens_player(struct player_def* players, int nplayer){
    for (int i = 0; i < nplayer; ++i){
        players[i].tokens = 10000;
    }
    return players;
}

struct player_def* utg_positions(struct player_def* positions, int utg_positions_in_table,int nplayer){
    utg_positions_in_table = utg_positions_in_table % nplayer;

    for (int i = 0; i < nplayer; i++) {
        positions[i].player_id = i;
        positions[i].position = utg_positions_in_table;
        utg_positions_in_table = (utg_positions_in_table + 1) % nplayer;
    }
    return positions;
}

struct player_def* set_hand_cards(struct player_def* positions, struct card* deck52, int nplayer){
    for (int i = 0; i < nplayer; ++i){
        positions[i].hand[0] = take_alt_card(deck52);
        positions[i].hand[1] = take_alt_card(deck52);
    }
    return positions;
}

struct card* pair(struct card* all_cards){
    struct card* pair = (struct card*)malloc(2 * sizeof(struct card));
    pair[0].value = -1;
    pair[0].suit = 'n';
    pair[1].value = -1;
    pair[1].suit = 'n';

    int found = 0;

    for (int i = 0; i < 6; ++i){
        if (all_cards[i].value == all_cards[i+1].value && all_cards[i].value > pair[0].value ){
            pair[0] = all_cards[i];
            pair[1] = all_cards[i+1];
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

struct card* two_pair(struct card* all_cards){
    struct card* pairs = (struct card*)malloc(2 * sizeof(struct card));
    pairs[0].value = -1;
    pairs[0].suit = 'n';
    pairs[1].value = -1;
    pairs[1].suit = 'n';

    struct card* first_pair_check = (struct card*)malloc(2 * sizeof(struct card));
    first_pair_check = pair(all_cards);

    if (first_pair_check == NULL) {
        free(first_pair_check);
        free(pairs);
        return NULL;
    }

    struct card* five_cards = (struct card*)malloc(5 * sizeof(struct card));

    int j = 0;
    for (int i = 0; i < 7; ++i) {
        if (all_cards[i].value != first_pair_check[0].value) {
            five_cards[j] = all_cards[i];
            j++;
        }
    }

    int found = 0;
    for (int i = 0; i < 4; ++i) {
        if (five_cards[i].value == five_cards[i + 1].value && five_cards[i].value < first_pair_check[0].value) {
            pairs[0] = five_cards[i];
            pairs[1] = five_cards[i + 1];
            found = 1;
            break;
        }
    }

    free(first_pair_check);
    free(five_cards);

    if (found == 1){
        return pairs;
    }
    else {
        free(pairs);
        return NULL;
    }
}

struct card* three_of_a_kind(struct card* all_cards){
    struct card* three = (struct card*)malloc(3 * sizeof(struct card));
    int found = 0;

    three[0].value = -1;
    three[0].suit = 'n';
    three[1].value = -1;
    three[1].suit = 'n';
    three[2].value = -1;
    three[2].suit = 'n';

    for (int i = 0; i < 5; ++i){
        if (all_cards[i].value == all_cards[i+1].value && all_cards[i].value == all_cards[i+2].value && all_cards[i].value > three[0].value ){
            three[0] = all_cards[i];
            three[1] = all_cards[i+1];
            three[2] = all_cards[i+2];
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

struct card* straight(struct card* all_cards) {
    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    int found = 0;
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];
    struct card* straight = (struct card*)malloc(5*sizeof(struct card));

    for (int i = 0; i < 5; ++i){
        straight[i].value = -1;//if it not straight
        straight[i].suit = 'n';
    }

    int has_2 = 0, has_3 = 0, has_4 = 0, has_14 = 0;
    for (int i = 0; i < 7; ++i) {
        if (all_cards_loc[i].value == 2) has_2 = 1;
        if (all_cards_loc[i].value == 3) has_3 = 1;
        if (all_cards_loc[i].value == 4) has_4 = 1;
        if (all_cards_loc[i].value == 14) has_14 = 1;
    }

    if (has_2 && has_3 && has_4 && has_14) {
        for (int i = 0; i < 7; ++i) {
            if (all_cards_loc[i].value == 14) {
                all_cards_loc[i].value = 1;
                break;
            }
        }
    }

    for (int i = 0; i < 6; ++i){
        if (all_cards_loc[i].value == all_cards_loc[i+1].value){
            all_cards_loc[i+1].value = 16;
        }
    }

    qsort(all_cards_loc, 7, sizeof(struct card), compare_cards_value);

    for (int i = 0; i < 3; ++i){
        if (all_cards_loc[i].value+1 == all_cards_loc[i+1].value && all_cards_loc[i].value+2 == all_cards_loc[i+2].value && all_cards_loc[i].value+3 == all_cards_loc[i+3].value && all_cards_loc[i].value+4 == all_cards_loc[i+4].value){
            straight[0] = all_cards_loc[i];
            straight[1] = all_cards_loc[i+1];
            straight[2] = all_cards_loc[i+2];
            straight[3] = all_cards_loc[i+3];
            straight[4] = all_cards_loc[i+4];
            found = 1;
        }
    }

    if (found == 1){
        free(all_cards_loc);
        return straight;
    }
    else{
        free(all_cards_loc);
        free(straight);
        return NULL;
    }
}

struct card* flush(struct card* all_cards){
    struct card* flush = (struct card*)malloc(5 * sizeof(struct card));
    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    int flushCount = 0;
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];
    for (int i = 0; i < 5; ++i){
        flush[i].value = -1;//if it not flush
        flush[i].suit = 'n';
    }


    for (int i = 0; i < 7; ++i) {
        int suitCount = 1; // Number of cards with the same suit
        flush[0] = all_cards_loc[i];
        flushCount = 1;
        for (int j = i + 1; j < 7; ++j) {
            if (all_cards_loc[j].suit == flush[0].suit) {
                flush[flushCount] = all_cards_loc[j];
                flushCount++;
                if (flushCount == 5) {
                    free(all_cards_loc);
                    return flush;
                }
            }
        }
    }

    // No flush found
    free(all_cards_loc);
    free(flush);
    return NULL;
}

struct card* full(struct card* all_cards){
    struct card* full = (struct card*)malloc(5 * sizeof(struct card));
    int found = 0;
    for (int i = 0; i < 5; ++i){
        full[i].value = -1;
        full[i].suit = 'n';
    }

    struct card* three_of_a_kind_check = (struct card*)malloc(3 * sizeof(struct card));

    three_of_a_kind_check = three_of_a_kind(all_cards);

    if (three_of_a_kind_check == NULL) {
        free(three_of_a_kind_check);
        free(full);
        return NULL;
    }

    full[0] = three_of_a_kind_check[0];
    full[1] = three_of_a_kind_check[1];
    full[2] = three_of_a_kind_check[2];

    free(three_of_a_kind_check);

    struct card* four_cards = (struct card*)malloc(4 * sizeof(struct card));

    int j = 0;
    for (int i = 0; i < 7; ++i){
        if(all_cards[i].value != full[0].value){
            four_cards[j] = all_cards[i];
            j++;
        }
    }

    for (int i = 0; i < 4; ++i){
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

struct card* square(struct card* all_cards){
    struct card* square = (struct card*)malloc(4 * sizeof(struct card));
    int found = 0;
    for (int i = 0; i < 4; ++i){
        square[i].value = -1;//if it not square
        square[i].suit = 'n';
    }
    for (int i = 0; i < 4; ++i){
        if (all_cards[i].value == all_cards[i+1].value && all_cards[i].value == all_cards[i+2].value
            && all_cards[i].value == all_cards[i+3].value && all_cards[i].value > square[0].value){
            square[0] = all_cards[i];
            square[1] = all_cards[i+1];
            square[2] = all_cards[i+2];
            square[3] = all_cards[i+3];
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

struct card* straight_flush(struct card* all_cards){
    struct card* straight_flush = (struct card*)malloc(5 * sizeof(struct card));
    straight_flush = straight(all_cards);
    if (straight_flush == NULL) {
        free(straight_flush);
        return NULL;
    }
    
    int j = 0;
    for (int i = 0; i < 5; ++i){
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

struct card* find_best_combo(struct card* all_cards) {
    struct card* combo = (struct card*)malloc(2 * sizeof(struct card));

    // Check for different combinations in descending order of hand rankings
    struct card* straight_flush_combo = (struct card*)malloc(5 * sizeof(struct card));
    straight_flush_combo = straight_flush(all_cards);
    if(straight_flush_combo != NULL){
        combo[0].value = 8;
        combo[1].value = straight_flush_combo[4].value;
        combo[1].suit = straight_flush_combo[4].suit;
        free(straight_flush_combo);
        return combo;
    }
    free(straight_flush_combo);

    struct card* square_combo = (struct card*)malloc(4 * sizeof(struct card));
    square_combo = square(all_cards);
    if(square_combo != NULL){
        combo[0].value = 7;
        combo[1].value = square_combo[3].value;
        combo[1].suit = square_combo[3].suit;
        free(square_combo);
        return combo;
    }
    free(square_combo);

    struct card* full_combo = (struct card*)malloc(5 * sizeof(struct card));
    full_combo = full(all_cards);
    if(full_combo != NULL){
        combo = realloc(combo,3 * sizeof(struct card));
        combo[0].value = 6;
        combo[1].value = full_combo[2].value;
        combo[1].suit = full_combo[2].suit;
        combo[2].value = full_combo[4].value;
        combo[2].suit = full_combo[4].suit;
        free(full_combo);
        return combo;
    }
    free(full_combo);

    struct card* flush_combo = (struct card*)malloc(5 * sizeof(struct card));
    flush_combo = flush(all_cards);
    if(flush_combo != NULL){
        combo[0].value = 5;
        combo[1].value = flush_combo[4].value;
        combo[1].suit = flush_combo[4].suit;
        free(flush_combo);
        return combo;
    }
    free(flush_combo);

    struct card* straight_combo = (struct card*)malloc(5 * sizeof(struct card));
    straight_combo = straight(all_cards);
    if(straight_combo != NULL){
        combo[0].value = 4;
        combo[1].value = straight_combo[4].value;
        combo[1].suit = straight_combo[4].suit;
        free(straight_combo);
        return combo;
    }
    free(straight_combo);

    struct card* three_of_a_kind_combo = (struct card*)malloc(3 * sizeof(struct card));
    three_of_a_kind_combo = three_of_a_kind(all_cards);
    if(three_of_a_kind_combo != NULL){
        combo[0].value = 3;
        combo[1].value = three_of_a_kind_combo[2].value;
        combo[1].suit = three_of_a_kind_combo[2].suit;
        free(three_of_a_kind_combo);
        return combo;
    }
    free(three_of_a_kind_combo);

    struct card* two_pair_combo = (struct card*)malloc(3 * sizeof(struct card));
    two_pair_combo = two_pair(all_cards);
    if(two_pair_combo != NULL){
        combo[0].value = 2;
        combo[1].value = two_pair_combo[0].value;
        combo[1].suit = two_pair_combo[0].suit;
        free(two_pair_combo);
        return combo;
    }
    free(two_pair_combo);

    struct card* pair_combo = (struct card*)malloc(3 * sizeof(struct card));
    pair_combo = pair(all_cards);
    if(pair_combo != NULL){
        combo[0].value = 1;
        combo[1].value = pair_combo[0].value;
        combo[1].suit = pair_combo[0].suit;
        free(pair_combo);
        return combo;
    }
    free(pair_combo);

    return NULL;  // No valid combination found
}

int main(int argc, char const *argv[]){
    srand((unsigned int) time(NULL));
    int nplayer = 1;
    struct card* deck52 = (struct card*)malloc(52 * sizeof(struct card));
    struct player_def* players = (struct player_def*)malloc(nplayer * sizeof(struct player_def));
    struct card* all_cards = (struct card*)malloc(7 * sizeof(struct card));deck52 = create_deck();
    //players = utg_positions(players, 0, nplayer);
    //players = set_hand_cards(players, deck52, nplayer);
    //players = set_tokens_player(players, nplayer);
    int score_tot = 0;
    //for (int i = 0; i < 10000; i++){
    for (int i = 0; i < 5; ++i){
        players[0].hand[0] = take_alt_card(deck52);
        players[0].hand[1] = take_alt_card(deck52);
        //player bet
        if (i == 1){
            //flop
            all_cards[0] = take_alt_card(deck52);
            all_cards[1] = take_alt_card(deck52);
            all_cards[2] = take_alt_card(deck52);
        }
        else if (i == 2){
            //turn
            all_cards[3] = take_alt_card(deck52);
        }
        else if (i == 3){
            //river
            all_cards[4] = take_alt_card(deck52);
        }
        else if(i == 4){
            struct card* best_combo = (struct card*)malloc(3 * sizeof(struct card));

            all_cards[5] = players[0].hand[0];
            all_cards[6] = players[0].hand[1];
            
            qsort(all_cards, 7, sizeof(struct card), compare_cards_value);

            for (int i = 0; i < 7; ++i)
                printf("%d%c:", all_cards[i].value, all_cards[i].suit);
            printf("\n");

            best_combo = find_best_combo(all_cards);

            /*if (best_combo == NULL){
            }

            for (int i = 0; i < 52; i++)
            {
                deck52[i].already_taken = 0;
            }*/
            

            if (best_combo == NULL && players[0].hand[1].value < players[0].hand[0].value)
                printf("high card : %d%c\n\n",players[0].hand[0].value,players[0].hand[0].suit);
            else if(best_combo == NULL && players[0].hand[0].value < players[0].hand[1].value)
                printf("high card : %d%c\n\n",players[0].hand[1].value,players[0].hand[1].suit);    
            else if(best_combo[0].value == 6)
                printf("score combo : %d\nthree kind:%d%c\npair: %d%c\n",best_combo[0].value,best_combo[1].value,best_combo[1].suit,best_combo[2].value,best_combo[2].suit);
            else
                printf("score combo : %d\nbest of combo:%d%c\n",best_combo[0].value,best_combo[1].value,best_combo[1].suit);
            free(best_combo);
            for (int i = 0; i < nplayer; ++i)
                printf("player %d ; pos %d ; card1 %d%c card2 %d%c ; tokens player:%d\n",players[i].player_id, players[i].position, players[i].hand[0].value, players[i].hand[0].suit, players[i].hand[1].value, players[i].hand[1].suit, players[i].tokens);
        }
    }
    //}

    free(all_cards);
    free(players);
    free(deck52);
    return 0;
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
