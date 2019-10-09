#include "jellyfish.h"
#include "python.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

double weighted_levenshtein_distance(const JFISH_UNICODE *s1, int s1_len, const JFISH_UNICODE *s2, int s2_len, PyObject *insert_weights, PyObject *delete_weights, PyObject *substitute_weights)
{
    size_t rows = s1_len + 1;
    size_t cols = s2_len + 1;
    size_t i, j;

    double result;
    double delete_cost, insert_cost, substitute_cost;
    PyObject *insert_weight, *delete_weight, *substitute_weight;
    double *dist = malloc(rows * cols * sizeof(double));
    if (!dist) {
        return -1;
    }

    for (i = 0; i < rows; i++) {
        dist[i * cols] = i;
    }


    for (j = 0; j < cols; j++) {
        dist[j] = j;
    }

    for (j = 1; j < cols; j++) {
        for (i = 1; i < rows; i++) {
            if (s1[i - 1] == s2[j - 1]) {
                dist[(i * cols) + j] = dist[((i - 1) * cols) + (j - 1)];
            } else {
                delete_weight = PyDict_GetItem(delete_weights, Py_BuildValue("u#", &s1[i-1], 1));
                insert_weight = PyDict_GetItem(insert_weights, Py_BuildValue("u#", &s2[j-1], 1));
                substitute_weight = PyDict_GetItem(substitute_weights, Py_BuildValue("(u#u#)", &s1[i-1], 1, &s2[j-1], 1));

                delete_cost = dist[((i - 1) * cols) + j] + (delete_weight!=NULL?PyFloat_AsDouble(delete_weight):1.0); 
                insert_cost = dist[(i * cols) + (j - 1)] + (insert_weight!=NULL?PyFloat_AsDouble(insert_weight):1.0);
                substitute_cost = dist[((i - 1) * cols) + (j - 1)] + (substitute_weight!=NULL?PyFloat_AsDouble(substitute_weight):1.0);

                dist[(i * cols) + j] = MIN(delete_cost, MIN(insert_cost, substitute_cost));
            }
        }
    }



    result = dist[(cols * rows) - 1];
//    printf("Final : %f\n", dist[(cols * rows) - 1]);
    fflush(stdout);
    free(dist);

    return result;
}
