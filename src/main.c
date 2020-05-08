//
// Created by František Gič on 07/05/2020.
//

#include <stdio.h>
#include <stdlib.h>

#define DRAGON 'D'
#define PRINCESS 'P'
#define BLOCK 'N'
#define BUSH 'H'
#define PATH 'C'

typedef struct coordinates {
    int x;
    int y;
} coordinates;

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
 * @param x height of the map
 * @param y width of the map
 * @param dragon coordinates of the dragon
 * @param princesses array of princesses where they should be stored
 * @param n_of_princessess number of princesses
 */
void getEntities (
    char **map,
    const int *x,
    const int *y,
    coordinates **dragon,
    coordinates **princesses,
    int *n_of_princessess
) {
    for (int i = 0; i < *x; ++i) {
        for (int j = 0; j < *y; ++j) {
            // get the (one and only) dragon coords
            if (map[i][j] == DRAGON) {
                *dragon = (coordinates *) malloc(sizeof(coordinates));
                (*dragon)->x = i;
                (*dragon)->y = j;
            }
            else if (map[i][j] == PRINCESS) {
                if (*princesses == NULL) {
                    // initialize the array of princesses
                    *n_of_princessess = 1;
                    *princesses = (coordinates *) malloc(sizeof(coordinates));
                }
                else {
                    // enlarge the array of princesses
                    *princesses = (coordinates *) realloc(
                        *princesses,
                        ++(*n_of_princessess) * sizeof(coordinates)
                    );
                }
                (*princesses)[*n_of_princessess - 1].x = i;
                (*princesses)[*n_of_princessess - 1].y = j;
            }
        }
    }
}

int *zachran_princezne (char **mapa, int n, int m, int t, int *dlzka_cesty) {
    coordinates *dragon = NULL;
    coordinates *princesses = NULL;
    int n_of_princesses = 0;
    
    // get the dragons and princesses' coordinates
    getEntities(mapa, &n, &m, &dragon, &princesses, &n_of_princesses);
    coordinates *start = {0, 0};
    
    return (int *) calloc(1, sizeof(int));
}

int main () {
    int n = 0, m = 0, t = 0, path_length = 0;
    
    char **map = file_load("input/test.txt", &n, &m, &t);
    
    int *path = zachran_princezne(map, n, m, t, &path_length);
    result(map, &t, path, &path_length);
    
    print_map(map, &n, &m);
    print_path(path, &path_length);
    
    free(path);
    free_map(map, &n);
    return 0;
}