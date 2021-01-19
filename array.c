#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "array.h"

struct Performance *newPerformance(){
    /* dynamically allocating memory for the new performance structure */
    struct Performance *newPerf = (struct Performance*)malloc(sizeof(struct Performance));
    if (newPerf == NULL){
        printf("Error: Memory could not be allocated to the new Performance Struct\n");
        exit(1);
    }
    /* setting all struct values to 0 */
    newPerf->reads = 0;
    newPerf->writes = 0;
    newPerf->mallocs = 0;
    newPerf->frees = 0;

    return(newPerf);
}

struct Array *newArray(struct Performance *performance, unsigned int width, unsigned int capacity){
    /* allocating memory for the new array */
    struct Array *newAr = (struct Array*)malloc(sizeof(struct Array));
    if (newAr == NULL){
        printf("Error: Memory could not be allocated to the new Array Struct\n");
        exit(2);
    }
    /* setting the structure variables to the given values */
    newAr->capacity = capacity;
    newAr->width = width;
    newAr->nel = 0;
    newAr->data = (void *)malloc(newAr->width * newAr->capacity);
    if (newAr->data == NULL){
        printf("Error: Memory could not be allocated to the new Array data\n");
        exit(3);
    }
    /* increasing the number of mallocs performed */
    performance->mallocs++;

    return(newAr);
}

void readItem(struct Performance *performance, struct Array *array, unsigned int index, void *dest){
    if (index >= array->nel){
        printf("Error: The index number is too large\n");
        exit(4);
    }
    unsigned char *ptr = array->data;
    /* copying the specified source to the destination */
    memcpy(dest, ptr + (index * array->width), array->width);
    /* increasing the number of reads performed */
    performance->reads++;
}

void writeItem( struct Performance *performance, struct Array *array, unsigned int index, void *src){
    if (index > array->nel || index >= array->capacity){
        printf("Error: The Index number is too large\n");
        exit(5);
    }
    unsigned char *ptr = array->data;
    /* copying the specified source to the destination */
    memcpy(ptr + (index * array->width), src, array->width);
    
    if (index == array->nel){
        array->nel++;
    }
    /* increasing the number of writes performed */
    performance->writes++;
}

void contract(struct Performance *performance, struct Array *array){
    if (array->nel == 0){
        printf("Error: Array at nel is equal to 0\n");
        exit(6);
    } else {
        /* removing the last index in array */
        array->nel--;
    }
}

void freeArray(struct Performance *performance, struct Array *array){
    /* freeing malloced pointers/structures */
    free(array->data);
    free(array);
    performance->frees++;
}

void appendItem( struct Performance *performance, struct Array *array, void *src){
    /* using writeItem to add a value at the end of the array index (array->nel) */
    writeItem(performance, array, array->nel, src);
}

void insertItem(struct Performance *performance, struct Array *array, unsigned int index, void *src){
    void *temp1 = malloc(array->width);
    void *temp2 = malloc(array->width);
    int i;
    /* reading the initial to-be shifted value into temp1*/
    readItem(performance, array, index, temp1);
    /* writing the item that has to be inserted into the index */
    writeItem(performance, array, index, src);

    /* shifting the rest of the values */
    for(i = 1; i <= array->nel - index - 1; i++){
        readItem(performance, array, index + i, temp2);
        writeItem(performance, array, index + i, temp1);
        temp1 = temp2;
        temp2 = malloc(array->width);
    }
    /* writing final value */
    writeItem(performance, array, array->nel, temp1);
    /* freeing temp pointers */
    free(temp1);
    free(temp2);
}

void prependItem(struct Performance *performance, struct Array *array, void *src){
    /* using insertItem to add an item to the beginning of the array */
    insertItem(performance, array, 0, src);
}

void deleteItem(struct Performance *performance, struct Array *array, unsigned int index){
    void *temp1 = malloc(array->width);
    void *temp2 = malloc(array->width);
    int i = array->nel - 1;

    /* making temp1 equal to the last index value of array */
    readItem(performance, array, i, temp1);
    /* making temp2 equal to the second last index value of array */
    readItem(performance, array, i-1, temp2);

    /* shifting the rest of the values from the last index to the deleted index */
    for(i = array->nel - 1; i > index; i--){
        writeItem(performance, array, i-1, temp1);
        temp1 = temp2;
        temp2 = malloc(array->width);
        readItem(performance, array, i-2, temp2);
    }
    /* removing the last index of the array */
    contract(performance, array);
    /* freeing malloced temps */
    free(temp1);
    free(temp2);
}

int findItem(struct Performance *performance, struct Array *array, int (*compar)(const void *, const void *), void *target){
    int i;
    void *temp = malloc(array->width);
    /* determining which value is equal to target */
    for (i = 0; i < array->nel; i++){
        readItem(performance, array, i, temp);
        if (compar(target, temp) == 0){
            return(i);
        }
    }
    /* freeing malloced temp */
    free(temp);
    return(-1);
}

int searchItem( struct Performance *performance, struct Array *array, int (*compar)(const void *, const void *), void *target ){
    int high = array->width - 1;
    int low = 0;
    int mid = 0;
    void *temp = malloc(array->width);    

    /* using binary shifting method from zybooks */
    /* it doesnt work completely :( */
    while(high >= low){
        mid = (high + low)/2; 
        readItem(performance, array, mid, temp);
        if (compar(temp, target) == 0){
            return(mid);
        } else if (compar(temp, target) < 0){
            low = mid + 1;
        } else if (compar(temp, target) > 0) {
            high = mid - 1;
        } else {
            return(mid);
        }
        temp = malloc(array->width); 
    }
    /* freeing malloced temp */
    free(temp);
    return(-1);
}

