//Author: Benjamin Medoff
//#include "generate_graph.h"
#include <metis.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    /*group** graph;
    generate_graph(graph);
    idx_t **adjncy = malloc(sizeof(idx_t*));
    idx_t **xadj = malloc(sizeof(idx_t*));
    idx_t **adjwgt = malloc(sizeof(idx_t*));
    idx_t num_verts = 0;
    convert_to_csr(*graph, adjncy, xadj, adjwgt, &num_verts);*/
    
    idx_t* adjncy = malloc(100*sizeof(idx_t));
    idx_t* adjwgt = malloc(100*sizeof(idx_t));
    idx_t* xadj = malloc(25*sizeof(idx_t));
    int iter = 0;
    int base = 0;
    for (int i = 0; i < 25; i++){
        xadj[i] = i*4;
        if(i%5 == 0 && i != 0)
            base +=5;
        for(int j = 0; j < 5; j++){
            if(j == i%5)
                continue;
            adjncy[iter] = base + j;
            adjwgt[iter] = 1;
            iter++;
        }
    }

    idx_t num_verts = 25;
    idx_t num_con = 1;
    idx_t num_partitions = 5;
    idx_t options[METIS_NOPTIONS];
    options[METIS_OPTION_PTYPE] = METIS_PTYPE_RB;
    options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
    options[METIS_OPTION_CTYPE] = METIS_CTYPE_RM;
    options[METIS_OPTION_IPTYPE] = METIS_IPTYPE_GROW;
    options[METIS_OPTION_RTYPE] = METIS_RTYPE_FM;
    options[METIS_OPTION_NCUTS] = num_partitions;
    options[METIS_OPTION_NUMBERING] = 0;//c numbering
    options[METIS_OPTION_NITER] = 10;
    options[METIS_OPTION_UFACTOR] = 1;
    idx_t* edge_cut;
    idx_t* part;
    int r = METIS_PartGraphRecursive(&num_verts, &num_con, xadj, adjncy, NULL, NULL, adjwgt,
     &num_partitions, NULL, NULL,  options, edge_cut, part );
    
    printf("finished\n");
}