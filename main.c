#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define VAR_NAME_SIZE 64 // based on the ISO C Standard for naming variables

typedef struct{
    enum{i, f, c} type;

    union{
        int i;
        float f;
        char c;
    } val;
} tagged_union;

typedef struct{
    char id[VAR_NAME_SIZE];
    int next;
    tagged_union data;
} tuple_t;

void createTuple(char id[VAR_NAME_SIZE], tagged_union in[], int dimS0);
void deleteTuple(char id[VAR_NAME_SIZE]);
void joinTuple(char id[VAR_NAME_SIZE], tuple_t* tuple_ptr_1, tuple_t* tuple_ptr_2);
tuple_t* getTupleByID(char search_id[VAR_NAME_SIZE]);
char* getTupleID(tuple_t* tuple_ptr);
void saveAllTuples(char path[]);
void loadAllTuples(char path[]);
void showTuple(tuple_t* tuple_ptr);

tuple_t *tuples;
int tuples_size = 0;

int main() {

    tuples = (tuple_t*)malloc(0);

    if (tuples == NULL) {
        puts("Memory allocation failed for initialisation of tuple array or stack.");
        exit(EXIT_FAILURE);
    }

    tagged_union test_1[2] = {{.type = i, .val.i = 10}, {.type = c, .val.c = 'T'}};
    tagged_union test_2[2] = {{.type = i, .val.i = 2}, {.type = c, .val.c = 'X'}};

    createTuple("abc", &test_1, 2);
    createTuple("efg", &test_2, 2);
    joinTuple("xyz", getTupleByID("efg"), getTupleByID("abc"));

    // showTuple(getTupleByID("test"));

    saveAllTuples("/Users/xandrumifsud/Desktop/test.txt");

    deleteTuple("efg");
    deleteTuple("abc");
    deleteTuple("xyz");

    loadAllTuples("/Users/xandrumifsud/Desktop/test.txt");

    return 0;
}

void createTuple(char id[VAR_NAME_SIZE], tagged_union in[], int dimS0){

    if(getTupleByID(id) != NULL){
        puts("Tuple ID already registered in stack.");
        exit(EXIT_FAILURE); // add graceful close were tuples in heap are saved
    }
    else{
        tuples = realloc(tuples, (tuples_size + dimS0) * sizeof(tuple_t));

        if (tuples == NULL) {
            puts("Memory re-allocation failed for new tuple creation.");
            exit(EXIT_FAILURE);
        }

        for(int j = tuples_size; j < (tuples_size + dimS0); j++){

            for (int k = 0; k < VAR_NAME_SIZE; k++){
                tuples[j].id[k] = id[k];
            }

            tuples[j].next = dimS0 - j + tuples_size;

            switch(in[j - tuples_size].type){
                case i: tuples[j].data.type = i;
                    tuples[j].data.val.i = in[j - tuples_size].val.i;
                    break;

                case f: tuples[j].data.type = f;
                    tuples[j].data.val.f = in[j - tuples_size].val.f;
                    break;

                case c: tuples[j].data.type = c;
                    tuples[j].data.val.c = in[j - tuples_size].val.c;
                    break;
            }
        }

        tuples_size += dimS0;
    }
}

tuple_t* getTupleByID(char search_id[VAR_NAME_SIZE]) {

    int dimS0_stack, match_count;
    int k = 0, dimS0_in = 0;
    tuple_t *match = NULL;

    while (search_id[k++] != '\0') {
        dimS0_in++;
    }

    for (int j = 0; j < tuples_size; j++) {
        k = 0;
        dimS0_stack = 0;
        match_count = 0;

        while (tuples[j].id[k++] != '\0') {
            dimS0_stack++;
        }

        if (dimS0_in != dimS0_stack) {
            j += tuples[j].next - 1;
            continue;
        } else {
            for (k = 0; k < dimS0_in; k++) {
                if (search_id[k] == tuples[j].id[k])
                    match_count++;
            }

            if (match_count < dimS0_in) {
                j += tuples[j].next - 1;
                continue;
            } else {
                match = &tuples[j];
                break;
            }
        }
    }

    return match;
}

char* getTupleID(tuple_t* tuple_ptr){

    if (tuple_ptr == NULL){
        return NULL;
    }
    else{
        return tuple_ptr->id;
    }
}

// add handling of null pointers

void deleteTuple(char id[VAR_NAME_SIZE]){

    tuple_t* tuple_ptr = getTupleByID(id);
    int size = tuple_ptr->next;
    long int copy_length = tuples_size - ((tuple_ptr + size) - tuples);
    tuples_size -= size;

    memmove(tuple_ptr, tuple_ptr + size, copy_length * sizeof(tuple_t));
    tuples = realloc(tuples, tuples_size * sizeof(tuple_t));
}

void joinTuple(char id[VAR_NAME_SIZE], tuple_t* tuple_ptr_1, tuple_t* tuple_ptr_2){

    int dimS0 = tuple_ptr_1->next + tuple_ptr_2->next;
    tagged_union joinedData[dimS0];

    for (int j = 0; j < tuple_ptr_1->next; j++){
        joinedData[j] = tuple_ptr_1[j].data;
    }

    for (int j = tuple_ptr_1->next; j < dimS0; j++){
        joinedData[j] = tuple_ptr_2[j - tuple_ptr_1->next].data;
    }

    createTuple(id, joinedData, dimS0);
}

void showTuple(tuple_t* tuple_ptr){

    if (tuple_ptr != NULL){

        switch(tuple_ptr->data.type){
            case i: printf("(%d,", tuple_ptr->data.val.i);
                break;

            case f: printf("(%f,", tuple_ptr->data.val.f);
                break;

            case c: printf("('%c',", tuple_ptr->data.val.c);
                break;
        }

        int last = tuple_ptr->next - 1;

        for (int j = 1; j < last; j++){
            switch((tuple_ptr + j)->data.type){
                case i: printf(" %d,", (tuple_ptr + j)->data.val.i);
                    break;

                case f: printf(" %f,", (tuple_ptr + j)->data.val.f);
                    break;

                case c: printf(" '%c',", (tuple_ptr + j)->data.val.c);
                    break;
            }
        }

        switch((tuple_ptr + last)->data.type){
            case i: printf(" %d)", (tuple_ptr + last)->data.val.i);
                break;

            case f: printf(" %f)", (tuple_ptr + last)->data.val.f);
                break;

            case c: printf(" '%c')", (tuple_ptr + last)->data.val.c);
                break;
        }
    }
}

void saveAllTuples(char path[]){

    FILE *fp;
    fp = fopen(path, "w");

    for(int j = 0; j < tuples_size; j++){

        fprintf(fp, "%s %d ", tuples[j].id, tuples[j].next);

        switch(tuples[j].data.type){
            case i: fprintf(fp, "%c %d\n", 'i', tuples[j].data.val.i);
                break;

            case f: fprintf(fp, "%c %f\n", 'f', tuples[j].data.val.f);
                break;

            case c: fprintf(fp, "%c %c\n", 'c', tuples[j].data.val.c);
                break;
        }
    }

    fclose(fp);
}

void loadAllTuples(char path[]){

    int new_line_size = 128, tuple_size = 0, tuple_size_temp = 0, current_size = 0;

    char tuple_id[VAR_NAME_SIZE];
    char format;
    int need_not_realloc;

    char *new_line;
    char *data;
    char *conversion_ptr;
    tagged_union *read_store;

    new_line = (char*)malloc(new_line_size * sizeof(char));
    data = (char*)malloc(new_line_size * sizeof(char));
    read_store = (tagged_union*)malloc(0);

    FILE *fp;
    fp = fopen(path, "r");

    if(fp != NULL){
        while(!feof(fp)){
            need_not_realloc = 0;

            fgets(new_line, new_line_size, fp);

            for(int j = new_line_size - 1; j >= 0; j--){
                if (new_line[j] == '\n'){
                    fseek(fp, -j-1, SEEK_CUR);
                    fscanf(fp,"%s %d %c %s\n", tuple_id, &tuple_size_temp, &format, data);

                    if(tuple_size_temp >= current_size){
                        tuple_size = tuple_size_temp;
                        read_store = (tagged_union*)realloc(read_store, tuple_size * sizeof(tagged_union));
                    }

                    current_size = tuple_size_temp;
                    int k = tuple_size - current_size;

                    // to add error codes
                    switch(format){
                        case 'i': read_store[k].type = i;
                            long fetch_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_int <= INT_MAX && fetch_int >= INT_MIN){
                                read_store[k].val.i = strtol(data, &conversion_ptr, 10);
                            }
                            break;

                        case 'f': read_store[k].type = f;
                            read_store[k].val.f = strtof(data, &conversion_ptr);
                            break;

                        case 'c': read_store[k].type = c;
                            if(data[1] == '\0'){
                                read_store[k].val.c = data[0];
                            }
                            break;

                        default: break; //add error code
                    }

                    if(current_size == 1){
                        createTuple(tuple_id, read_store, tuple_size);
                    }

                    need_not_realloc = 1;
                    break;
                }
            }

            if (need_not_realloc == 0){
                fseek(fp, 1 - new_line_size, SEEK_CUR);

                new_line_size *= 2;
                new_line = (char*)realloc(new_line, new_line_size * sizeof(char));
                data = (char*)realloc(data, new_line_size * sizeof(char));
            }
        }
    }

    fclose(fp);

    free(new_line);
    free(data);
    free(read_store);
}