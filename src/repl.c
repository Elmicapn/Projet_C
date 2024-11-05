#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef struct {
  int id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
  Row* rows;
  size_t num_rows;
} Table;

typedef struct {
  StatementType type;
  Row row_to_insert;  // Nouveau champ pour stocker la ligne à insérer
} Statement;

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;
  return input_buffer;
}

void print_prompt() { printf("db > "); }

void r_input(InputBuffer* input_buffer) {  // Libére le buffer précédent s'il existe
  if (input_buffer->buffer != NULL) {
    free(input_buffer->buffer);
  }

  input_buffer->buffer = (char*)malloc(256); // Taille arbitraire pour la ligne d'entrée
  if (input_buffer->buffer == NULL) {
    printf("Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  if (fgets(input_buffer->buffer, 256, stdin) == NULL) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  input_buffer->input_length = strlen(input_buffer->buffer);
  if (input_buffer->buffer[input_buffer->input_length - 1] == '\n') {
    input_buffer->buffer[input_buffer->input_length - 1] = 0;
    input_buffer->input_length--;
  }
}


void stop_input(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void initialize_table(Table* table) {
    table->rows = (Row*)malloc(sizeof(Row) * 100);
    table->num_rows = 0;
}

// Sauvegarde d'une ligne dans leCSV
void wrows(Row* row) {
    FILE* file = fopen("database.csv", "a");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d,%s,%s\n", row->id, row->username, row->email);
    fclose(file);
}

// Chargement des données depuis le CSV avec le read
void rrows(Table* table) {
    FILE* file = fopen("database.csv", "r");
    if (file == NULL) {
        printf("Aucun fichier de données trouvé. Création d'une nouvelle base de données.\n");
        return;
    }
    Row row;
    while (fscanf(file, "%d,%31[^,],%254[^\n]\n", &row.id, row.username, row.email) == 3) {
        table->rows[table->num_rows++] = row;
    }
    fclose(file);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    stop_input(input_buffer);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        
        // je commence a implementer le insert avec comme commande  ala suite le ID, username et le mail
        int id;
        char username[COLUMN_USERNAME_SIZE];
        char email[COLUMN_EMAIL_SIZE];
        
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %31s %254s", &id, username, email);
        if (args_assigned < 3) {
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }

        statement->row_to_insert.id = id;
        strncpy(statement->row_to_insert.username, username, COLUMN_USERNAME_SIZE);
        strncpy(statement->row_to_insert.email, email, COLUMN_EMAIL_SIZE);
        
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}


void insert_row(Table* table, Row* row) {
    if (table->num_rows >= 100) {
        printf("Table is full. Cannot insert more rows.\n");
        return;
    }
    
    // Ajoute la ligne dans la table
    table->rows[table->num_rows] = *row;
    table->num_rows++;
    wrows(row);
}


// Exécution des commandes SQL
void execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            insert_row(table, &statement->row_to_insert);
            printf("Row inserted.\n");
            break;
        case (STATEMENT_SELECT):
            rrows(table);

            for (size_t i = 0; i < table->num_rows; i++) {
                Row* row = &(table->rows[i]);
                printf("(%d, %s, %s)\n", row->id, row->username, row->email);
            }
            break;
    }
}

void repl(Table* table) {
  InputBuffer* input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    r_input(input_buffer);
    if (input_buffer->buffer[0] == '.') {
      switch (do_meta_command(input_buffer)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'\n", input_buffer->buffer);
          continue;
      }
    }
    Statement statement;
    switch (prepare_statement(input_buffer, &statement)) {
      case (PREPARE_SUCCESS):
        printf("recognized statement\n");
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n",
               input_buffer->buffer);
        continue;
    }
     execute_statement(&statement, table);
     printf("Executed.\n");
  }
}
