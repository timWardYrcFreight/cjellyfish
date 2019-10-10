#include "jellyfish.h"
#include "python.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum { false, true } bool;

double custom_weighted_levenshtein_distance(const JFISH_UNICODE *s1, int s1_len, const JFISH_UNICODE *s2, int s2_len, double insert_numeric_weight, double insert_alpha_weight, double delete_numeric_weight, double delete_alpha_weight, double substitute_numeric_weight, double substitute_alpha_weight)
{
    size_t rows = s1_len + 1;
    size_t cols = s2_len + 1;
    size_t i, j;
   
    double result;
    double delete_cost, insert_cost, substitute_cost;
    double insert_weight, delete_weight, substitute_weight;
    double *dist = malloc(rows * cols * sizeof(double));
    bool *isValid1 = calloc(s1_len, sizeof(bool));
    bool *isDigit1 = calloc(s1_len, sizeof(bool));

    if (!dist || !isValid1 || !isDigit1) {
        return -1;
    }

    // Determine isValid and isDigit for each character of s1 in advance, rather than multiple times each in the inner loop
    for (i = 0; i < s1_len; i++) {
        if(s1[i] == ' ' || Py_UNICODE_ISALNUM(s1[i]) == true) {
            isValid1[i] = true;
        }
        if(Py_UNICODE_ISDIGIT(s1[i])){
            isDigit1[i] = true;
        }
    }

    for (i = 0; i < rows; i++) {
        dist[i * cols] = i;
    }

    for (j = 0; j < cols; j++) {
        dist[j] = j;
    }

    for (j = 1; j < cols; j++) {
        bool isDigit2 = (Py_UNICODE_ISDIGIT(s2[j-1]) == true)?true:false;
        bool isValid2 = (s2[j-1] == ' ' || Py_UNICODE_ISALNUM(s2[j-1]) == true)?true:false;

        for (i = 1; i < rows; i++) {
            if (s1[i - 1] == s2[j - 1]) {
                dist[(i * cols) + j] = dist[((i - 1) * cols) + (j - 1)];
            } else {

                if(isValid2 == true) {
                    if(isDigit2 == true) {
                        insert_weight = insert_numeric_weight;
                    } else {
                        insert_weight = insert_alpha_weight;
                    }
                } else {
                    insert_weight = 0.0;
                }

                if(isValid1[i-1] == true) {
                    if(isDigit1[i-1] == true) {
                        delete_weight = delete_numeric_weight;
                    } else {
                        delete_weight = delete_alpha_weight;
                    }
                } else {
                    delete_weight = 0.0;
                }

                if(isValid1[i-1] == true && isValid2 == true) {
                    if(isDigit1[i-1] == true || isDigit2 == true) {
                        substitute_weight = substitute_numeric_weight;
                    } else {
                        substitute_weight = substitute_alpha_weight;
                    }
                } else {
                    substitute_weight = 0.0;
                }

                delete_cost = dist[((i - 1) * cols) + j] + delete_weight; 
                insert_cost = dist[(i * cols) + (j - 1)] + insert_weight;
                substitute_cost = dist[((i - 1) * cols) + (j - 1)] + substitute_weight;

                dist[(i * cols) + j] = MIN(delete_cost, MIN(insert_cost, substitute_cost));

            }
        }
    }

    result = dist[(cols * rows) - 1];

    free(dist);
    free(isValid1);
    free(isDigit1);

    return result;
}
