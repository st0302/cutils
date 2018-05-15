#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "st_hash.h"

#define MAX_SIZE  60000
#define ST_INFO  __FILE__,__LINE__

#define PRINT_KEY(s, x) sprintf(s, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", \
                                (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5], (x)[6], (x)[7], \
                                (x)[8], (x)[9], (x)[10], (x)[11], (x)[12], (x)[13], (x)[14], (x)[15])

typedef struct tagNode{
    char key[33];
}NODE;

/***********************************************************
 *在main中测试argv[1]的三个hash值：
 *  ./hash  "arr\units.dat"
 *  ./hash  "unit\neutral\acritter.grp"
 ***********************************************************/
int main( int argc, char **argv ){
    unsigned long table_size = 0;
    STHASH_TABLE *table = NULL;
    unsigned long i = 0,j=0;
    NODE *pNode = NULL;

    fprintf(stderr, "[%s:%d]line dbg\n", ST_INFO);
    table_size = stHashCalcTableSize(MAX_SIZE);

    fprintf(stderr, "[%s:%d]line dbg, table size=%lu\n", ST_INFO, table_size);

    table = stHashCreateTable(table_size);
    if(table == NULL){
        fprintf(stderr, "stHashCreateTable error\n");
        return -1;
    }

    fprintf(stderr, "[%s:%d]line dbg\n", ST_INFO);

    pNode = (NODE *)malloc(100000*sizeof(NODE));
    if(!pNode){
        fprintf(stderr, "malloc error[%s]\n", strerror(errno));
        goto end__;
    }

    memset(pNode, 0, 100000*sizeof(NODE));

    for(i=0; i<6; i++){
        unsigned char ch[16]={0,};
        for(j=0; j<16; j++){
            ch[j] = rand() & 0xff;
        }
        PRINT_KEY(pNode[i].key, ch);
        fprintf(stderr, "insert key=[%s], value=[%s]\n", pNode[i].key, pNode[i].key);
        stHashInsert(table, table_size, pNode[i].key, pNode[i].key);
    }

    fprintf(stderr, "[%s:%d]line dbg\n", ST_INFO);

    stHashDelValueFromKey(table, table_size, pNode[i].key);
    for(i=0; i<MAX_SIZE; i++){
        char *value = (char *)stHashGetValueFromKey(table, table_size, pNode[i].key);
        if(value){
            fprintf(stderr, "key=[%s], value=[%s]\n", pNode[i].key, value);
        }
    }

    stHashClearTable(table);

end__:
    if(pNode){
        free(pNode);
    }
    return 0;
}
