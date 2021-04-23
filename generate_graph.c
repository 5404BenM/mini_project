//Author: Benjamin Medoff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./metis-5.1.0/include/metis.h"

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

int group_root = 0;
int num_groups;
int num_edges = 0;

int breakHere(){
    char s[50];
    printf("Break Here:");
    gets(s);
}

//generate_graph() generates a graph to be passed to metis and saves it to "graph.txt"
int generate_graph(group** grp){
    //graph will be generated by creating fully connected groups and then adding connections between the groups
    //first we generate random parameters for rough graph creation
    num_groups = 100 + (rand()%10);
    int avg_group_size = 10 + (rand()%20);
    int avg_interconnects = 2; //this can be done randomly, but to make it easy on the partitioner, we start low

    //done manually rn to ensure it is easy for partition.
    int avg_in_group_edge_weight = 2;
    int avg_out_group_edge_weight = 6;

    int max_group_size = avg_group_size + 13;//from line 58
    //create groups
    sbrk(num_groups*sizeof(group) + num_groups*max_group_size*sizeof(vertex) +
     num_groups*max_group_size*max_group_size*sizeof(edge));
    *grp = malloc(num_groups*sizeof(group));
    group* groups = (*grp);

    if(groups == NULL)
        breakHere();

    for(int i = 0; i < num_groups; i++){
        groups[i].size = avg_group_size + (rand()%20 - 7);
        groups[i].verts = malloc(groups[i].size*sizeof(vertex));

        if(groups[i].verts == NULL)
            breakHere();

        groups[i].id = i;

        //fill out each vertex
        for(int j = 0; j < groups[i].size; j++){
            groups[i].verts[j].id = group_root + j;
            groups[i].verts[j].in_edges = malloc((groups[i].size - 1)*sizeof(edge));

            if(groups[i].verts[j].in_edges == NULL)
                breakHere();

            num_edges += groups[i].size - 1;
            groups[i].verts[j].out_edges = NULL;

            for(int k = 0; k < j; k++){
                groups[i].verts[j].in_edges[k].vert_id = group_root + k;
                groups[i].verts[j].in_edges[k].weight = avg_in_group_edge_weight + ((rand()%3)-1);
            }
            for(int k = j+1; k < groups[i].size; k++){
                groups[i].verts[j].in_edges[k-1].vert_id = group_root + k;
                groups[i].verts[j].in_edges[k-1].weight = avg_in_group_edge_weight + ((rand()%3)-1);
            }
        }
        group_root = group_root + groups[i].size;
    }

    //add interconnects
    for(int i = 0; i < num_groups; i++){
        for(int j = 0; j < avg_interconnects + ((rand()%3)-1); j++){
            num_edges++;
            vertex* vert = &groups[i].verts[rand()%groups[i].size];
            int other_group = (i+1+(rand()%(num_groups-1)))%num_groups;
            int add_index = rand()%groups[other_group].size;

            if(vert->out_edges == NULL){
                vert->out_edges = malloc(sizeof(edge));

                if(vert->out_edges == NULL)
                    breakHere();
                
                vert->out_size = 1;

                vert->out_edges[0].vert_id = groups[other_group].verts[add_index].id;
                vert->out_edges[0].weight = avg_out_group_edge_weight + ((rand()%5)-2);
            }
            else{
                edge* temp = malloc((vert->out_size+1)*sizeof(edge));
                
                if(temp == NULL)
                    breakHere();

                int adding_vert_id = groups[other_group].verts[add_index].id;

                for(int k = 0; k < vert->out_size; k++){
                    temp[k].vert_id = vert->out_edges[k].vert_id;
                    temp[k].weight = vert->out_edges[k].weight;

                    if(adding_vert_id == temp[k].vert_id){
                        add_index = (add_index +1)%groups[other_group].size;
                        adding_vert_id = groups[other_group].verts[add_index].id;
                    }
                }

                temp[vert->out_size].vert_id = adding_vert_id;
                temp[vert->out_size].weight = avg_out_group_edge_weight + ((rand()%5)-2);

                free(vert->out_edges);
                vert->out_edges = temp;
            }

            //add the edge to the other vertex.
            vertex* vert2 = &groups[other_group].verts[add_index];
            if(vert2->out_edges == NULL){
                vert2->out_edges = malloc(sizeof(edge));

                if(vert2->out_edges == NULL)
                    breakHere();
                
                vert2->out_size = 1;

                vert2->out_edges[0].vert_id = vert->id;
                vert2->out_edges[0].weight = avg_out_group_edge_weight + ((rand()%5)-2);
            }
            else{
                edge* temp = malloc((vert2->out_size+1)*sizeof(edge));

                if(temp == NULL)
                    breakHere();

                int adding_vert_id = vert2->id;

                for(int k = 0; k < vert2->out_size; k++){
                    temp[k].vert_id = vert2->out_edges[k].vert_id;
                    temp[k].weight = vert2->out_edges[k].weight;

                    if(adding_vert_id == temp[k].vert_id){
                        add_index = (add_index +1)%groups[other_group].size;
                        //fuck
                        adding_vert_id = (vert->id+1);
                    }
                }

                temp[vert2->out_size].vert_id = adding_vert_id;
                temp[vert2->out_size].weight = avg_out_group_edge_weight + ((rand()%5)-2);

                free(vert2->out_edges);
                vert2->out_edges = temp;
            }
        }
    }
}

int convert_to_csr(group* G, idx_t** adjncy, idx_t** xadj, idx_t** adjwgt){
    sbrk(2*num_edges*sizeof(idx_t) + (G[num_groups-1].size + group_root)*sizeof(idx_t));
    *adjncy = malloc(num_edges*sizeof(idx_t));
    *adjwgt = malloc(num_edges*sizeof(idx_t));
    *xadj = malloc((G[num_groups-1].size + group_root)*sizeof(idx_t));

    if(*adjncy == NULL || *adjwgt == NULL || *xadj == NULL)
        breakHere();

    int edgeIter = 0;
    int vertIter = 0;
    for(int i = 0; i < num_groups; i++)
    {
        for(int j = 0; j < G[i].size; j++){
            (*xadj)[vertIter] = edgeIter;

            //iterate through in group edges
            for(int k = 0; k < G[i].size-1; k++){
                (*adjncy)[edgeIter] = G[i].verts[j].in_edges[k].vert_id;
                (*adjwgt)[edgeIter] = G[i].verts[j].in_edges[k].weight;
                edgeIter++;
            }
            //iterate throgh out of group edges
            for(int k = 0; k < G[i].verts[j].out_size; k++){
                (*adjncy)[edgeIter] = G[i].verts[j].out_edges[k].vert_id;
                (*adjwgt)[edgeIter] = G[i].verts[j].out_edges[k].weight;
                edgeIter++;
            }
            vertIter++;
        }
    }
    return 0;
}

int main(){
    group** graph = malloc(sizeof(group*));
    generate_graph(graph);
    idx_t **adjncy = malloc(sizeof(idx_t*));
    idx_t **xadj = malloc(sizeof(idx_t*));
    idx_t **adjwgt = malloc(sizeof(idx_t*));
    convert_to_csr(*graph, adjncy, xadj, adjwgt);
    printf("%d %d %d %d %d\n", (*xadj)[0], (*xadj)[1], (*xadj)[2], (*xadj)[3], (*xadj)[4]);
}
