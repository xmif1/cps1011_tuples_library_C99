//
// Created by Xandru Mifsud on 09/12/2018.
//

#ifndef TUPLE_T_TUPLES_T_H
#define TUPLE_T_TUPLES_T_H
#endif //TUPLE_T_TUPLES_T_H

#define VAR_NAME_SIZE 64 // based on the ISO C Standard for naming variables

typedef struct{
    enum{d, u, hi, hu, li, lu, lli, llu, f, lf, Lf, c, s} type;

    union{
        int d;
        unsigned int u;
        short int hi;
        unsigned short int hu;
        long int li;
        unsigned long int lu;
        long long int lli;
        unsigned long long int llu;
        float f;
        double lf;
        long double Lf;
        char c;
        char *s;
    } val;
} tagged_union; // defines a union structure where data type can be tracked

typedef struct{
    char id[VAR_NAME_SIZE];
    int next;
    tagged_union data;
} tuple_t; // defined the tuple structure, with format specification as defined by tagged_union

void createTuple(char id[VAR_NAME_SIZE], tagged_union in[], int dimS0);
void deleteTuple(char id[VAR_NAME_SIZE]);
void joinTuple(char id[VAR_NAME_SIZE], tuple_t* tuple_ptr_1, tuple_t* tuple_ptr_2);
tuple_t* getTupleByID(char search_id[VAR_NAME_SIZE]);
char* getTupleID(tuple_t* tuple_ptr);
void saveAllTuples(char path[]);
void loadAllTuples(char path[]);
void showTuple(tuple_t* tuple_ptr);
void ptr_alloc_valid(void *ptr);