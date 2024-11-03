#include <stdio.h>
#include <stdlib.h>
#include "repl.c"
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {
    Table table;
    initialize_table(&table);
    read_rows_from_csv(&table);  // Chargement des données existantes
    
    repl(&table);  // Lancer la REPL avec la table en mémoire
    return 0;
}
