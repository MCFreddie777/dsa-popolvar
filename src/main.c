//
// Created by František Gič on 07/05/2020.
//

#include <stdio.h>
#include <stdlib.h>

#define DRAGON 'D'
#define PRINCESS 'P'
#define BLOCK 'N'
#define BUSH 'H'

typedef struct Coordinates {
    int x;
    int y;
} Coordinates;

typedef struct Node {
    Coordinates coordinates;
    int value;
    struct Node *prev;
} Node;

typedef struct Heap {
    Node *heap;
    int size;
} Heap;

typedef struct Map {
    char **map;
    int x;
    int y;
} Map;

/**
 * Loads the file from the map
 *
 * @param path Path to the file on the disk (relative to the project root)
 * @param x height of the map
 * @param y width of the map
 * @param t time of the map
 * @return map
 */
char **file_load (char *path, int *x, int *y, int *t) {
    FILE *f;
    
    f = fopen(path, "r");
    
    if (f)
        fscanf(f, "%d %d %d", x, y, t);
    else {
        printf("Cannot read file \"%s\"", path);
        exit(1);
    }
    
    char **map = (char **) malloc(*x * sizeof(char *));
    
    for (int i = 0; i < *x; i++) {
        map[i] = (char *) malloc(*y * sizeof(char));
        for (int j = 0; j < *y; j++) {
            char c = (char) fgetc(f);
            if (c == '\n') c = (char) fgetc(f);
            map[i][j] = c;
        }
    }
    
    fclose(f);
    return map;
}

/**
 * Pretty prints the map to the console
 *
 * @param map
 * @param x height of the map
 * @param y width of the map
 */
void print_map (char **map, const int *x, const int *y) {
    for (int i = 0; i < *x; ++i) {
        for (int j = 0; j < *y; ++j) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

/**
 * Pretty prints the path to the console
 * @param path the path, min heap of coordinates
 * @param path_length
 */
void print_path (int *path, const int *path_length) {
    for (int i = 0; i < *path_length; i++) {
        printf("%d %d", path[i * 2 + 1], path[i * 2]);
    }
}

/**
 * Frees the 2D array memory
 *
 * @param map the map
 * @param x height of the map
 */
void free_map (char **map, const int *x) {
    for (int i = 0; i < *x; i++) {
        free(map[i]);
    }
    free(map);
}


void result (char **map, const int *t, int *path, const int *path_length) {
    int time = 0;
    
    for (int i = 0; i < *path_length; i++) {
        printf("%d %d\n", path[i * 2], path[i * 2 + 1]);
        
        if (map[path[i * 2 + 1]][path[i * 2]] == 'H')
            time += 2;
        else
            time += 1;
        
        if (map[path[i * 2 + 1]][path[i * 2]] == 'D' && time > *t)
            printf("Nestihol si zabit draka!\n");
        
        if (map[path[i * 2 + 1]][path[i * 2]] == 'N')
            printf("Prechod cez nepriechodnu prekazku!\n");
        
        if (i > 0 && (
            abs(path[i * 2 + 1] - path[(i - 1) * 2 + 1]) +
            abs(path[i * 2] - path[(i - 1) * 2]) > 1
        )) {
            printf("Neplatny posun Popolvara!\n");
        }
    }
    printf("%d\n", time);
}

/**
 * Finds all the entities on the map and their coordinates
 *
 * @param map
 * @param dragon coordinates of the dragon
 * @param princesses array of princesses where they should be stored
 * @param n_of_princessess number of princesses
 */
void getEntities (
    Map *map,
    Coordinates *dragon,
    Coordinates *princesses,
    int *n_of_princessess
) {
    for (int i = 0; i < map->x; ++i) {
        for (int j = 0; j < map->y; ++j) {
            if (map->map[i][j] == DRAGON && dragon) {
                dragon->x = i;
                dragon->y = j;
            }
            else if (map->map[i][j] == PRINCESS) {
                if (*n_of_princessess != 0) {
                    // enlarge the array of princesses
                    princesses = (Coordinates *) realloc(
                        princesses,
                        (*n_of_princessess + 1) * sizeof(Coordinates)
                    );
                }
                ++(*n_of_princessess);
                princesses[(*n_of_princessess) - 1].x = i;
                princesses[(*n_of_princessess) - 1].y = j;
            }
        }
    }
}

/**
 * Helper function to check whether those coordinates
 * are not out of boundaries of the map
 *
 * @param map
 * @param x
 * @param y
 * @return
 */
short outOfBounds (Map *map, int x, int y) {
    return (short) (x < 0 || x >= map->x || y < 0 || y >= map->y);
};

/**
 * This function calculates the next node after moving according to rules
 *
 * @param node current node
 * @param map
 * @param x number of steps to move on x axis
 * @param y number of steps to move on y axis
 * @return new node on new direction | null if the map on those coordinates is BLOCK/NULL
 */
Node *move (Node *node, Map *map, int x, const int y) {
    // If we're out of the map or the pathway is blocked
    if (outOfBounds(map, (node->coordinates.x + x), node->coordinates.y + y) ||
        map->map[node->coordinates.x + x][node->coordinates.y + y] == BLOCK
        ) {
        return NULL;
    }
    
    Node *new = (Node *) malloc(sizeof(Node));
    new->coordinates.x = node->coordinates.x + x;
    new->coordinates.y = node->coordinates.y + y;
    
    // Remember the previous node
    new->prev = node;
    
    // Increment the pathway "cost" according to map
    new->value = node->value + (map->map[new->coordinates.x][new->coordinates.y] == BUSH ? 2 : 1);
    
    return new;
}

void insert (Node *node, Heap **heap) {
    if (node != NULL) {
        ++(*heap)->size;
        
        if ((*heap)->heap == NULL) {
            ((*heap)->heap) = (Node *) malloc(sizeof(Node));
        }
        else {
            // Enlarge the heap by one element
            ((*heap)->heap) = (Node *) realloc((*heap)->heap, ((*heap)->size * sizeof(Node)));
        }
        
        // Insert to the very end of the heap
        Node *last_element = &((*heap)->heap[(*heap)->size - 1]);
        *last_element = *node;
        
        // Rearrange heap
        Node temp = *last_element;
        
        int i = (*heap)->size - 1;
        Node *parent = &((*heap)->heap[(i - 1) / 2]);
        
        while (i > 0 && temp.value < parent->value) {
            (*heap)->heap[i] = *parent;
            i = (i - 1) / 2;
            parent = &((*heap)->heap[(i - 1) / 2]);
        }
        (*heap)->heap[i] = temp;
    }
}

Node pop (Heap **heap) {
    Node root = (*heap)->heap[0];
    
    (*heap)->size--;
    if ((*heap)->size == 0) {
        (*heap)->heap = NULL;
        return root;
    }
    
    // swap leaf and root
    (*heap)->heap[0] = (*heap)->heap[(*heap)->size];
    
    // shrink the array
    ((*heap)->heap) = (Node *) realloc((*heap)->heap, ((*heap)->size * sizeof(Node)));
    
    int i = 0;
    int j = (2 * i) + 1;
    
    while (j < (*heap)->size) {
        // select the lesser children
        if ((*heap)->heap[j].value > (*heap)->heap[j + 1].value) {
            j = j + 1;
        }
        
        // swap the parent and child
        if ((*heap)->heap[i].value > (*heap)->heap[j].value) {
            Node temp = (*heap)->heap[i];
            (*heap)->heap[i] = (*heap)->heap[j];
            (*heap)->heap[j] = temp;
            
            // move the "parent" to the child
            i = j;
            
            // move the "child pointer" into new parents child
            j = (2 * j) + 1;
        }
        else break;
    }
};

/**
 *
 * @param start
 * @param stop
 */
Heap *dijkstra (Map *map, Node *start, Coordinates *stop) {
    Heap *heap = malloc(sizeof(Heap));
    heap->size = 0;
    heap->heap = NULL;
    
    insert(start, &heap);
    Node actual = pop(&heap);
    
    while (actual.coordinates.x != stop->x || actual.coordinates.y != stop->y) {
        insert(move(&actual, map, -1, 0), &heap); // up
        insert(move(&actual, map, 0, +1), &heap);  // right
        insert(move(&actual, map, +1, 0), &heap); // down
        insert(move(&actual, map, 0, -1), &heap); // left
        actual = pop(&heap);
    }
    
    return heap;
}

int *zachran_princezne (char **mapa, int n, int m, int t, int *dlzka_cesty) {
    Coordinates *dragon = (Coordinates *) malloc(sizeof(Coordinates));
    Coordinates *princesses = (Coordinates *) malloc(sizeof(Coordinates));
    int n_of_princesses = 0;
    
    // initialize the map object
    Map *map = (Map *) malloc(sizeof(Map));
    map->map = mapa;
    map->x = n;
    map->y = m;
    
    // get the dragons and princesses' coordinates
    getEntities(map, dragon, princesses, &n_of_princesses);
    
    // Init heap with starting node
    Node *start = malloc(sizeof(Node));
    start->coordinates.x = 0;
    start->coordinates.y = 0;
    start->value = 1;
    start->prev = NULL;
    
    // Get the dragon, POPOLVAR!
    Heap *path = dijkstra(map, start, dragon);
    
    return (int *) calloc(1, sizeof(int)); // TODO return minheap of coordinates
}

int main () {
    int n = 0, m = 0, t = 0, path_length = 0;
    
    char **map = file_load("input/test.txt", &n, &m, &t);
    int *path = zachran_princezne(map, n, m, t, &path_length);
    //result(map, &t, path, &path_length);
    
    //print_map(map, &n, &m);
    //print_path(path, &path_length);
    
    free(path);
    free_map(map, &n);
    return 0;
}