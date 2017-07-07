//
//  main.c
//  variant
//
//  Created by Jaseem V V on 04/04/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/* exception */
static jmp_buf excep;

/* error codes */
const int err_tag = 100;

/* variant tags */
enum SzTag { short_t, ptr_t };

/* A type with a tagged union */
struct Data {
    enum SzTag tag;
    union {
        short short_val;
        int *ptr_val;
    };
};

typedef struct Data Data;

/* Set the ptr_val in the given struct */
Data *set_data_ptr_val(Data *data) {
    data->tag = ptr_t;
    data->ptr_val = malloc(sizeof(int));
    *(data->ptr_val) = 5;
    return data;
}

/* Set the short_val in the given struct */
Data *set_data_short_val(Data *data) {
    data->tag = short_t;
    data->short_val = 3;
    return data;
}

/* Process the struct. But which value in the union is set? There is no way of knowing it. */
void process(Data *data) {
    printf("tag enum: %d\n", data->tag);
    printf("short_val: %d\n", data->short_val);
    printf("ptr_val: %d\n\n", *(data->ptr_val));  /* can segfault */
}

/* Using a tagged union and by checking the tag, we can identify which value point in the union is set. */
void process_variant(Data *data) {
    if (data->tag == short_t) {
        printf("variant tag: short_t, val: %d\n", data->short_val);
    } else if (data->tag == ptr_t) {
        printf("variant tag: ptr_t, val: %d\n", *(data->ptr_val));
    } else longjmp(excep, err_tag);  /* handle unknown tags */
}

int main(int argc, const char **argv) {
    Data *data = NULL;
    switch (setjmp(excep)) {
        /* default, success case */
        case 0:
            data = malloc(sizeof(Data));
            
            /* set ptr_val */
            data = set_data_ptr_val(data);
            process(data);  /* short_val is garbage */
            free(data->ptr_val);
            // data->ptr_val = NULL;  /* commenting to avoid segfault, as we don't know which member of union is set */
            
            /* set short_val */
            data = set_data_short_val(data);
            process(data);  /* ptr_val is garbage */
            
            /* set ptr_val */
            data = set_data_ptr_val(data);
            process_variant(data);  /* checks the tag, no garbage */
            free(data->ptr_val);
            data->ptr_val = NULL;
            
            /* set short_val */
            data = set_data_short_val(data);
            data->tag = -13;  /* set an unknown tag => open variant */
            process_variant(data);
            break;
        /* exception cases */
        case err_tag:
            printf("\nError: tag exception: %d\n", err_tag);
            break;
    }
    if (data != NULL) {
        free(data);
        data = NULL;
    }
    return 0;
}
