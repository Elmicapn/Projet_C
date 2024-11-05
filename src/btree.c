#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255


typedef struct {
    int id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct Node {
    Row row;
    struct Node* left;
    struct Node* right;
} Node;

// Nouveau noeud
Node* create_node(Row row) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->row = row;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Insere une ligne dans l'arbre
Node* insert_node(Node* root, Row row) {
    if (root == NULL) {
        return create_node(row);
    }
    if (row.id < root->row.id) {
        root->left = insert_node(root->left, row);
    } else if (row.id > root->row.id) {
        root->right = insert_node(root->right, row);
    }
    return root;
}

// Parcours l'arbre
void in_order_traversal(Node* root) {
    if (root != NULL) {
        in_order_traversal(root->left);
        printf("(%d, %s, %s)\n", root->row.id, root->row.username, root->row.email);
        in_order_traversal(root->right);
    }
}
