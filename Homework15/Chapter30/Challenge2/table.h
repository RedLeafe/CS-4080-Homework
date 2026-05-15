#ifndef CLOX_TABLE_H
#define CLOX_TABLE_H

#include "common.h"
#include "value.h"

typedef struct Obj Obj;

typedef struct {
  Obj* key;
  Value value;
} Entry;

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, Obj* key, Value* value);
bool tableSet(Table* table, Obj* key, Value value);
bool tableDelete(Table* table, Obj* key);
void tableAddAll(Table* from, Table* to);
Obj* tableFindString(Table* table, const char* chars, int length, uint32_t hash);
void markTable(Table* table);
void tableRemoveWhite(Table* table);

#endif
