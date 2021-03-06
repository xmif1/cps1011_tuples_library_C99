#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "tuples_t.h" // include main project header file

tuple_t *tuples;
int tuples_size = 0;

char **str_ptrs;
int str_ptrs_size = 0;

int main() {

    tuples = (tuple_t*)malloc(0); // malloc array of tuple_t structs
    ptr_alloc_valid((void*) tuples);
    str_ptrs = (char**)malloc(0); // malloc array of str_ptrs structs
    ptr_alloc_valid((void*) str_ptrs);

    return 0;
}

/* Creates a tuple with an associated name (specified by id), with its data formatting enforced by a tagged_union
 * struct. User is required to specify size of struct, dimS0.*/
void createTuple(char id[VAR_NAME_SIZE], tagged_union in[], int dimS0){

    // Ensure unique tuple id by means of the getTupleByID() function
    if(getTupleByID(id) != NULL){
        printf("TUPLE_CREATE_ERROR: Tuple ID %s already registered in stack.\n", id);
    }
    else{
        tuples = realloc(tuples, (tuples_size + dimS0) * sizeof(tuple_t)); // adjust size to hold new tuple data
        ptr_alloc_valid((void*) tuples);

        // loop to populate new allocated memory via returned pointer
        for(int j = tuples_size; j < (tuples_size + dimS0); j++){

            for (int k = 0; k < VAR_NAME_SIZE; k++){ // set tuple id for j^th entry
                tuples[j].id[k] = id[k];
            }

            // store int increment such that the next tuple in line is accessed from current pointer location
            tuples[j].next = dimS0 + tuples_size - j;

            // assigning of type and values to tagged_union within tuple_t
            switch(in[j - tuples_size].type){
                case d: tuples[j].data.type = d;
                    tuples[j].data.val.d = in[j - tuples_size].val.d;
                    break;

                case u: tuples[j].data.type = u;
                    tuples[j].data.val.u = in[j - tuples_size].val.u;
                    break;

                case hi: tuples[j].data.type = hi;
                    tuples[j].data.val.hi = in[j - tuples_size].val.hi;
                    break;

                case hu: tuples[j].data.type = hu;
                    tuples[j].data.val.hu = in[j - tuples_size].val.hu;
                    break;

                case li: tuples[j].data.type = li;
                    tuples[j].data.val.li = in[j - tuples_size].val.li;
                    break;

                case lu: tuples[j].data.type = lu;
                    tuples[j].data.val.lu = in[j - tuples_size].val.lu;
                    break;

                case lli: tuples[j].data.type = lli;
                    tuples[j].data.val.lli = in[j - tuples_size].val.lli;
                    break;

                case llu: tuples[j].data.type = llu;
                    tuples[j].data.val.llu = in[j - tuples_size].val.llu;
                    break;

                case f: tuples[j].data.type = f;
                    tuples[j].data.val.f = in[j - tuples_size].val.f;
                    break;

                case lf: tuples[j].data.type = lf;
                    tuples[j].data.val.lf = in[j - tuples_size].val.lf;
                    break;

                case Lf: tuples[j].data.type = Lf;
                    tuples[j].data.val.Lf = in[j - tuples_size].val.Lf;
                    break;

                case c: tuples[j].data.type = c;
                    tuples[j].data.val.c = in[j - tuples_size].val.c;
                    break;

                case s: tuples[j].data.type = s;
                    long int str_size = 0;
                    while(in[j - tuples_size].val.s[str_size] != '\0'){
                        str_size++;
                    }

                    str_ptrs = (char**)realloc(str_ptrs, (str_ptrs_size + 1) * sizeof(char*));
                    ptr_alloc_valid((void*) str_ptrs);
                    str_ptrs[str_ptrs_size] = (char*)malloc((str_size + 1) * sizeof(char));
                    ptr_alloc_valid((void*) str_ptrs[str_ptrs_size]);

                    for(int k = 0; k <= str_size; k++){
                        str_ptrs[str_ptrs_size][k] = in[j - tuples_size].val.s[k];
                    }

                    tuples[j].data.val.s = str_ptrs[str_ptrs_size];
                    str_ptrs_size++;
                    break;
            } // default case not necessary since .type specifies an enum format
        }

        tuples_size += dimS0; // update size
    }
}

/* Performs a delete operation for a tuple and its contents, with some memory management to free up memory.*/
void deleteTuple(char id[VAR_NAME_SIZE]){

    tuple_t* tuple_ptr = getTupleByID(id);

    if(tuple_ptr == NULL){
        printf("TUPLE_DELETE_ERROR: Tuple ID %s not found and thus deletion cannot be performed.\n", id);
    }
    else{
        int size = tuple_ptr->next; // since getTupleByID() returns a pointer to the first element, .next is equal to size
        long int copy_length = tuples_size - ((tuple_ptr + size) - tuples); // size of tuple array AFTER tuple to be deleted

        // freeing any character arrays which are pointed to by the tuple
        for(int j = 0; j < size; j++){
            if((tuple_ptr + j)->data.type == s){
                free((tuple_ptr + j)->data.val.s);
            }
        }

        /* Performing a shift in memory contents, where the block of memory for tuples array AFTER the tuple to be deleted
         * is shifted to the START of the tuple to be deleted. A realloc is then performed so as to decrement the block
         * size and free up memory for the OS.*/
        tuples_size -= size; // decrement tuples array size by size of deleted tuple
        memmove(tuple_ptr, tuple_ptr + size, copy_length * sizeof(tuple_t));
        tuples = realloc(tuples, tuples_size * sizeof(tuple_t));
        ptr_alloc_valid((void*) tuples);
    }
}

/* Takes a pointer to tuples and joins them (in the order of specification), assigning the specified ID (if available).
 * -> Note that, as a matter of fact the pointers need not point to the start of a tuple; one can choose to fetch a start
 * pointer, then shift it by x < size of tuple, to join with only a part of the tuple.
 * -> Developers should take care to ensure that the specified pointers are current and point to the desired tuples. With
 * this in mind, it is recommended to fetch pointers at compile time using getTupleByID().*/
void joinTuple(char id[VAR_NAME_SIZE], tuple_t* tuple_ptr_1, tuple_t* tuple_ptr_2){

    if(tuple_ptr_1 != NULL && tuple_ptr_2 != NULL){
        int dimS0 = tuple_ptr_1->next + tuple_ptr_2->next; // establish size of new tuple
        tagged_union joinedData[dimS0]; // define new tagged_union struct to format the joined data

        for (int j = 0; j < tuple_ptr_1->next; j++){
            joinedData[j] = tuple_ptr_1[j].data;
        } // populate tagged_union with data of ptr_1 first

        for (int j = tuple_ptr_1->next; j < dimS0; j++){
            joinedData[j] = tuple_ptr_2[j - tuple_ptr_1->next].data;
        } // then populate tagged_union with data of ptr_1 first

        createTuple(id, joinedData, dimS0); // create new tuple by explicit call to createTuple
    }
    else{
        printf("TUPLE_JOIN_ERROR: At least one of the tuple pointers specified is NULL.\n");
    }
}

/* Fetches by ID a pointer to the start of a tuple, if a match is found. Otherwise a NULL pointer is returned.
 * -> Developers should explicitly handle NULL pointer return, to implement their own logic depending on their particular
 * use case.
 * -> Return pointer should not be stored indefinitely, especially if to be used after creation, deletion, or joining of
 * tuples, as the tuple_t dynamic array may be moved around for memory management purposes.
 * */
tuple_t* getTupleByID(char search_id[VAR_NAME_SIZE]) {

    int dimS0_stack, match_count;
    int k = 0, dimS0_in = 0;
    tuple_t *match = NULL;

    while (search_id[k++] != '\0') {
        dimS0_in++;
    } // establishing inputted string length, which should at most be VAR_NAME_SIZE

    for (int j = 0; j < tuples_size; j++) { // search across all tuple_t type elements
        k = 0;
        dimS0_stack = 0;
        match_count = 0;

        while (tuples[j].id[k++] != '\0') {
            dimS0_stack++;
        } // establishing j^th string length, which should at most be VAR_NAME_SIZE

        if (dimS0_in != dimS0_stack) {
            j += tuples[j].next - 1;
            continue; // if lengths are not equal, then definitely strings are not equal; thus point to start of next tuple
        }
        else {
            for (k = 0; k < dimS0_in; k++) {
                if (search_id[k] == tuples[j].id[k])
                    match_count++;
            } // count number of matching individual characters between both strings, which can be at most dimS0_in

            if (match_count < dimS0_in) {
                j += tuples[j].next - 1;
                continue; // if < dimS0_in, strings do not match at all positions; thus point to start of next tuple
            } else {
                match = &tuples[j];
                break; // set match pointer to start of matched tuple, based on current dynamic array locations
            }
        }
    }

    return match; // return match, which is NULL unless a corresponding tuple is found
}

// Simple function to fetch a tuple ID based on a pointer to any element of that tuple.
char* getTupleID(tuple_t* tuple_ptr){

    if (tuple_ptr == NULL){
        return NULL;
    }
    else{
        return tuple_ptr->id;
    }
}

/* Simple function that prints out a formatted string with the contents of a tuple.
* -> Developers should take care to ensure that the specified pointers are current and point to the desired tuples. With
* this in mind, it is recommended to fetch pointers at compile time using getTupleByID().*/
void showTuple(tuple_t* tuple_ptr){

    if (tuple_ptr != NULL){

        /* The following series of switches simply defines formatting for each data type, depending on if the element is
         * in the first, middle, or last position relative to the tuple. */

        switch(tuple_ptr->data.type){
            case d: printf("(%d", tuple_ptr->data.val.d);
                break;

            case u: printf("(%u", tuple_ptr->data.val.u);
                break;

            case hi: printf("(%hi", tuple_ptr->data.val.hi);
                break;

            case hu: printf("(%hu", tuple_ptr->data.val.hu);
                break;

            case li: printf("(%li", tuple_ptr->data.val.li);
                break;

            case lu: printf("(%lu", tuple_ptr->data.val.lu);
                break;

            case lli: printf("(%lli", tuple_ptr->data.val.lli);
                break;

            case llu: printf("(%llu", tuple_ptr->data.val.llu);
                break;

            case f: printf("(%f", tuple_ptr->data.val.f);
                break;

            case lf: printf("(%lf", tuple_ptr->data.val.lf);
                break;

            case Lf: printf("(%Lf", tuple_ptr->data.val.Lf);
                break;

            case c: printf("('%c'", tuple_ptr->data.val.c);
                break;

            case s: printf("('%s'", tuple_ptr->data.val.s);
                break;
        } // default case not necessary since .type specifies an enum format

        int last = tuple_ptr->next - 1;

        if (last == 0){ // handling case that tuple has a single element
            printf(")");
        }
        else {
            printf(",");

            for (int j = 1; j < last; j++) {
                switch ((tuple_ptr + j)->data.type) {
                    case d:
                        printf(" %d,", (tuple_ptr + j)->data.val.d);
                        break;

                    case u:
                        printf(" %u,", (tuple_ptr + j)->data.val.u);
                        break;

                    case hi:
                        printf(" %hi,", (tuple_ptr + j)->data.val.hi);
                        break;

                    case hu:
                        printf(" %hu,", (tuple_ptr + j)->data.val.hu);
                        break;

                    case li:
                        printf(" %li,", (tuple_ptr + j)->data.val.li);
                        break;

                    case lu:
                        printf(" %lu,", (tuple_ptr + j)->data.val.lu);
                        break;

                    case lli:
                        printf(" %lli,", (tuple_ptr + j)->data.val.lli);
                        break;

                    case llu:
                        printf(" %llu,", (tuple_ptr + j)->data.val.llu);
                        break;

                    case f:
                        printf(" %f,", (tuple_ptr + j)->data.val.f);
                        break;

                    case lf:
                        printf(" %lf,", (tuple_ptr + j)->data.val.lf);
                        break;

                    case Lf:
                        printf(" %Lf,", (tuple_ptr + j)->data.val.Lf);
                        break;

                    case c:
                        printf(" '%c',", (tuple_ptr + j)->data.val.c);
                        break;

                    case s:
                        printf(" '%s',", (tuple_ptr + j)->data.val.s);
                        break;
                } // default case not necessary since .type specifies an enum format
            }

            switch ((tuple_ptr + last)->data.type) {
                case d:
                    printf(" %d)\n", (tuple_ptr + last)->data.val.d);
                    break;

                case u:
                    printf(" %u)\n", (tuple_ptr + last)->data.val.u);
                    break;

                case hi:
                    printf(" %hi)\n", (tuple_ptr + last)->data.val.hi);
                    break;

                case hu:
                    printf(" %hu)\n", (tuple_ptr + last)->data.val.hu);
                    break;

                case li:
                    printf(" %li)\n", (tuple_ptr + last)->data.val.li);
                    break;

                case lu:
                    printf(" %lu)\n", (tuple_ptr + last)->data.val.lu);
                    break;

                case lli:
                    printf(" %lli)\n", (tuple_ptr + last)->data.val.lli);
                    break;

                case llu:
                    printf(" %llu)\n", (tuple_ptr + last)->data.val.llu);
                    break;

                case f:
                    printf(" %f)\n", (tuple_ptr + last)->data.val.f);
                    break;

                case lf:
                    printf(" %lf)\n", (tuple_ptr + last)->data.val.lf);
                    break;

                case Lf:
                    printf(" %Lf)\n", (tuple_ptr + last)->data.val.Lf);
                    break;

                case c:
                    printf(" '%c')\n", (tuple_ptr + last)->data.val.c);
                    break;

                case s:
                    printf(" '%s')\n", (tuple_ptr + last)->data.val.s);
                    break;
            } // default case not necessary since .type specifies an enum format
        }
    }

    else{
        printf("TUPLE_SHOW_ERROR: Tuple pointer specified is NULL.\n");
    }
}

/* Simple function to carry out comparisons between tuples of that are compatible. Tuples are compatible if they have
 * the same size on the 0th dimension, and have pair-wise matching types. Integer 0 is returned if tuples match. If they
 * mismatch, a positive integer is returned if left is greater then right, otherwise a negative integer is returned. For
 * the mismatch case, the absolute of the return is equal to the position of first mismatch. If the return is INT_MAX,
 * an error has occured.*/
int cmpTuples(char id_1[VAR_NAME_SIZE], char id_2[VAR_NAME_SIZE]){

    tuple_t* tuple_ptr_1 = getTupleByID(id_1);
    tuple_t* tuple_ptr_2 = getTupleByID(id_2);

    int cmp_ret = INT_MAX; //set to INT_MAX, to be used as error code

    if(tuple_ptr_1 == NULL || tuple_ptr_2 == NULL){
        printf("TUPLE_COMPARE_ERROR: At least one of the passed identifiers is not associated with a tuple.\n");
    }
    else if(tuple_ptr_1->next != tuple_ptr_2->next){ // check for dimensionality match
        printf("TUPLE_COMPARE_ERROR: Tuple dimensions do not match and thus tuples are not compatible.\n");
    }
    else{
        int match = 1;

        for(int j = 0; j < tuple_ptr_1->next; j++){ // check for type mismatch
            if((tuple_ptr_1 + j)->data.type != (tuple_ptr_2 + j)->data.type){
                match = 0;
                break;
            }
        }

        if(match == 0){
            printf("TUPLE_COMPARE_ERROR: Tuple pair-wise data types do not match and thus tuples are not compatible.\n");
        }
        else{
            cmp_ret = 0;
            int ret_strcmp;

            // carry out data comparisons based on type, calling any necessary functions as needed
            for(int j = 0; j < tuple_ptr_1->next; j++){
                switch((tuple_ptr_1 + j)->data.type){
                    case d: cmp_ret = int_types_cmp((tuple_ptr_1 + j)->data.val.d, (tuple_ptr_2 + j)->data.val.d, j);
                        break;

                    case u: cmp_ret = uint_types_cmp((tuple_ptr_1 + j)->data.val.u, (tuple_ptr_2 + j)->data.val.u, j);
                        break;

                    case hi: cmp_ret = int_types_cmp((tuple_ptr_1 + j)->data.val.hi, (tuple_ptr_2 + j)->data.val.hi, j);
                        break;

                    case hu: cmp_ret = uint_types_cmp((tuple_ptr_1 + j)->data.val.hu, (tuple_ptr_2 + j)->data.val.hu, j);
                        break;

                    case li: cmp_ret = int_types_cmp((tuple_ptr_1 + j)->data.val.li, (tuple_ptr_2 + j)->data.val.li, j);
                        break;

                    case lu: cmp_ret = uint_types_cmp((tuple_ptr_1 + j)->data.val.lu, (tuple_ptr_2 + j)->data.val.lu, j);
                        break;

                    case lli: cmp_ret = int_types_cmp((tuple_ptr_1 + j)->data.val.lli, (tuple_ptr_2 + j)->data.val.lli, j);
                        break;

                    case llu: cmp_ret = uint_types_cmp((tuple_ptr_1 + j)->data.val.llu, (tuple_ptr_2 + j)->data.val.llu, j);
                        break;

                    case f: cmp_ret = float_types_cmp((tuple_ptr_1 + j)->data.val.f, (tuple_ptr_2 + j)->data.val.f, j);
                        break;

                    case lf: cmp_ret = float_types_cmp((tuple_ptr_1 + j)->data.val.lf, (tuple_ptr_2 + j)->data.val.lf, j);
                        break;

                    case Lf: cmp_ret = float_types_cmp((tuple_ptr_1 + j)->data.val.Lf, (tuple_ptr_2 + j)->data.val.Lf, j);
                        break;

                    case c: cmp_ret = int_types_cmp((tuple_ptr_1 + j)->data.val.c, (tuple_ptr_2 + j)->data.val.c, j);
                        break;

                    case s:
                        ret_strcmp = strcmp((tuple_ptr_1 + j)->data.val.s, (tuple_ptr_2 + j)->data.val.s);

                        if(ret_strcmp > 0){
                            cmp_ret = j+1;
                        }
                        else if(ret_strcmp < 0){
                            cmp_ret = -j-1;
                        }
                        else{
                            cmp_ret = 0;
                        }
                        break;
                }

                if(cmp_ret != 0){
                    break;
                }
            }
        }
    }

    return cmp_ret;
}

// Function to create a formatted file output with the contents of all tuples, and save the file to the specified path
void saveAllTuples(char path[]){

    FILE *fp;
    fp = fopen(path, "w"); // specify write only mode

    for(int j = 0; j < tuples_size; j++){

        fprintf(fp, "%s;%d;", tuples[j].id, tuples[j].next);

        switch(tuples[j].data.type){ // associate a numerical value to each data type
            case d: fprintf(fp, "%d;%d;\n", 1, tuples[j].data.val.d);
                break;

            case u: fprintf(fp, "%d;%u;\n", 2, tuples[j].data.val.u);
                break;

            case hi: fprintf(fp, "%d;%hi;\n", 3, tuples[j].data.val.hi);
                break;

            case hu: fprintf(fp, "%d;%hu;\n", 4, tuples[j].data.val.hu);
                break;

            case li: fprintf(fp, "%d;%li;\n", 5, tuples[j].data.val.li);
                break;

            case lu: fprintf(fp, "%d;%lu;\n", 6, tuples[j].data.val.lu);
                break;

            case lli: fprintf(fp, "%d;%lli;\n", 7, tuples[j].data.val.lli);
                break;

            case llu: fprintf(fp, "%d;%llu;\n", 8, tuples[j].data.val.llu);
                break;

            case f: fprintf(fp, "%d;%f;\n", 9, tuples[j].data.val.f);
                break;

            case lf: fprintf(fp, "%d;%lf;\n", 10, tuples[j].data.val.lf);
                break;

            case Lf: fprintf(fp, "%d;%Lf;\n", 11, tuples[j].data.val.Lf);
                break;

            case c: fprintf(fp, "%d;%c;\n", 12, tuples[j].data.val.c);
                break;

            case s: fprintf(fp, "%d;%s;\n", 13, tuples[j].data.val.s);
                break;
        } // default case not necessary since .type specifies an enum format
    }

    fclose(fp);
}

/* Function to read tuple data from a file at the specified path, which must be formatted in the manner specified by
 * saveAllTuples. Otherwise, undefined behaviour is almost definitely guaranteed to occur. Any manual manipulation to
 * said files may also result in undefined behaviour. Any data type errors will lead to program exit, to restrict
 * undefined behaviour as much as possible: a graceful close is carried out, where tuples are saved to the directory.*/
void loadAllTuples(char path[]){

    int new_line_size = 2 * VAR_NAME_SIZE, tuple_size = 0, tuple_size_temp = 0, current_size = 0, load_str_ptrs_size = 0;

    char tuple_id[VAR_NAME_SIZE];
    int format;
    int need_not_realloc;

    char *new_line;
    char *data;
    char *conversion_ptr;
    char **load_str_ptrs;
    tagged_union *read_store;

    // to store fetched line from file; will be expanded as necessary
    new_line = (char*)malloc(new_line_size * sizeof(char));
    ptr_alloc_valid((void*)new_line);

    // to store the data part of the fetched line; will be expanded as necessary
    data = (char*)malloc(new_line_size * sizeof(char));
    ptr_alloc_valid((void*)data);

    // to store the formatted data read from the file, until a createTuple() call is made
    read_store = (tagged_union*)malloc(0);
    ptr_alloc_valid((void*)read_store);

    FILE *fp;
    fp = fopen(path, "r"); // specify read mode only

    if(fp != NULL){
        long int fp_pos;
        long int fp_pos_state = ftell(fp);

        while(!feof(fp)){ // loop until end of file reached
            need_not_realloc = 0;
            load_str_ptrs_size = 0;

            load_str_ptrs = (char**)malloc(0);
            ptr_alloc_valid((void*)load_str_ptrs);

            fgets(new_line, new_line_size, fp); // fetch line
            fp_pos = ftell(fp);

            for(int j = new_line_size - 1; j >= 0; j--){ // to check of \n occurs in fetched line

                if (new_line[j] == '\n'){ // if \n found in input stream, need_not_realloc = TRUE
                    fseek(fp, fp_pos_state - fp_pos, SEEK_CUR); // shift fp to start of line
                    fscanf(fp,"%[^;];%d;%d;%[^;];\n", tuple_id, &tuple_size_temp, &format, data); // fetch formatted input
                    fp_pos = ftell(fp);
                    fp_pos_state = ftell(fp);

                    if(tuple_size_temp >= current_size){ // if start of new tuple found
                        tuple_size = tuple_size_temp;
                        read_store = (tagged_union*)realloc(read_store, tuple_size * sizeof(tagged_union));
                        ptr_alloc_valid((void*)read_store);
                    }

                    current_size = tuple_size_temp;
                    int k = tuple_size - current_size; // indirectly specifies an index over the current tuple elements

                    // population of data with corresponding type; perform graceful close if data/type errors occur
                    long fetch_long_int;
                    long long fetch_long_long_int;

                    switch(format){
                        case 1: read_store[k].type = d;
                            fetch_long_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_long_int <= INT_MAX && fetch_long_int >= INT_MIN && errno != ERANGE){
                                read_store[k].val.d = fetch_long_int;
                            } // check to see if return long int is in fact in int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 2: read_store[k].type = u;
                            fetch_long_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_long_int <= UINT_MAX && fetch_long_int >= 0 && errno != ERANGE){
                                read_store[k].val.u = fetch_long_int;
                            } // check to see if return long int is in fact in unsigned int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unsigned int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 3: read_store[k].type = hi;
                            fetch_long_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_long_int <= SHRT_MAX && fetch_long_int >= SHRT_MIN && errno != ERANGE){
                                read_store[k].val.hi = fetch_long_int;
                            } // check to see if return long int is in fact in short int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(short int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 4: read_store[k].type = hu;
                            fetch_long_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_long_int <= USHRT_MAX && fetch_long_int >= 0 && errno != ERANGE){
                                read_store[k].val.hu = fetch_long_int;
                            } // check to see if return long int is in fact in unsigned short int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unsigned short int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 5: read_store[k].type = li;
                            fetch_long_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_long_int <= LONG_MAX && fetch_long_int >= LONG_MIN && errno != ERANGE){
                                read_store[k].val.li = fetch_long_int;
                            } // check to see if return long int is in fact in long int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(long int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 6: read_store[k].type = lu;
                            fetch_long_int = strtol(data, &conversion_ptr, 10);
                            if (fetch_long_int <= ULONG_MAX && fetch_long_int >= 0 && errno != ERANGE){
                                read_store[k].val.lu = fetch_long_int;
                            } // check to see if return long int is in fact in unsigned long int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unsigned long int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 7: read_store[k].type = lli;
                            fetch_long_long_int = strtoll(data, &conversion_ptr, 10);
                            if (fetch_long_long_int <= LONG_LONG_MAX && fetch_long_long_int >= LONG_LONG_MIN && errno != ERANGE){
                                read_store[k].val.lli = fetch_long_long_int;
                            } // check to see if return long long int is in fact in long long int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(long long int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 8: read_store[k].type = llu;
                            fetch_long_long_int = strtoll(data, &conversion_ptr, 10);
                            if (fetch_long_long_int <= ULONG_LONG_MAX && fetch_long_long_int >= 0 && errno != ERANGE){
                                read_store[k].val.llu = fetch_long_long_int;
                            } // check to see if return long long int is in fact in unsigned long long int range
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unsigned long long int range out of bounds).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 9: read_store[k].type = f;
                            float fetch_float = strtof(data, &conversion_ptr);
                            if(errno != ERANGE){ // check if strtof() had any errors
                                read_store[k].val.f = fetch_float;
                            }
                            else{
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unable to cast float from input string).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 10: read_store[k].type = lf;
                            double fetch_double = strtod(data, &conversion_ptr);
                            if(errno != ERANGE){ // check if strtod() had any errors
                                read_store[k].val.lf = fetch_double;
                            }
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unable to cast double from input string).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 11: read_store[k].type = Lf;
                            long double fetch_long_double = strtold(data, &conversion_ptr);
                            if(errno != ERANGE){ // check if strtold() had any errors
                                read_store[k].val.Lf = fetch_long_double;
                            }
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(unable to cast long double from input string).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 12: read_store[k].type = c;
                            if(data[1] == '\0'){
                                read_store[k].val.c = data[0];
                            }
                            else{ // graceful close
                                printf("TUPLE_LOAD_ERROR: Incompatibility in fetched data and corresponding type "
                                       "(char array detected when one char expected).\n");
                                saveAllTuples("tuple_load_error_SAVE.txt");
                                exit(EXIT_FAILURE);
                            }
                            break;

                        case 13:
                            read_store[k].type = s;
                            load_str_ptrs = (char**)realloc(load_str_ptrs, (load_str_ptrs_size + 1) * sizeof(char*));
                            ptr_alloc_valid((void*) load_str_ptrs);

                            int size = 0;
                            while(data[size] != '\0'){
                                size++;
                            }

                            load_str_ptrs[load_str_ptrs_size] = (char*)malloc((size + 1) * sizeof(char));
                            ptr_alloc_valid((void*) load_str_ptrs[load_str_ptrs_size]);

                            for(int n = 0; n <= size; n++){
                                load_str_ptrs[load_str_ptrs_size][n] = data[n];
                            }

                            read_store[k].val.s = load_str_ptrs[load_str_ptrs_size];
                            load_str_ptrs_size++;
                            break;

                        default: printf("TUPLE_LOAD_ERROR: Specified type with numeric identifier %d not recognised by library.\n", format);
                            saveAllTuples("tuple_load_error_SAVE.txt");
                            exit(EXIT_FAILURE); // graceful close
                    }

                    if(current_size == 1){ // if reached end of tuple, call createTuple and move on to the next tuple
                        createTuple(tuple_id, read_store, tuple_size);

                        for(int n = 0; n < load_str_ptrs_size; n++){
                            free(load_str_ptrs[n]);
                        }

                        free(load_str_ptrs);
                    }

                    need_not_realloc = 1;
                    break;
                }
            }

            if (need_not_realloc == 0){ // occurs only if \n not found in inputted line
                fseek(fp, fp_pos_state - fp_pos, SEEK_CUR); // shift fp to start of line

                new_line_size *= 2; // double line size and then expand new_line and data using realloc
                new_line = (char*)realloc(new_line, new_line_size * sizeof(char));
                ptr_alloc_valid((void*)new_line);
                data = (char*)realloc(data, new_line_size * sizeof(char));
                ptr_alloc_valid((void*)data);
            }
        }
    }
    else { // in case fp unable to open i.e. NULL, handle scenario
        printf("TUPLE_LOAD_ERROR: Unable to open file at specified location.\n");
    }

    fclose(fp);

    free(new_line);
    free(data);
    free(read_store);
}

// function used for integer comparisons in tuples
int int_types_cmp(long long int num1, long long int num2, int shift){

    int cmp_ret = 0;

    if(num1 > num2){
        cmp_ret = shift+1;
    }
    else if(num1 < num2){
        cmp_ret = -shift-1;
    }

    return cmp_ret;
}

// function used for unsigned integer comparisons in tuples
int uint_types_cmp(unsigned long long int num1, unsigned long long int num2, int shift){

    int cmp_ret = 0;

    if(num1 > num2){
        cmp_ret = shift+1;
    }
    else if(num1 < num2){
        cmp_ret = -shift-1;
    }

    return cmp_ret;
}

// function used for float comparisons in tuples
int float_types_cmp(long double num1, long double num2, int shift){

    int cmp_ret = 0;

    if(num1 > num2){
        cmp_ret = shift+1;
    }
    else if(num1 < num2){
        cmp_ret = -shift-1;
    }

    return cmp_ret;
}

/* Simple function that checks if valid pointer is returned after a malloc or realloc operation; if NULL, EXIT_FAILURE
 * is instructed so as to eliminate potential issues in memory management, prevent core dumps, etc...*/
void ptr_alloc_valid(void *ptr) {

    if (ptr == NULL) {
        printf("TUPLE_ALLOCATION_FAILURE: Memory allocation failed for array during either initial malloc or subsequent realloc.\n");
        saveAllTuples("tuple_load_error_SAVE.txt");
        exit(EXIT_FAILURE); // graceful close
    }
}