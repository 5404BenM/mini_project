//Author: Benjamin Medoff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <metis.h>
//#include "./metis-5.1.0/include/metis.h"

typedef struct edg{
    int vert_id;
    int weight;
} edge;

typedef struct vert{
    int id;
    edge* in_edges;
    int out_size;
    edge* out_edges;
} vertex;

typedef struct grp{
    int id;
    int size;
    vertex* verts;
} group;

//generate_graph() generates a graph to be passed to metis and saves it to "graph.txt"
int generate_graph(group** grp);

//convert_to_csr turns graph into csr format that metis uses
int convert_to_csr(group* G, idx_t** adjncy, idx_t** xadj, idx_t** adjwgt, idx_t* num_verts);