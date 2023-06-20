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
    struct card* bestof = (struct card*)malloc(5 * sizeof(struct card));
    for (int i = 0; i < 5; i++){
        bestof[i].value = -1;
        bestof[i].suit = 'n';    
    }
    
    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    int found = 0;
    int j = 0;

    for (int i = 0; i < 6; ++i){
        if (all_cards_loc[i].value == all_cards_loc[i+1].value && all_cards_loc[i].value > bestof[0].value ){
            j = i;
            found = 1;
        }
    }


    if (found == 1){
        bestof[0] = all_cards_loc[j];
        bestof[1] = all_cards_loc[j+1];
        all_cards_loc[j].value = -1;
        all_cards_loc[j+1].value = -1;
        qsort(all_cards_loc, 7, sizeof(struct card), compare_cards_value);
        bestof[2] = all_cards_loc[6];
        bestof[3] = all_cards_loc[5];
        bestof[4] = all_cards_loc[4];
        free(all_cards_loc);
        return bestof;
    }
    else{
        free(all_cards_loc);
        free(bestof);
        return NULL;
    }
}

struct card* two_pair(struct card* all_cards){
    struct card* bestof = (struct card*)malloc(5 * sizeof(struct card));
    for (int i = 0; i < 5; i++){
        bestof[i].value = -1;
        bestof[i].suit = 'n';    
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    struct card* first_pair_check = (struct card*)malloc(5 * sizeof(struct card));
    first_pair_check = pair(all_cards_loc);

    if (first_pair_check == NULL) {
        free(first_pair_check);
        free(all_cards_loc);
        free(bestof);
        return NULL;
    }

    bestof[0] = first_pair_check[0];
    bestof[1] = first_pair_check[1];

    free(first_pair_check);

    struct card* five_cards = (struct card*)malloc(5 * sizeof(struct card));

    int j = 0;
    for (int i = 0; i < 7; ++i) {
        if (all_cards_loc[i].value != bestof[0].value) {
            five_cards[j] = all_cards_loc[i];
            j++;
        }
    }

    int found = 0;
    j = 0;
    for (int i = 0; i < 4; ++i) {
        if (five_cards[i].value == five_cards[i+1].value && five_cards[i].value < bestof[0].value) {
            j = i;
            found = 1;
            break;
        }
    }

    if (found == 1){
        bestof[2] = five_cards[j];
        bestof[3] = five_cards[j+1];
        five_cards[j].value = -1;
        five_cards[j+1].value = -1;
        qsort(five_cards, 5, sizeof(struct card), compare_cards_value);
        bestof[4] = five_cards[4];
        free(five_cards);
        free(all_cards_loc);
        return bestof;
    }
    else {
        free(all_cards_loc);
        free(five_cards);
        free(bestof);
        return NULL;
    }
}

struct card* three_of_a_kind(struct card* all_cards){
    struct card* bestof = (struct card*)malloc(5 * sizeof(struct card));
    
    for (int i = 0; i < 5; i++)
    {
        bestof[i].value = -1;
        bestof[i].suit = 'n';
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    int found = 0;
    int j = 0;
    for (int i = 0; i < 5; ++i){
        if (all_cards[i].value == all_cards[i+1].value && all_cards[i].value == all_cards[i+2].value && all_cards[i].value > bestof[0].value ){
            j = i;
            found = 1;
        }
    }

    if (found == 1){
        bestof[0] = all_cards_loc[j];
        bestof[1] = all_cards_loc[j+1];
        bestof[2] = all_cards_loc[j+2];
        all_cards_loc[j].value = -1;
        all_cards_loc[j+1].value = -1;
        all_cards_loc[j+2].value = -1;
        qsort(all_cards_loc, 7, sizeof(struct card), compare_cards_value);
        bestof[3] = all_cards_loc[6];
        bestof[4] = all_cards_loc[5];
        free(all_cards_loc);
        return bestof;
    }
    else{
        free(all_cards_loc);
        free(bestof);
        return NULL;
    }
}

struct card* straight(struct card* all_cards) {
    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];
    struct card* straight = (struct card*)malloc(5*sizeof(struct card));
    int found = 0;

    for (int i = 0; i < 5; ++i){
        straight[i].value = -1;//if it not straight
        straight[i].suit = 'n';
    }
    
    int has_2 = 0;
    int has_3 = 0;
    int has_4 = 0;
    int has_14 = 0;
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
    struct card* bestof = (struct card*)malloc(5 * sizeof(struct card));
    for (int i = 0; i < 5; ++i){
        bestof[i].value = -1;//if it not square
        bestof[i].suit = 'n';
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    int j = 0;
    int found = 0;
    for (int i = 0; i < 4; ++i){
        if (all_cards[i].value == all_cards[i+1].value && all_cards[i].value == all_cards[i+2].value
            && all_cards[i].value == all_cards[i+3].value && all_cards[i].value > bestof[0].value){
            bestof[0] = all_cards[i];
            bestof[1] = all_cards[i+1];
            bestof[2] = all_cards[i+2];
            bestof[3] = all_cards[i+3];
            j = i;
            found = 1;
        }
    }
    if (found == 1)
    {
        bestof[0] = all_cards_loc[j];
        bestof[1] = all_cards_loc[j+1];
        bestof[2] = all_cards_loc[j+2];
        bestof[3] = all_cards_loc[j+3];
        all_cards_loc[j].value = -1;
        all_cards_loc[j+1].value = -1;
        all_cards_loc[j+2].value = -1;
        all_cards_loc[j+3].value = -1;
        qsort(all_cards_loc, 7, sizeof(struct card), compare_cards_value);
        bestof[4] = all_cards_loc[6];
        free(all_cards_loc);
        return bestof;
    }
        
    else{
        free(all_cards_loc);
        free(bestof);
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
    struct card* combo = (struct card*)malloc(6 * sizeof(struct card));
    struct card* bestof = (struct card*)malloc(5 * sizeof(struct card));

    // Check for different combinations in descending order of hand rankings
    bestof = straight_flush(all_cards);
    if(bestof != NULL){
        combo[0].value = 8;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = square(all_cards);
    if(bestof != NULL){
        combo[0].value = 7;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = full(all_cards);
    if(bestof != NULL){
        combo[0].value = 6;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = flush(all_cards);
    if(bestof != NULL){
        combo[0].value = 5;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = straight(all_cards);
    if(bestof != NULL){
        combo[0].value = 4;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = three_of_a_kind(all_cards);
    if(bestof != NULL){
        combo[0].value = 3;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = two_pair(all_cards);
    if(bestof != NULL){
        combo[0].value = 2;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    bestof = pair(all_cards);
    if(bestof != NULL){
        combo[0].value = 1;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof[i].value;
            combo[i+1].suit = bestof[i].suit;
        }
        free(bestof);
        return combo;
    }

    free(bestof);

    qsort(all_cards, 7, sizeof(struct card), compare_cards_value);

    combo[0].value = 0;
    combo[1] = all_cards[6];
    combo[2] = all_cards[5];
    combo[3] = all_cards[4];
    combo[4] = all_cards[3];
    combo[5] = all_cards[2];
    return combo;  // if there is no combo return the 5 highest cards
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
    //for (int i = 0; i < 1000000; i++){
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
            struct card* best_combo = (struct card*)malloc(6 * sizeof(struct card));

            all_cards[5] = players[0].hand[0];
            all_cards[6] = players[0].hand[1];
            
            qsort(all_cards, 7, sizeof(struct card), compare_cards_value);

            for (int i = 0; i < 7; ++i)
                printf("%d%c:", all_cards[i].value, all_cards[i].suit);
            printf("\n");

            best_combo = find_best_combo(all_cards);

            /*if (best_combo == NULL){
            }
            else if (best_combo[0].value == 8)
            {
                score_tot += 1;
            }
            
            for (int i = 0; i < 52; i++)
            {
                deck52[i].already_taken = 0;
            }*/

            printf("score combo: %d\nbest of combo: ", best_combo[0].value);
            for (int i = 0; i < 5; i++)
                printf("%d%c:",best_combo[i+1].value,best_combo[i+1].suit);
            
            free(best_combo);
            //for (int i = 0; i < nplayer; ++i)
            //    printf("player %d ; pos %d ; card1 %d%c card2 %d%c ; tokens player:%d\n",players[i].player_id, players[i].position, players[i].hand[0].value, players[i].hand[0].suit, players[i].hand[1].value, players[i].hand[1].suit, players[i].tokens);
        }
    }
    //}
    //printf("%d",score_tot);

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
