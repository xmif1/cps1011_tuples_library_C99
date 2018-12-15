#include <stdio.h>
#include "tuples_t.h"

int main(){

    tagged_union tuple_1[4] = {{.type = d, .val.d = 10},
                               {.type = c, .val.c = 'X'},
                               {.type = f, .val.f = 3.14159},
                               {.type = s, .val.s = "String 1"}};
    tagged_union tuple_2 = {.type = d, .val.d = 10};

    printf("-----\nTesting creation of tuples of different sizes and data types, and displaying them to user: \n\n");

    createTuple("mixedTypes", tuple_1, 4);
    createTuple("singleElement", &tuple_2, 1);

    showTuple(getTupleByID("mixedTypes"));
    showTuple(getTupleByID("singleElement"));

    printf("\n-----\nTesting joining of tuples in different orders: \n\n");

    joinTuple("join1", getTupleByID("mixedTypes"), getTupleByID("singleElement"));
    joinTuple("join2", getTupleByID("singleElement"), getTupleByID("mixedTypes"));

    showTuple(getTupleByID("join1"));
    showTuple(getTupleByID("join2"));

    printf("\n-----\nTesting deletion of tuples, which include string data, and that proper errors are reported when an attempt"
           " is made to access a deleted tuple: \n\n");

    deleteTuple("mixedTypes");
    showTuple(getTupleByID("mixedTypes"));
    joinTuple("join1", getTupleByID("mixedTypes"), getTupleByID("singleElement"));
    deleteTuple("mixedTuples");
    cmpTuples("mixedTuple", "singleElement");

    printf("\n-----\nTesting that deleted tuple is not saved to disk and then reloaded: \n\n");

    saveAllTuples("test_save.txt");
    deleteTuple("singleElement");
    deleteTuple("join1");
    deleteTuple("join2");
    loadAllTuples("test_save.txt");
    showTuple(getTupleByID("mixedTypes"));

    printf("\n-----\nTesting that if loaded tuples include duplicates, the duplicates are not loaded but the remaining are: \n\n");

    deleteTuple("join2");
    loadAllTuples("test_save.txt");
    showTuple(getTupleByID("join2"));

    printf("\n-----\nTesting that correct error is reported if file to load does not exist: \n\n");

    loadAllTuples("does_not_exist.txt");

    printf("\n-----\nTesting tuple comparison operations: \n\n");

    tagged_union tuple_3[4] = {{.type = d, .val.d = 10},
                               {.type = c, .val.c = 'X'},
                               {.type = f, .val.f = 3.14159},
                               {.type = s, .val.s = "String 2"}};
    tagged_union tuple_4[4] = {{.type = d, .val.d = 10},
                               {.type = c, .val.c = 'X'},
                               {.type = f, .val.f = 3.14158},
                               {.type = s, .val.s = "String 1"}};
    tagged_union tuple_5[4] = {{.type = d, .val.d = 10},
                               {.type = c, .val.c = 'X'},
                               {.type = lf, .val.f = 3.14158},
                               {.type = s, .val.s = "String 1"}};

    createTuple("compare_1", tuple_1, 4);
    createTuple("compare_2", tuple_3, 4);
    createTuple("compare_3", tuple_4, 4);
    createTuple("compare_4", tuple_5, 4);

    printf("String Compare: %d\n", cmpTuples("compare_1", "compare_2"));
    printf("Numeric Compare: %d\n", cmpTuples("compare_1", "compare_3"));
    printf("Incompatible Compare (dimensions): ");
    cmpTuples("compare_1", "singleElement");
    printf("Incompatible Compare (types): ");
    cmpTuples("compare_1", "compare_4");

    printf("\n-----\nTesting that data is not loaded when file to load contains garbled data, and that graceful close is carried out: \n\n");

    FILE *fp;
    fp = fopen("garbled.txt", "w");
    fprintf(fp, "assdfd;1;dggdfsgsgddfasfdffsfdfdsfsf"); //including semicolons intentionally to 'trick' program
    fclose(fp);

    loadAllTuples("garbled.txt");

    return 0;
}