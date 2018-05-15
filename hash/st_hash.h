#ifndef INCLUDED_ST_HASH_H_
#define INCLUDED_ST_HASH_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    unsigned long hash_a;
    unsigned long hash_b;
    void *value;
} STHASH_TABLE;

/* calc table size, max_size must <= 1403641*/
unsigned long stHashCalcTableSize(unsigned long max_size);

/*  create a hash table (use table_size calc from stHashCalcTableSize)*/
STHASH_TABLE * stHashCreateTable(unsigned long table_size);

/* insert a key and context to a hash table, value should not be NULL*/
int stHashInsert(STHASH_TABLE *pTable, unsigned long table_size, const char *key, void *value);

/* get a value from a hash table, if not have a value, return NULL  */
void *stHashGetValueFromKey(STHASH_TABLE *pTable, unsigned long table_size, const char *key);

/* get a value from a hash table, if not have a value, return NULL  */
void *stHashGetValueFromIndex(STHASH_TABLE *pTable, unsigned long table_size, unsigned long index);

/* del a value from a hash table  */
int stHashDelValueFromKey(STHASH_TABLE *pTable, unsigned long table_size, const char *key);
int stHashDelValueFromIndex(STHASH_TABLE *pTable, unsigned long table_size, unsigned long index);

/* clear a hash table */
void stHashClearTable(STHASH_TABLE *pTable);

#ifdef __cplusplus
}
#endif


#endif
