#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
    int actions;
    struct card hand[2];
};

struct tables{
    struct player_def* players;
};

int compare_cards_value(const void *a, const void *b) {
    const struct card *card_a = (const struct card*)a;
    const struct card *card_b = (const struct card*)b;
    return card_a->value - card_b->value;
}

int inverse_compare_cards_value(const void *a, const void *b) {
    const struct card *card_a = (const struct card*)a;
    const struct card *card_b = (const struct card*)b;
    return card_b->value - card_a->value;
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

struct player_def* init_players(int nplayer){
    struct player_def* players = (struct player_def*)malloc(nplayer * sizeof(struct player_def));
    for (int i = 0; i < nplayer; i++){
        players[i].player_id = i;
        players[i].position = i;
        players[i].actions = 0;
        players[i].tokens = 80;
    }
    players[0].actions = 5;
    //players[1].actions = 6;
    //players[2].actions = 8;
    //players[3].actions = 8;
    return players;
}

struct tables* init_tables(int ntable){
    struct tables* table = (struct tables*)malloc(ntable * sizeof(struct tables));
    for (int i = 0; i < ntable; i++){
        table[i].players = init_players(6);
    }
    return table;
}

void free_tables(struct tables* table, int ntable) {
    for (int i = 0; i < ntable; i++) {
        free(table[i].players);
    }
    free(table);
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
        all_cards_loc[j].value = 16;
        all_cards_loc[j+1].value = 16;
        qsort(all_cards_loc, 7, sizeof(struct card), inverse_compare_cards_value);
        bestof[2] = all_cards_loc[0];
        bestof[3] = all_cards_loc[1];
        bestof[4] = all_cards_loc[2];
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
        if (five_cards[i].value == five_cards[i+1].value && five_cards[i].value > bestof[0].value) {
            j = i;
            found = 1;
        }
    }

    if (found == 1){
        bestof[2] = five_cards[j];
        bestof[3] = five_cards[j+1];
        five_cards[j].value = 16;
        five_cards[j+1].value = 16;
        qsort(five_cards, 5, sizeof(struct card), inverse_compare_cards_value);
        bestof[4] = five_cards[0];
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
        all_cards_loc[j].value = 16;
        all_cards_loc[j+1].value = 16;
        all_cards_loc[j+2].value = 16;
        qsort(all_cards_loc, 7, sizeof(struct card), inverse_compare_cards_value);
        bestof[3] = all_cards_loc[0];
        bestof[4] = all_cards_loc[1];
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
        qsort(straight, 5, sizeof(struct card), inverse_compare_cards_value);
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
                if (flushCount == 5){
                    qsort(flush, 5, sizeof(struct card), inverse_compare_cards_value);
                    return flush;
                }
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
        all_cards_loc[j].value = 16;
        all_cards_loc[j+1].value = 16;
        all_cards_loc[j+2].value = 16;
        all_cards_loc[j+3].value = 16;
        qsort(all_cards_loc, 7, sizeof(struct card), inverse_compare_cards_value);
        bestof[4] = all_cards_loc[0];
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

struct card* find_best_combo(struct card* all_cards){
    struct card* combo = (struct card*)malloc(6 * sizeof(struct card));
    if(!combo){
        return NULL;
    }
    struct card* all_cards_loc = (struct card*)malloc(7 * sizeof(struct card));
    if(!all_cards_loc){
        free(combo);
        return NULL;
    }
    for (int i = 0; i < 7; ++i) all_cards_loc[i] = all_cards[i];

    qsort(all_cards_loc, 7, sizeof(struct card), compare_cards_value);

    printf("\nyyyyy\n");
    for (int k = 0; k < 7; k++)
        printf("%d%c:",all_cards_loc[k].value, all_cards_loc[k].suit);
    printf("\nyyyyy");
    
    
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

    printf("\nxxxxx\n");
    for (int k = 0; k < 6; k++)
        printf("%d%c:",combo[k].value, combo[k].suit);
    printf("\nxxxxx");

    return combo;  // if there is no combo return the 5 highest cards
}

int forward_propagation(struct player_def player, struct tables table){

}

void game_round(struct tables table){
    int pot = 0;
    for (int i = 0; i < 6; i++){
        if (table.players[i].position == 0){
            struct card* deck52 = create_deck();
            if(!deck52){
                return;
            }
            /* pay of big and small blind*/
            table.players[(i+4)%6].tokens -= 1;
            table.players[(i+4)%6].actions = 1;
            table.players[(i+5)%6].tokens -= 2;
            table.players[(i+5)%6].actions = 2;
            pot += 3;

            /* card distribution */
            for (int j = 0; j < 6; j++){
                table.players[j].hand[0] = take_alt_card(deck52);
                table.players[j].hand[1] = take_alt_card(deck52);
            }

            /* preflop player actions */
            int call_price = table.players[(i+5)%6].actions;
            int all_player_check = 1;
            int j = i;
            for (int all_player_check = 0; all_player_check < 5;){
                forward_propagation(table.players[j], table);
                if (table.players[j].actions > call_price)
                {
                    call_price = table.players[j].actions;
                    table.players[j].tokens -= call_price;
                    pot += call_price;
                    //printf("player action :%d   player id :%d  position: %d   raise   tokens :%d\n", table.players[j].actions, table.players[j].player_id, table.players[j].position, table.players[j].tokens);
                    all_player_check = 0;
                }
                else if (table.players[j].actions == call_price){
                    table.players[j].tokens -= call_price;
                    pot += call_price;
                    //printf("player action :%d   player id :%d  position: %d   check/call   tokens :%d\n", table.players[j].actions, table.players[j].player_id, table.players[j].position, table.players[j].tokens);
                    all_player_check += 1;
                }
                else{
                    //printf("player action :%d   player id :%d  position: %d   fold   tokens :%d\n", table.players[j].actions, table.players[j].player_id, table.players[j].position, table.players[j].tokens);
                    table.players[j].actions = -1;
                    all_player_check += 1;
                }
                j += 1;
                j = j%6;
            }
            for (int j = 0; j < 6; j++)
            {
                printf("player action :%d   player id :%d  position: %d   tokens :%d\n", table.players[j].actions, table.players[j].player_id, table.players[j].position, table.players[j].tokens);
            }
            
            struct card* all_cards = (struct card*)malloc(7 * sizeof(struct card));
            if(!all_cards){
                free(deck52);
                return;
            }
            all_cards[0] = take_alt_card(deck52);
            all_cards[1] = take_alt_card(deck52);
            all_cards[2] = take_alt_card(deck52);
            all_cards[3] = take_alt_card(deck52);
            all_cards[4] = take_alt_card(deck52);
            /* finding the winner */
            size_t nwiner = 1;
            int* winers = (int*)calloc(6, sizeof(int));
            if(!winers){
                free(all_cards);
                free(deck52);
                return;
            }
            for (int j = 0; j < 6; j++){
                
                //table.players[3].hand[0] = table.players[2].hand[0];
                //table.players[3].hand[1] = table.players[2].hand[1];
                    
                struct card* all_combo_player = (struct card*)malloc(6 * sizeof(struct card));
                if(!all_combo_player)
                {
                    free(winers);
                    free(all_cards);
                    free(deck52);
                    return;
                }
                /* all players who want to check */
                if(table.players[j].actions >= 0){
                    all_cards[5] = table.players[j].hand[0];
                    all_cards[6] = table.players[j].hand[1];

                    struct card* best_combo = find_best_combo(all_cards);
                    if(!best_combo){
                        free(all_combo_player);
                        free(winers);
                        free(all_cards);
                        free(deck52);
                        return;
                    }
                    if (winers[0] == 0){
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else if (best_combo[0].value > all_combo_player[0].value){
                        printf("1 best combo\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",best_combo[k].value, best_combo[k].suit);
                        printf("\n");
                        printf("all combo player\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",all_combo_player[k].value, all_combo_player[k].suit);
                        printf("\n");
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else if (best_combo[1].value > all_combo_player[1].value){
                        printf("2 best combo\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",best_combo[k].value, best_combo[k].suit);
                        printf("\n");
                        printf("all combo player\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",all_combo_player[k].value, all_combo_player[k].suit);
                        printf("\n");
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else if (best_combo[2].value > all_combo_player[2].value){
                        printf("3 best combo\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",best_combo[k].value, best_combo[k].suit);
                        printf("\n");
                        printf("all combo player\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",all_combo_player[k].value, all_combo_player[k].suit);
                        printf("\n");
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else if (best_combo[3].value > all_combo_player[3].value){
                        printf("4 best combo\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",best_combo[k].value, best_combo[k].suit);
                        printf("\n");
                        printf("all combo player\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",all_combo_player[k].value, all_combo_player[k].suit);
                        printf("\n");
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else if (best_combo[4].value > all_combo_player[4].value){
                        printf("5 best combo\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",best_combo[k].value, best_combo[k].suit);
                        printf("\n");
                        printf("all combo player\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",all_combo_player[k].value, all_combo_player[k].suit);
                        printf("\n");
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else if (best_combo[5].value > all_combo_player[5].value){
                        printf("6 best combo\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",best_combo[k].value, best_combo[k].suit);
                        printf("\n");
                        printf("all combo player\n");
                        for (int k = 0; k < 6; k++)
                            printf("%d%c:",all_combo_player[k].value, all_combo_player[k].suit);
                        printf("\n");
                        for (int k = 0; k < 6; k++)
                            all_combo_player[k] = best_combo[k];
                        winers[0] = j;
                    }
                    else{
                        nwiner += 1;
                        winers[nwiner - 1] = j;
                    }
                    free(best_combo);
                }
                if (j >= 5)
                {
                    if (nwiner == 1){
                        table.players[winers[0]].tokens += pot;
                        printf("\nplayer %d win %d token\n", table.players[winers[0]].player_id, pot);
                    }
                    else{
                        for (int k = 0; k < nwiner; k++){
                            table.players[winers[k]].tokens += floor(pot/nwiner);
                            printf("\nplayer %d win %f token\n", table.players[winers[k]].player_id, floor(pot/nwiner));
                        }
                    pot = 0;
                    }
                }
                free(all_combo_player);
            }
            free(winers);
            free(all_cards);
            free(deck52);
        }
    }
    return;
}

int main(){
    srand((unsigned int) time(NULL));
    int ntable = 3;  
    int nplayer = ntable * 6;
    struct tables* tables = init_tables(ntable);
    if(!tables){
        return -1;
    }
    game_round(tables[1]);
    free_tables(tables, ntable);
    
    return 0;
}
