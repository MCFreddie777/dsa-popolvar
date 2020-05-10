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
    Node **heap;
    int size;
    int max_size;
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

void free_linkedlist (Node *start) {
    Node *actual = start;
    Node *prev = start->prev;
    
    while (prev != NULL) {
        free(actual);
        actual = prev;
        prev = actual->prev;
    };
}

void result (char **mapa, const int *t, int *cesta, const int dlzka_cesty) {
    int cas = 0;
    
    for (int i = 0; i < dlzka_cesty; i++) {
        printf("%d %d\n", cesta[i * 2], cesta[i * 2 + 1]);
        
        if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'H')
            cas += 2;
        else
            cas += 1;
        
        if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'D' && cas > *t)
            printf("Nestihol si zabit draka!\n");
        
        if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'N')
            printf("Prechod cez nepriechodnu prekazku!\n");
        
        if (i > 0 && (
            abs(cesta[i * 2 + 1] - cesta[(i - 1) * 2 + 1]) +
            abs(cesta[i * 2] - cesta[(i - 1) * 2]) > 1
        )) {
            printf("Neplatny posun Popolvara!\n");
        }
    }
    printf("%d\n", cas);
}

/**
 * Finds all the entities on the map and their coordinates
 *
 * @param map
 * @param dragon coordinates of the dragon
 * @param princesses array of princesses where they should be stored
 * @param n_of_princesses number of princesses
 */
void getEntities (
    Map *map,
    Coordinates *dragon,
    Coordinates **princesses,
    int *n_of_princesses
) {
    for (int i = 0; i < map->x; ++i) {
        for (int j = 0; j < map->y; ++j) {
            // We don't care about other entities
            if (map->map[i][j] != DRAGON && map->map[i][j] != PRINCESS) continue;
            
            if (map->map[i][j] == DRAGON && dragon) {
                dragon->x = i;
                dragon->y = j;
                continue;
            }
            // If the entity is princess
            if (*n_of_princesses != 0) {
                // enlarge the array of princesses
                *princesses = (Coordinates *) realloc(
                    *princesses,
                    (*n_of_princesses + 1) * sizeof(Coordinates)
                );
            }
            (*n_of_princesses)++;
            (*princesses)[(*n_of_princesses) - 1].x = i;
            (*princesses)[(*n_of_princesses) - 1].y = j;
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
 * Reverses a linked list
 */
Node *reverse_linkedlist (Node *last) {
    Node *current = last;
    Node *prev = NULL, *next = NULL;
    
    while (current != NULL) {
        next = current->prev;
        current->prev = prev;
        prev = current;
        current = next;
    }
    return prev;
}

/**
 * This function calculates the next node after moving according to rules
 *
 * @param node current node
 * @param map
 * @param x number of steps to move on x axis
 * @param y number of steps to move on y axis
 * @return new node on new direction | null if the map on those coordinates is BLOCK/NULL
 */
Node *move (Node *node, Map *map, const int x, const int y) {
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

void push (Node *node, Heap *heap) {
    heap->size++;
    
    if (heap->heap == NULL) {
        (heap->heap) = (Node **) malloc(heap->max_size * sizeof(Node *));
    }
    else {
        // Resize to double the length to avoid O(n^2) resizing.
        if (heap->size == heap->max_size) {
            heap->max_size *= 2;
            (heap->heap) = (Node **) realloc(heap->heap, (heap->max_size * sizeof(Node *)));
        }
    }
    
    // Insert to the very end of the heap
    heap->heap[heap->size - 1] = node;
    // Rearrange heap
    Node *temp = heap->heap[heap->size - 1];
    
    int i = heap->size - 1;
    Node *parent = heap->heap[(i - 1) / 2];
    
    while (i > 0 && temp->value < parent->value) {
        heap->heap[i] = parent;
        i = (i - 1) / 2;
        parent = heap->heap[(i - 1) / 2];
    }
    heap->heap[i] = temp;
}

Node *pop (Heap *heap) {
    Node *root = heap->heap[0];
    
    heap->size--;
    if (heap->size == 0) {
        heap->heap = NULL;
        return root;
    }
    
    // swap leaf and root
    heap->heap[0] = heap->heap[heap->size];
    
    int i = 0;
    int j = (2 * i) + 1;
    
    while (j < heap->size) {
        // select the lesser children
        if (heap->heap[j]->value > heap->heap[j + 1]->value) j++;
        if (heap->heap[i]->value <= heap->heap[j]->value) break;
        
        // swap the parent and child
        Node *temp = heap->heap[i];
        heap->heap[i] = heap->heap[j];
        heap->heap[j] = temp;
        
        // move the "parent" to the child
        i = j;
        
        // move the "child pointer" into new parents child
        j = (2 * j) + 1;
        
    }
    return root;
};

/**
 *
 * @param start
 * @param stop
 */
Node *dijkstra (Map *map, Node *start, Coordinates *stop) {
    // initialize the heap
    Heap *heap = (Heap *) malloc(sizeof(Heap));
    heap->size = 0;
    heap->max_size = 4;
    heap->heap = NULL;
    
    // boolean matrix to mark the visited / not visited nodes
    short **visited = (short **) malloc(map->x * sizeof(short *));
    for (int i = 0; i < map->x; i++) {
        visited[i] = (short *) calloc(map->y, sizeof(short));
    }
    
    // push starting node into the heap
    push(start, heap);
    
    // X & Y direction vectors
    const int sX[4] = {-1, 0, 1, 0};
    const int sY[4] = {0, 1, 0, -1};
    
    Node *actual;
    do {
        actual = pop(heap);
        visited[actual->coordinates.x][actual->coordinates.y] = 1;
        
        for (int i = 0; i < 4; i++) {
            Node *m = move(actual, map, sX[i], sY[i]);
            
            if (m == NULL || visited[m->coordinates.x][m->coordinates.y]) continue;
            
            m->prev = actual;
            push(m, heap);
        }
    } while (actual->coordinates.x != stop->x || actual->coordinates.y != stop->y);
    
    // Cleanup memory
    for (int i = 0; i < map->x; i++)
        free(visited[i]);
    free(visited);
    
    free(heap->heap);
    free(heap);
    
    return actual;
}

/**
 * Recursively gets the path from first to the last princess
 * @return
 */
Node *getLastPrincess (Map *map, Node *start, Coordinates *princesses, int n_of_princesses) {
    if (n_of_princesses == 1)
        // First princess from a permutated array
        return dijkstra(map, start, princesses);
    else {
        // Get the princess before
        Node *temp = getLastPrincess(map, start, princesses, n_of_princesses - 1);
        // Run dijkstra from princess before till current princess
        return dijkstra(map, temp, princesses + (n_of_princesses - 1));
    }
}

void swap (Coordinates *x, Coordinates *y) {
    Coordinates temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

/**
 * Function which permutes given array of princesses
 *
 * @param map
 * @param start start node where to start from in dijkstra
 * @param princesses array of princesses
 * @param n_of_princesses
 * @param index
 */
void permutePrincesses (
    Map *map,
    Node *start,
    Coordinates *princesses,
    int n_of_princesses,
    int index,
    Heap *result
) {
    for (int i = index; i < n_of_princesses; i++) {
        swap(princesses + i, princesses + index);
        permutePrincesses(map, start, princesses, n_of_princesses, index + 1, result);
        swap(princesses + index, princesses + i);
    }
    if (index == n_of_princesses - 1) {
        Node *last = getLastPrincess(map, start, princesses, n_of_princesses);
        
        // Push the result to the array of results
        result->size++;
        if (result->heap == NULL) {
            (result->heap) = (Node **) malloc(result->max_size * sizeof(Node *));
        }
        else {
            // Resize to double the length to avoid O(n^2) resizing.
            if (result->size == result->max_size) {
                result->max_size *= 2;
                (result->heap) = (Node **) realloc(
                    result->heap,
                    (result->max_size * sizeof(Node *))
                );
            }
        }
        result->heap[result->size - 1] = last;
    }
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
    getEntities(map, dragon, &princesses, &n_of_princesses);
    
    // Init heap with starting node
    Node *start = malloc(sizeof(Node));
    start->coordinates.x = 0;
    start->coordinates.y = 0;
    start->value = 1;
    start->prev = NULL;
    
    // Get the dragon, POPOLVAR!
    Node *path = dijkstra(map, start, dragon);
    
    // Result array to be stored all values of all permutations
    Heap *result = (Heap *) malloc(sizeof(Heap));
    result->size = 0;
    result->max_size = 4;
    result->heap = NULL;
    
    // Get the shortest path by permuting between princesses
    permutePrincesses(map, path, princesses, n_of_princesses, 0, result);
    
    // Get the minimum cost value
    Node *min = result->heap[0];
    for (int i = 1; i < result->size; i++) {
        if (result->heap[i]->value < min->value)
            min = result->heap[i];
    }
    path = min;
    
    /*
     * Fill the found path into array of int* in reverse order
     * (Requirement by the printing "result()" function given by the teacher)
     */
    int *arr = (int *) malloc(2 * path->value * sizeof(int));
    path = reverse_linkedlist(path);
    Node *actual = path;
    
    int i = 0;
    while (actual != NULL) {
        arr[i] = actual->coordinates.y;
        arr[i + 1] = actual->coordinates.x;
        i += 2;
        actual = actual->prev;
    };
    
    free_linkedlist(path);
    *dlzka_cesty = (i / 2);
    return arr;
}

int main () {
    char **mapa;
    int test, dlzka_cesty, *cesta;
    int n = 0, m = 0, t = 0;
    
    while (1) {
        printf("Zadajte cislo testu (0 ukonci program):\n");
        scanf("%d", &test);
        
        dlzka_cesty = 0;
        n = m = t = 0;
        switch (test) {
            case 0:
                return 0;
            case 1:
                mapa = file_load("input/test1.txt", &n, &m, &t);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 2:
                n = 10;
                m = 10;
                t = 12;
                mapa = (char **) malloc(n * sizeof(char *));
                mapa[0] = "CCHCNHCCHN";
                mapa[1] = "NNCCCHHCCC";
                mapa[2] = "DNCCNNHHHC";
                mapa[3] = "CHHHCCCCCC";
                mapa[4] = "CCCCCNHHHH";
                mapa[5] = "PCHCCCNNNN";
                mapa[6] = "NNNNNHCCCC";
                mapa[7] = "CCCCCPCCCC";
                mapa[8] = "CCCNNHHHHH";
                mapa[9] = "HHHPCCCCCC";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 3:
                mapa = file_load("input/test3.txt", &n, &m, &t);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 4:
                mapa = file_load("input/test4.txt", &n, &m, &t);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 5:
                mapa = file_load("input/test5.txt", &n, &m, &t);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 6:
                mapa = file_load("input/test6.txt", &n, &m, &t);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            default:
                continue;
        }
        result(mapa, &t, cesta, dlzka_cesty);
        free(cesta);
        for (int i = 0; i < n; i++) {
            free(mapa[i]);
        }
        free(mapa);
    }
    return 0;
}