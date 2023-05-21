
#include <stdio.h>
#include <stdlib.h>

struct card {
    int value;
    char suit;
    int already_taken;
};

int compare_cards(const void *a, const void *b) {
    const struct card *card_a = (const struct card*)a;
    const struct card *card_b = (const struct card*)b;
    return card_a->value - card_b->value;
}

struct card* flush(struct card* seven_cards) {
    int i, j, count;
    struct card* flush_cards = malloc(5*sizeof(struct card));
    char suits[4] = {'C', 'D', 'H', 'S'};

    // Check each suit for flush
    for (i = 0; i < 4; i++) {
        count = 0;
        // Count the number of cards with the current suit
        for (j = 0; j < 7; j++) {
            if (!seven_cards[j].already_taken && seven_cards[j].suit == suits[i]) {
                count++;
            }
        }
        // If there are 5 or more cards with the current suit, add them to the flush_cards array
        if (count >= 5) {
            int k = 0;
            for (j = 0; j < 7 && k < 5; j++) {
                if (!seven_cards[j].already_taken && seven_cards[j].suit == suits[i]) {
                    flush_cards[k] = seven_cards[j];
                    seven_cards[j].already_taken = 1;
                    k++;
                }
            }
            break;
        }
    }

    // If no flush was found, return NULL
    if (i == 4) {
        free(flush_cards);
        return NULL;
    }

    // Sort the flush_cards array by value
    for (i = 0; i < 4; i++) {
        for (j = i+1; j < 5; j++) {
            if (flush_cards[i].value < flush_cards[j].value) {
                struct card temp = flush_cards[i];
                flush_cards[i] = flush_cards[j];
                flush_cards[j] = temp;
            }
        }
    }

    return flush_cards;
}

struct card* straight_flush(struct card* seven_cards) {

    struct card* five_cards = malloc(5 * sizeof(struct card));

    five_cards = flush(seven_cards);

    int has_2 = 0, has_3 = 0, has_4 = 0, has_14 = 0;
    for (int i = 0; i < 5; ++i) {
        if (five_cards[i].value == 2) has_2 = 1;
        if (five_cards[i].value == 3) has_3 = 1;
        if (five_cards[i].value == 4) has_4 = 1;
        if (five_cards[i].value == 14) has_14 = 1;
    }
    if (has_2 && has_3 && has_4 && has_14) {
        for (int i = 0; i < 5; ++i) {
            if (five_cards[i].value == 14) {
                five_cards[i].value = 1;
                break;
            }
        }
    }

    qsort(five_cards, 5, sizeof(struct card), compare_cards);
    struct card* straight = malloc(5*sizeof(struct card));
    for (int i = 0; i < 5; ++i){
        straight[i].value = -1;//if it not straight
        straight[i].suit = 'n';
    }

    int k = 1;

    for (int i = 0; i < 5; ++i){
        for (int j = 0; j < 5; ++j){
            if (five_cards[i].value+1 == five_cards[j].value){
                ++k;
                if(k==2){
                    straight[0] = five_cards[i];
                    straight[1] = five_cards[j];
                    j=5;
                }
                else if(k==3){
                    straight[2] = five_cards[j];
                    j=5;
                }
                else if(k==4){
                    straight[3] = five_cards[j];
                    j=5;
                }
                else if(k==5){
                    straight[4] = five_cards[j];
                    return straight;
                }
            }
            else if (j==4 && five_cards[i].value+1 != five_cards[j].value)
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

int main() {
    struct card* seven_cards = malloc(7 * sizeof(struct card));

    // create some sample cards
    seven_cards[0].value = 2;
    seven_cards[0].suit = 's';
    seven_cards[1].value = 5;
    seven_cards[1].suit = 's';
    seven_cards[2].value = 9;
    seven_cards[2].suit = 'h';
    seven_cards[3].value = 10;
    seven_cards[3].suit = 'h';
    seven_cards[4].value = 11;
    seven_cards[4].suit = 'h';
    seven_cards[5].value = 12;
    seven_cards[5].suit = 'h';
    seven_cards[6].value = 13;
    seven_cards[6].suit = 'h';

    // test flush()
    printf("Flush:\n");
    struct card* flush_cards = flush(seven_cards);
    for (int i = 0; i < 5; ++i) {
        printf("%d%c ", flush_cards[i].value, flush_cards[i].suit);
    }
    printf("\n");

    // test straight_flush()
    printf("Straight flush:\n");
    struct card* straight_flush_cards = straight_flush(seven_cards);
    for (int i = 0; i < 5; ++i) {
        printf("%d%c ", straight_flush_cards[i].value, straight_flush_cards[i].suit);
    }
    printf("\n");

    return 0;
}
