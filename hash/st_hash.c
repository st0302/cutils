#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "st_hash.h"

#define ST_INFO  __FILE__,__LINE__

static unsigned long prime_array[] = {
    10949,          /* 0 */
    21911,          /* 1 */
    43853,          /* 2 */
    87719,          /* 3 */
    175447,         /* 4 */
    350899,         /* 5 */
    701819,         /* 6 */
    1403641         /* 7 */
};               
#define PRIME_ARRAY_SIZE  (8)
#define HASH_OFFSET  0
#define HASH_A       1
#define HASH_B       2

/*crytTable[]里面保存的是HashString函数里面将会用到的一些数据，在prepareCryptTable
 *函数里面初始化*/
static unsigned long cryptTable[0x500];
static int prepared = 0;

/***********************************************************
 *以下的函数生成一个长度为0x500（合10进制数：1280）的cryptTable[0x500]
 *
 *
 ***********************************************************/
static void prepareCryptTable(){ 
    unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;
    for( index1 = 0; index1 < 0x100; index1++ ){ 
        for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 ){ 
            unsigned long temp1, temp2;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp1 = (seed & 0xFFFF) << 0x10;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            temp2 = (seed & 0xFFFF);
            cryptTable[index2] = ( temp1 | temp2 ); 
        } 
    } 
}

/***********************************************************
 *以下函数计算key 字符串的hash值，其中dwHashType 为hash的类型，
 *在下面GetHashTablePos函数里面调用本函数，其可以取的值为0、1、2；该函数
 *返回key 字符串的hash值；
 ***********************************************************/
static unsigned long stHashString( const char *key, unsigned long dwHashType ){ 
    unsigned char *p  = (unsigned char *)key;
    unsigned long seed1 = 0x7FED7FED;
    unsigned long seed2 = 0xEEEEEEEE;
    int ch;
    while( *p != 0 ){ 
        ch = toupper(*p++);
        seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
    }
    return seed1; 
}

/* calc table size, if max item size =max_size */
unsigned long stHashCalcTableSize(unsigned long max_size){
    int i = 0;
    unsigned long need_size = max_size * 4;
    for(i=0; i<PRIME_ARRAY_SIZE; i++){
        if(prime_array[i] > need_size)break;
    }

    if(i == PRIME_ARRAY_SIZE)return prime_array[PRIME_ARRAY_SIZE-1];
    else return prime_array[i];
}


STHASH_TABLE * stHashCreateTable(unsigned long table_size){
    STHASH_TABLE *ret_ptr = NULL;

    if(prepared == 0){
        prepareCryptTable();
        prepared = 1;
    }

    ret_ptr = (STHASH_TABLE *)malloc(table_size*sizeof(STHASH_TABLE));
    if(ret_ptr == NULL){
        fprintf(stderr, "[%s:%d]malloc(size=%lu) error:[%s]\n", ST_INFO, 
                table_size, strerror(errno));
    }

    memset(ret_ptr, 0, table_size*sizeof(STHASH_TABLE));
    return ret_ptr;
}

/* insert a key and context to a hash table, value should not be NULL*/
int stHashInsert(STHASH_TABLE *pTable, unsigned long table_size, const char *key, void *value){
    int ret = 0;
    unsigned long hash = 0;
    unsigned long hash_a = 0;
    unsigned long hash_b = 0;
    unsigned long pos = 0;
    unsigned long times = 0;
    
    if(key == NULL || value == NULL){
        fprintf(stderr, "[%s:%d]input key(%p) or value(%p) is NULL\n", ST_INFO, key, value);
        ret = -1;goto end__;
    }

    hash = stHashString( key, HASH_OFFSET );
    hash_a = stHashString( key, HASH_A );
    hash_b = stHashString( key, HASH_B );
    pos = hash % table_size;
    times = table_size;

    while ( pTable[pos].value && times--){
        if(pTable[pos].hash_a == hash_a && 
           pTable[pos].hash_b == hash_b){
//            fprintf(stderr, "[%s:%d]key:%s have been inserted before\n", ST_INFO, key);
            ret = -1;
            break;
        }
        pos = (pos + 1) % table_size;
//        usleep(100);
    }

    /* if(table_size-times>100) */
    /* { */
    /*     fprintf(stderr, "[%s:%d]times:%lu\n", ST_INFO, table_size-times); */
    /* } */

    /* found empty node */
    if(pTable[pos].value == NULL && ret == 0){
        pTable[pos].hash_a = hash_a;
        pTable[pos].hash_b = hash_b;
        pTable[pos].value = value;
    }else{
        ret = -1;
    }

end__:
    return ret;
}

/* get a value from a hash table, if not value, return NULL  */
void *stHashGetValueFromKey(STHASH_TABLE *pTable, unsigned long table_size, const char *key){
    void *ret_ptr = NULL;
    unsigned long hash = 0;
    unsigned long hash_a = 0;
    unsigned long hash_b = 0;
    unsigned long pos = 0;
    unsigned long times = 0;
    
    if(key == NULL){
        fprintf(stderr, "[%s:%d]input key is NULL\n", ST_INFO);
        goto end__;
    }

    hash = stHashString( key, HASH_OFFSET );
    hash_a = stHashString( key, HASH_A );
    hash_b = stHashString( key, HASH_B );
    pos = hash % table_size;
    times = table_size;

    while( pTable[pos].value && times--){
        if ( pTable[pos].hash_a == hash_a &&
             pTable[pos].hash_b == hash_b ){
            ret_ptr = pTable[pos].value;
            break;
        }else{
            pos = (pos + 1) % table_size;
        }
    }

end__:
    return ret_ptr;
}

/* get a value from a hash table, if not value, return NULL  */
void *stHashGetValueFromIndex(STHASH_TABLE *pTable, unsigned long table_size, unsigned long index){
    void *ret_ptr = NULL;
    
    if(index >= table_size){
        fprintf(stderr, "[%s:%d]input index >= table size\n", ST_INFO);
        goto end__;
    }
    ret_ptr = pTable[index].value;
end__:
    return ret_ptr;
}

/* del a value from a hash table  */
int stHashDelValueFromKey(STHASH_TABLE *pTable, unsigned long table_size, const char *key){
    int ret = -1;
    unsigned long hash = 0;
    unsigned long hash_a = 0;
    unsigned long hash_b = 0;
    unsigned long pos = 0;
    unsigned long times = 0;
    
    if(key == NULL){
        fprintf(stderr, "[%s:%d]input key is NULL\n", ST_INFO);
        goto end__;
    }

    hash = stHashString( key, HASH_OFFSET );
    hash_a = stHashString( key, HASH_A );
    hash_b = stHashString( key, HASH_B );
    pos = hash % table_size;
    times = table_size;

    while( pTable[pos].value && times--){
        if ( pTable[pos].hash_a == hash_a &&
             pTable[pos].hash_b == hash_b ){
            pTable[pos].value = NULL;
            ret = 0;
            break;
        }else{
            pos = (pos + 1) % table_size;
        }
    }

end__:
    return ret;
}

/* get a value from a hash table, if not value, return NULL  */
int stHashDelValueFromIndex(STHASH_TABLE *pTable, unsigned long table_size, unsigned long index){
    int ret = 0;
    
    if(index >= table_size){
        fprintf(stderr, "[%s:%d]input index >= table size\n", ST_INFO);
        ret = -1;goto end__;
    }
    pTable[index].value = NULL;
end__:
    return ret;
}

/* clear a hash table */
void stHashClearTable(STHASH_TABLE *pTable){
    if(pTable){
        free(pTable);
    }
}

