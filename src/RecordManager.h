/*
 * RecordManager.h
 *
 *  Created on: 2015年11月1日
 *      Author: yangyuming
 */

#ifndef RECORDMANAGER_H_
#define RECORDMANAGER_H_

/*
 * RecordManager.cpp
 *
 *  Created on: 2015年11月1日
 *      Author: yangyuming
 */
#include "RecordManager.h"

#include "BufferManager.h"
#include "Declaration.h"
#include "IndexManager.h"

bool createIndex(string DB_Name, TableInfo table, string Attr_Name,
		string indexName);

void selectRecord(string DB_Name, vector<string>& columns, TableInfo table,
		vector<Condition>& conds);

bool insertRecord(string DB_Name, TableInfo table, Record insertedRecord);

bool deleteRecord(string DB_Name, TableInfo table, vector<Condition>& conds);

//API只要用到前面四个函数

bool emptyRecord(string record);

bool compare(string s1, string s2, int type, string op);

void getData(string DB_Name, TableInfo table, Data& data, set<int> offsetSet,vector<Condition>& conds);

void getData(string DB_Name, TableInfo table, Data& data,vector<Condition>& conds);

void select(TableInfo table, Data dataIn, Data &dataOut,
		vector<string>& columns);

void coutPrint(vector<string>& columns, Data& dataOut);



Data selectData(string DB_Name, vector<string>& columns, TableInfo table,
		vector<Condition>& conds);



int NotFullBlock(string DB_Name, TableInfo table, int& blockOffset);



string emptyRecord(int length);




#endif /* RECORDMANAGER_H_ */
