/*
 * Declaration.h
 *
 *  Created on: 2015年11月1日
 *      Author: yangyuming
 */

#ifndef DECLARATION_H_
#define DECLARATION_H_

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <set>
using namespace std;

#define INT 0
#define FLOAT 1
#define CHAR 2

class Record {
public:
	int blockNum;
	long offset;
	vector<string> columns;
};

class Data {
public:
	vector<Record> records;
};

class AttrInfo
{
public:
	string name;
	int type;
	int length;
	bool isPrimeryKey;
	bool isUnique;
	string indexName;
	AttrInfo():isPrimeryKey(false), isUnique(false), indexName("NULL"){};
	AttrInfo(string a_name, int a_type, int a_length, bool isP, bool isU):
	name(a_name), type(a_type), length(a_length), isPrimeryKey(isP), isUnique(isU), indexName("NULL"){};
	void setIndex(string a_indexName)
	{
		indexName = a_indexName;
	}
	void setPrimeryKey(void)
	{
		isPrimeryKey = true;
		isUnique = true;
	}
};

class TableInfo
{
public:
	string name;
	int attrNum;
	int totalLength;
	vector<AttrInfo> attrs;
	TableInfo(){

	}
	TableInfo(string a_name, vector<AttrInfo> a_attrs):name(a_name), attrNum(a_attrs.size()), attrs(a_attrs), totalLength(0){
		for(unsigned int i = 0; i < a_attrs.size(); i++)
			totalLength += a_attrs[i].length;
	}
	void setTotalLength(int a_totalLength){totalLength = a_totalLength;}
	void addAttr(string a_name, int a_type, int length, bool isP, bool isU)
	{
		AttrInfo temp = AttrInfo(a_name, a_type, length, isP, isU);
		attrs.push_back(temp);
		attrNum += 1;
		totalLength = 0;
		totalLength += temp.length;
	}
};
class Index {
public:
	string name;
	string tableName;
	string attrName;
	Index() {
	}
	;
	Index(string a_name, string a_tableName, string a_attrName) :
			name(a_name), tableName(a_tableName), attrName(a_attrName) {
	}
	;
};

class Condition {
public:
	string attr;
	string compare;
	string data;
	Condition(){
	}
	Condition(string a_attr, string a_compare, string a_data) :
			attr(a_attr), compare(a_compare), data(a_data) {
	}
	;
};

struct fileInfo {
	int type;					// 0-> data file, 1 -> index file
	string fileName;			// the name of the file
	int recordAmount;			// the number of record in the file
	int freeNum;	// the free block number which could be used for the file
	int recordLength;			// the length of the record in the file
	struct fileInfo *next;				// the pointer points to the next file
	struct blockInfo *firstBlock;	// point to the first block within the file
};

struct blockInfo {
	int blockNum;// the block number of the block, which indicate it when it be newed
	bool dirtyBit;				// 0 -> flase
								// 1 -> indicate dirty, write back
	struct blockInfo *next;		// the pointer point to next block
	fileInfo *file;	// the pointer point to the file, which the block belongs to
	int charNum;				// the number of chars in the block
	char *cBlock;// the array space for storing the records in the block in buffer
	int iTime;					// it indicate the age of the block in use
	int lock;					// prevent the block from replacing
};

struct index_info {
	string index_name;			//the name of the index file
	int length;					//the length of the value
	char type;			//the type of the value, 0---int,1---float,2----char(n)
	long offset;				//the record offset in the table file
	string value;				//the value
};



#endif /* DECLARATION_H_ */
