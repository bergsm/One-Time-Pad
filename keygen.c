/* Author: Shawn Berg
 *
 * This file creates a key file of specified length
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]) {

    // seed random number generator
    srand(time(NULL));

    int key;
    int keylen;

    // check for arg
    if (argc == 2) {
        keylen = atoi(argv[1]); 
    } else {
        fprintf(stderr, "incorrect number of arguments\n");
        exit(0);
    }

    // generate the random keys
    for (int i=0; i<keylen; i++) {
        key=(rand() % 27);
        if (key == 0) {
            key=' ';
        } else {
            key += 64;
        }
        printf("%c", key);
    }
    printf("\n");


    return 0;
}

