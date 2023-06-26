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
    int tokens;
    struct card hand[2];
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
    if(!deck){
        return NULL;
    }

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

struct player_def* init_players(int nplayers){
    struct player_def* players = (struct player_def*)malloc(nplayers * sizeof(struct player_def));
    for (int i = 0; i < nplayers; i++){
        players[i].player_id = i;
        players[i].position = i;
        players[i].tokens = 80;
    }
    return players;
}

struct card take_alt_card(struct card* deck52){
    struct card selected_card;
    int alt_num = rand() % 52;
    while(deck52[alt_num].already_taken == 1){
        alt_num = rand() % 52;
    }
    selected_card = deck52[alt_num];
    deck52[alt_num].already_taken = 1;
    return selected_card;
}

struct card* pair(struct card* all_cards){
    struct card* bestof = (struct card*)malloc(5 * sizeof(struct card));
    if(!bestof){
        return NULL;
    }
    for (int i = 0; i < 5; i++){
        bestof[i].value = -1;
        bestof[i].suit = 'n';    
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    if(!all_cards_loc){
        free(bestof);
        return NULL;
    }
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
    if(!bestof){
        return NULL;
    }
    for (int i = 0; i < 5; i++){
        bestof[i].value = -1;
        bestof[i].suit = 'n';    
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    if(!all_cards_loc){
        free(bestof);
        return NULL;
    }
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    struct card* first_pair_check = pair(all_cards_loc);
    if(!first_pair_check){
        free(all_cards_loc);
        free(bestof);
        return NULL;
    }

    bestof[0] = first_pair_check[0];
    bestof[1] = first_pair_check[1];

    free(first_pair_check);

    struct card* five_cards = (struct card*)malloc(5 * sizeof(struct card));
    if(!five_cards){
        free(all_cards_loc);
        free(bestof);
        return NULL;
    }

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
    if(!bestof){
        return NULL;
    }
    for (int i = 0; i < 5; i++){
        bestof[i].value = -1;
        bestof[i].suit = 'n';
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    if(!all_cards_loc){
        free(bestof);
        return NULL;
    }
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
    if(!all_cards_loc){
        return NULL;
    }
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];
    struct card* straight = (struct card*)malloc(5*sizeof(struct card));
    if(!straight){
        free(all_cards_loc);
        return NULL;
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
            }
        }
    }

    for (int i = 0; i < 6; ++i){
        if (all_cards_loc[i].value == all_cards_loc[i+1].value){
            all_cards_loc[i+1].value = 16;
        }
    }

    qsort(all_cards_loc, 7, sizeof(struct card), compare_cards_value);

    int found = 0;
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
    if(!flush){
        return NULL;
    }

    int flushCount = 0;
    for (int i = 0; i < 7; ++i) {
        int suitCount = 1; // Number of cards with the same suit
        flush[0] = all_cards[i];
        flushCount = 1;
        for (int j = i + 1; j < 7; ++j) {
            if (all_cards[j].suit == flush[0].suit) {
                flush[flushCount] = all_cards[j];
                flushCount++;
                if (flushCount == 5)
                    return flush;
            }
        }
    }

    // No flush found
    free(flush);
    return NULL;
}

struct card* full(struct card* all_cards){
    struct card* full = (struct card*)malloc(5 * sizeof(struct card));
    if(!full){
        return NULL;
    }

    struct card* three_of_a_kind_check = three_of_a_kind(all_cards);
    if(!three_of_a_kind_check){
        free(full);
        return NULL;
    }

    full[0] = three_of_a_kind_check[0];
    full[1] = three_of_a_kind_check[1];
    full[2] = three_of_a_kind_check[2];

    free(three_of_a_kind_check);

    struct card* four_cards = (struct card*)malloc(4 * sizeof(struct card));
    if(!four_cards){
        free(full);
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < 7; ++i){
        if(all_cards[i].value != full[0].value){
            four_cards[j] = all_cards[i];
            j++;
        }
    }

    int found = 0;
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
    if(!bestof){
        return NULL;
    }
    for (int i = 0; i < 5; ++i){
        bestof[i].value = -1;//if it not square
        bestof[i].suit = 'n';
    }

    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    if(!all_cards_loc){
        free(bestof);
        return NULL;
    }
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    int j = 0;
    int found = 0;
    for (int i = 0; i < 3; ++i){
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
    struct card* straight_flush = straight(all_cards);
    if(!straight_flush){
        return NULL;
    }
    
    int flushCount = 0;
    for (int i = 0; i < 5; ++i){
        if(straight_flush[i].suit == straight_flush[0].suit){
            flushCount++;
            if (flushCount==5)
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
    if(!combo){
        return NULL;
    }

    qsort(all_cards, 7, sizeof(struct card), compare_cards_value);
    
    // Check for different combinations in descending order of hand rankings
    struct card* bestof_straight_flush = straight_flush(all_cards);
    if(bestof_straight_flush != NULL){
        combo[0].value = 8;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_straight_flush[i].value;
            combo[i+1].suit = bestof_straight_flush[i].suit;
        }
        free(bestof_straight_flush);
        return combo;
    }
    free(bestof_straight_flush);

    struct card* bestof_square = square(all_cards);
    if(bestof_square != NULL){
        combo[0].value = 7;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_square[i].value;
            combo[i+1].suit = bestof_square[i].suit;
        }
        free(bestof_square);
        return combo;
    }
    free(bestof_square);

    struct card* bestof_full = full(all_cards);
    if(bestof_full != NULL){
        combo[0].value = 6;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_full[i].value;
            combo[i+1].suit = bestof_full[i].suit;
        }
        free(bestof_full);
        return combo;
    }
    free(bestof_full);

    struct card* bestof_flush = flush(all_cards);
    if(bestof_flush != NULL){
        combo[0].value = 5;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_flush[i].value;
            combo[i+1].suit = bestof_flush[i].suit;
        }
        free(bestof_flush);
        return combo;
    }
    free(bestof_flush);

    struct card* bestof_straight = straight(all_cards);
    if(bestof_straight != NULL){
        combo[0].value = 4;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_straight[i].value;
            combo[i+1].suit = bestof_straight[i].suit;
        }
        free(bestof_straight);
        return combo;
    }
    free(bestof_straight);

    struct card* bestof_three = three_of_a_kind(all_cards);
    if(bestof_three != NULL){
        combo[0].value = 3;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_three[i].value;
            combo[i+1].suit = bestof_three[i].suit;
        }
        free(bestof_three);
        return combo;
    }
    free(bestof_three);

    struct card* bestof_two_pair = two_pair(all_cards);
    if(bestof_two_pair != NULL){
        combo[0].value = 2;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_two_pair[i].value;
            combo[i+1].suit = bestof_two_pair[i].suit;
        }
        free(bestof_two_pair);
        return combo;
    }
    free(bestof_two_pair);

    struct card* bestof_pair = pair(all_cards);
    if(bestof_pair != NULL){
        combo[0].value = 1;
        for (int i = 0; i < 5; i++)
        {
            combo[i+1].value = bestof_pair[i].value;
            combo[i+1].suit = bestof_pair[i].suit;
        }
        free(bestof_pair);
        return combo;
    }
    free(bestof_pair);

    combo[0].value = 0;
    combo[1] = all_cards[6];
    combo[2] = all_cards[5];
    combo[3] = all_cards[4];
    combo[4] = all_cards[3];
    combo[5] = all_cards[2];

    return combo;  // if there is no combo return the 5 highest cards
}

int main(){
    srand((unsigned int) time(NULL));
    int nplayer = 1;
    if (nplayer >= 23)
        return 1;    
    struct card* deck52 = create_deck();
    if(!deck52){
        return 1;
    }
    struct player_def* players = init_players(nplayer);
    if(!players){
        free(deck52);
        return 1;
    }
    struct card* all_cards = (struct card*)malloc(7 * sizeof(struct card));
    if(!all_cards){
        free(players);
        free(deck52);
        return 1;
    }
   
    //int score_tot = 0;
    for (int i = 0; i < 1; i++){
        for (int i = 0; i < nplayer; i++){
            players[i].hand[0] = take_alt_card(deck52);
            players[i].hand[1] = take_alt_card(deck52);
        }
        all_cards[0] = players[0].hand[0];
        all_cards[1] = players[0].hand[1];
        //player bet
        //flop
        all_cards[2] = take_alt_card(deck52);
        all_cards[3] = take_alt_card(deck52);
        all_cards[4] = take_alt_card(deck52);

        //player bet
        //turn
        all_cards[5] = take_alt_card(deck52);

        //player bet
        //river

        all_cards[6] = take_alt_card(deck52);
        //player bet
        //showdown

        for (int i = 0; i < 7; ++i)
            printf("%d%c:", all_cards[i].value, all_cards[i].suit);
        printf("\n");

        struct card* best_combo = find_best_combo(all_cards);
        if(!best_combo){
            free(all_cards);
            free(players);
            free(deck52);
            return 1;
        }

        //if (best_combo[0].value == 1)
        //    score_tot += 1;

        printf("score combo: %d\nbest of combo: \n", best_combo[0].value);
        for (int i = 0; i < 5; i++)
            printf("%d%c:",best_combo[i+1].value,best_combo[i+1].suit);
        printf("\n");
        
        free(best_combo);

        for (int i = 0; i < 52; i++)
            deck52[i].already_taken = 0;

        for (int i = 0; i < nplayer; i++)
            printf("id:%d position:%d tokens:%d hand:%d%c:%d%c\n", players[i].player_id, players[i].position, players[i].tokens, players[i].hand[0].value, players[i].hand[0].suit, players[i].hand[1].value, players[i].hand[1].suit);
    }
    //printf("%d\n",score_tot);

    free(all_cards);
    free(players);
    free(deck52);
    return 0;
}
