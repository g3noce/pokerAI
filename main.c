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

int compare_cards(const void *a, const void *b) {
    const struct card *card_a = (const struct card*)a;
    const struct card *card_b = (const struct card*)b;
    return card_a->value - card_b->value;
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

struct card* pair_checker(struct card* seven_cards){
    struct card* pair = malloc(2 * sizeof(struct card));
    for (int i = 0; i < 2; ++i){
        pair[i].value = -1;//if it not pair
        pair[i].suit = 'n';
    }
    for (int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if (j!=i && seven_cards[i].value == seven_cards[j].value && seven_cards[i].value > pair[0].value){
                pair[0] = seven_cards[i];
                pair[1] = seven_cards[j];
            }
        }
    }
    return pair;
}

struct card* two_pair_checker(struct card* seven_cards){
    struct card* pairs = malloc(4 * sizeof(struct card));
    for (int i = 0; i < 4; ++i){
        pairs[i].value = -1;
        pairs[i].suit = 'n';
    }

    struct card* first_pair_check = malloc(2 * sizeof(struct card));
    first_pair_check = pair_checker(seven_cards);
    
    for (int i = 0; i < 2; ++i){
        pairs[i] = first_pair_check[i];
    }

    for (int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if (j!=i && seven_cards[i].value == seven_cards[j].value && seven_cards[i].value > pairs[2].value){
                if (seven_cards[i].value != first_pair_check[0].value)
                {
                    pairs[2] = seven_cards[i];
                    pairs[3] = seven_cards[j];
                }
                else if (seven_cards[i].value == first_pair_check[0].value
                && seven_cards[i].suit != first_pair_check[0].suit
                && seven_cards[i].suit != first_pair_check[1].suit
                && seven_cards[j].suit != first_pair_check[0].suit
                && seven_cards[j].suit != first_pair_check[1].suit)
                {   
                    pairs[2] = seven_cards[i];
                    pairs[3] = seven_cards[j];
                }
            }
        }
    }
    return pairs;
}

struct card* three_of_a_kind(struct card* seven_cards){
	struct card* three;
    three = malloc(3 * sizeof(struct card));
    for (int i = 0; i < 3; ++i){
        three[i].value = -1;//if it not three
        three[i].suit = 'n';
    }
    for (int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if (j!=i && seven_cards[i].value == seven_cards[j].value){
                for (int k = 0; k < 7; ++k){
                    if(k!=j && k!=i && seven_cards[k].value == seven_cards[i].value && seven_cards[i].value > three[0].value){                        
                    	three[0] = seven_cards[i];
                        three[1] = seven_cards[j];
                        three[2] = seven_cards[k];
                    }
                }
            }
        }
    }
    return three;
}

struct card* square(struct card* seven_cards){
	struct card* square;
    square = malloc(4 * sizeof(struct card));
    for (int i = 0; i < 4; ++i){
        square[i].value = -1;//if it not square
        square[i].suit = 'n';
    }
    for (int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if (j!=i && seven_cards[i].value == seven_cards[j].value){
                for (int k = 0; k < 7; ++k){
                    if(k!=j && k!=i && seven_cards[k].value == seven_cards[i].value){
                        for (int l = 0; l < 7; ++l){
                        	if(l!=k && l!=j && l!=i && seven_cards[l].value == seven_cards[i].value){
                    			square[0] = seven_cards[i];
                                square[1] = seven_cards[j];
                                square[2] = seven_cards[k];
                                square[3] = seven_cards[l];
                                return square;
                        	}
                        }
                    }
                }
            }
        }
    }
    return square;
}

struct card* flush(struct card* seven_cards){
    struct card* flush = malloc(5 * sizeof(struct card));
    for (int i = 0; i < 5; ++i){
        flush[i].value = -1;//if it not flush
        flush[i].suit = 'n';
    }
    int k = 0;


    for (int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if (seven_cards[i].suit == seven_cards[j].suit){
                flush[0] = seven_cards[i];
                flush[k] = seven_cards[j];

                ++k;
                if (k>=5){
                    return flush;
                }

            }

        }
        for (int l = 0; l < 5; ++l){
            flush[l].value = -1;//if it not flush
            flush[l].suit = 'n';
        }
        k=0;
    }

    for (int i = 0; i < 5; ++i){
        flush[i].value = -1;//if it not flush
        flush[i].suit = 'n';
    }
    return flush;
}

struct card* straight(struct card* seven_cards) {

    int has_2 = 0, has_3 = 0, has_4 = 0, has_14 = 0;
    for (int i = 0; i < 7; ++i) {
        if (seven_cards[i].value == 2) has_2 = 1;
        if (seven_cards[i].value == 3) has_3 = 1;
        if (seven_cards[i].value == 4) has_4 = 1;
        if (seven_cards[i].value == 14) has_14 = 1;
    }
    if (has_2 && has_3 && has_4 && has_14) {
        for (int i = 0; i < 7; ++i) {
            if (seven_cards[i].value == 14) {
                seven_cards[i].value = 1;
                break;
            }
        }
    }

    qsort(seven_cards, 7, sizeof(struct card), compare_cards);
    struct card* straight = malloc(5*sizeof(struct card));
    for (int i = 0; i < 5; ++i){
        straight[i].value = -1;//if it not straight
        straight[i].suit = 'n';
    }

    int k = 1;

    for (int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if (seven_cards[i].value+1 == seven_cards[j].value){
                ++k;
                if(k==2){
                    straight[0] = seven_cards[i];
                    straight[1] = seven_cards[j];
                    j=7;
                }
                else if(k==3){
                    straight[2] = seven_cards[j];
                    j=7;
                }
                else if(k==4){
                    straight[3] = seven_cards[j];
                    j=7;
                }
                else if(k==5){
                    straight[4] = seven_cards[j];
                    return straight;
                }
            }
            else if (j==6 && seven_cards[i].value+1 != seven_cards[j].value)
            {
                k=1;
                for(int l = 0; l < 5; ++l){
                    straight[l].value = -1;//if it not straight
                    straight[l].suit = 'n';
                }
            }
        }
    }
    for(int l = 0; l < 5; ++l){
        straight[l].value = -1;//if it not straight
        straight[l].suit = 'n';
    }

    return straight;
}

int combo_ranking(struct card* five_cards_combo, struct player_attrib* positions){

}

int main(int argc, char const *argv[]){

	double current_time_ns = time_ns();
	time_t t;
	for (int i = 0; i < t % 10000000; ++i)
		i=i;
	struct card* deck52 = malloc(52 * sizeof(struct card));
	struct player_attrib* players = malloc(8 * sizeof(struct player_attrib));
	deck52 = create_deck();
	players = set_money_player(players);
	players = utg_positions(players, 2);
	players = set_hand_cards(players, deck52);

    struct card* visibles_cards = malloc(5 * sizeof(struct card));
    
    for (int i = 0; i < 5; ++i){
        visibles_cards[i] = take_alt_card(deck52);
    }
	

	/*visibles_cards[0].value = 2;
	visibles_cards[1].value = 3;
	visibles_cards[2].value = 4;
	visibles_cards[3].value = 5;
	visibles_cards[4].value = 12;
    players[0].hand[0].value = 13;
    players[0].hand[1].value = 14;
    visibles_cards[0].suit = 'P';
    visibles_cards[1].suit = 'P';
    visibles_cards[2].suit = 'H';
    visibles_cards[3].suit = 'P';
    visibles_cards[4].suit = 'P';
    players[0].hand[0].suit = 'P';
    players[0].hand[1].suit = 'H';*/

	struct card* seven_cards = malloc(7 * sizeof(struct card));

	for (int i = 0; i < 7; ++i){
		seven_cards[i] = visibles_cards[i];
		if(i == 5)
			seven_cards[i] = players[0].hand[0];
		else if(i == 6)
			seven_cards[i] = players[0].hand[1];
	}

    struct card* two_pair_combo = malloc(4 * sizeof(struct card));
    struct card* three_of_a_kind_combo = malloc(3 * sizeof(struct card));
    struct card* square_combo = malloc(4 * sizeof(struct card));
    struct card* flush_combo = malloc(5 * sizeof(struct card));
    struct card* straight_combo = malloc(5 * sizeof(struct card));
	two_pair_combo = two_pair_checker(seven_cards);
	three_of_a_kind_combo = three_of_a_kind(seven_cards);
	square_combo = square(seven_cards);
    flush_combo = flush(seven_cards);
    straight_combo = straight(seven_cards);

	for (int i = 0; i < 5; ++i){
		printf("visibles_cards %d%c\n", visibles_cards[i].value, visibles_cards[i].suit);
	}
	printf("first pair : %d%c%c\nscd pair : %d%c%c\n",two_pair_combo[0].value, two_pair_combo[0].suit, two_pair_combo[1].suit, two_pair_combo[2].value, two_pair_combo[2].suit, two_pair_combo[3].suit);
	printf("three of a kind : %d%c%c%c\n",three_of_a_kind_combo[0].value,three_of_a_kind_combo[0].suit,three_of_a_kind_combo[1].suit,three_of_a_kind_combo[2].suit);
    printf("square : %d%c%c%c%c\n",square_combo[0].value,square_combo[0].suit,square_combo[1].suit,square_combo[2].suit,square_combo[3].suit);
    printf("flush : %d:%d:%d:%d:%d/%c\n",flush_combo[0].value, flush_combo[1].value, flush_combo[2].value, flush_combo[3].value, flush_combo[4].value, flush_combo[0].suit);
    printf("straight :");
    for (int i = 0; i < 5; ++i)
        printf("%d%c:", straight_combo[i].value, straight_combo[i].suit);
    printf("\n");

	for (int i = 0; i < 8; ++i){
		printf("player %d ; pos %d ; card1 %d%c card2 %d%c ; tokens player:%f\n",players[i].player_num, players[i].position, players[i].hand[0].value, players[i].hand[0].suit, players[i].hand[1].value, players[i].hand[1].suit, players[i].tokens);
	}


    free(flush_combo);
    free(three_of_a_kind_combo);
    free(square_combo);
    free(two_pair_combo);
    free(seven_cards);
	free(visibles_cards);
    free(players);
    free(deck52);
    return 0;
}
