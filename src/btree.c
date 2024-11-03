#include <stdio.h>
#include <stdlib.h>
#include "repl.c"
#include <stdbool.h>
#include <string.h>



void btree(){





}

int nombreNoedsNiveau(node *root, int N){
    if (root!=NULL) {
        if(N==0)
            return 1;
        else{
            return nombreNoedsNiveau(root->droit, N-1)+nombreNoedsNiveau(root->gauche, N-1);
        }
    }
    return 0;
}