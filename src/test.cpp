//#include "IndexManager.h"
//#include "RecordManager.h"
//#include <stdio.h>
//#include <iostream>
//#include <string>
//
//#include "BufferManager.h"
//using namespace std;
//
//void init() {
//	FILE* fp = fopen("./db/db_table_index.index", "w");
//	fseek(fp, 4096 * 257, 0);
//	fprintf(fp, "a");
//	fclose(fp);
//	fp = fopen("./db/db_table.table", "w");
//	fseek(fp, 4096 * 257, 0);
//	fprintf(fp, "b");
//	fclose(fp);
//}
//
//int main() {
//	init();
//
//	string filename = "db_table.table";
//	TableInfo table;
//	table.attrNum = 3;
//	table.name = "table";
//	table.totalLength = 4 + 10 + 3;
//	AttrInfo attrInt, attrChar, attrFloat;
//	attrInt.indexName = "NULL";
//	attrInt.length = 4;
//	attrInt.name = "age";
//	attrInt.type = 0;
//
//	attrFloat.indexName = "NULL";
//	attrFloat.length = 10;
//	attrFloat.name = "grade";
//	attrFloat.type = 1;
//
//	attrChar.indexName = "NULL";
//	attrChar.length = 3;
//	attrChar.name = "name";
//	attrChar.type = 2;
//
//	table.attrs.push_back(attrChar);
//	table.attrs.push_back(attrInt);
//	table.attrs.push_back(attrFloat);
//
//	for (int i = 0; i < 1; i++) {
//		Record insertedRecord;
//		insertedRecord.blockNum = 0;
//		insertedRecord.offset = 0;
//		insertedRecord.columns.push_back("yym");
//		insertedRecord.columns.push_back("0021");
//		insertedRecord.columns.push_back("0000023.24");
//		insertRecord("db", table, insertedRecord);
//	}
//
//	for (int i = 0; i < 1; i++) {
//		Record insertedRecord;
//		insertedRecord.blockNum = 0;
//		insertedRecord.offset = 0;
//		insertedRecord.columns.push_back("xxx");
//		insertedRecord.columns.push_back("0020");
//		insertedRecord.columns.push_back("0000099.99");
//		insertRecord("db", table, insertedRecord);
//	}
//
//	for (int i = 0; i < 1; i++) {
//		Record insertedRecord;
//		insertedRecord.blockNum = 0;
//		insertedRecord.offset = 0;
//		insertedRecord.columns.push_back("lll");
//		insertedRecord.columns.push_back("0026");
//		insertedRecord.columns.push_back("0000099.99");
//		insertRecord("db", table, insertedRecord);
//	}
//
//	vector<string> columns;
//	columns.push_back("name");
//	columns.push_back("age");
//
//	Condition cond;
//	cond.attr = "age";
//	cond.compare = "<=";
//	cond.data = "0020";
//
//	vector<Condition> conds;
//	conds.push_back(cond);
//
//	//selectRecord("db", columns, table, conds);
//
//	table.attrs[0].indexName = "index";
//
//	createIndex("db", table, "name", "index");
//
////	conds.clear();
////	cond.attr = "name";
////	cond.compare = "=";
////	cond.data = "yym";
////	conds.push_back(cond);
////	selectRecord("db", columns, table, conds);
//
//	for (int i = 0; i < 1; i++) {
//		Record insertedRecord;
//		insertedRecord.blockNum = 0;
//		insertedRecord.offset = 0;
//		insertedRecord.columns.push_back("wan");
//		insertedRecord.columns.push_back("0024");
//		insertedRecord.columns.push_back("0000099.99");
//		insertRecord("db", table, insertedRecord);
//	}
//
//	//conds.clear();
//
//
//	cond.attr = "age";
//	cond.compare = ">";
//	cond.data = "0025";
//	conds.push_back(cond);
//
//
//
//	selectRecord("db", columns, table, conds);
//
//	deleteRecord("db", table, conds);
//
//	for (int i = 0; i < 2; i++) {
//		blockInfo* node = readBlock("db", "db_table.table", i, 0);
//		string info = node->cBlock;
//		cout << "block " << node->blockNum << " : " << info << endl;
//	}
//
//	for (int i = 0; i < 2; i++) {
//		blockInfo* node = readBlock("db", "db_table_index.index", i, 1);
//		string info = node->cBlock;
//		cout << "block " << node->blockNum << " : " << info << endl;
//	}
//
//	quitProg("db");
//
//}
