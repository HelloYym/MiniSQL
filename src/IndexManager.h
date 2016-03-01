/*
 * IndexManager.h
 *
 *  Created on: 2015年10月11日
 *      Author: yangyuming
 */

#ifndef INDEXMANAGER_H_
#define INDEXMANAGER_H_

#include "Declaration.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
using namespace std;



string int_to_str(long value, int length);

int str_to_int(string str);

float str_to_float(string str);

int compareStr(string str1, string str2);

int compareInt(string int1, string int2);

int compareFloat(string float1, string float2);

/*
 *	查找一个值等于inform.value(inform为info结构体)的记录在table表中的记录号(即记录在表中的顺序位置)：
 *	参数说明：
 * 		database：数据库名，string型；
 * 		table_name：表格文件名，string型
 * 		inform：info结构体，是引用
 *		函数结束返回时，记录号保存在inform .offset中，若没有，inform .offset＝0。
 *	返回值：
 *		-7    inform .offset=0    索引文件为空
 *		-8    inform .offset=0    表中无记录
 *		-1    inform .offset=0    所查值的类型错误（int，float，char(n)）
 *		-3    inform .offset=0    读到异常块
 *		正数  叶子块号，   若inform .offset>0，记录存在表文件中，记录号为inform .offset
 *		正数  叶子块号 ，若inform .offset＝0，记录不在表文件中，返回的叶子块号在函数insert_one中要用（因为将值插入改块就够了）
 */
int search_one(string database, string table_name, struct index_info & inform);

/*
 *	查找一批>,  >=,  <, <=inform . value(inform为info结构体)的记录在table表中的记录号：
 *	参数说明：
 *		database：数据库名，string型；
 *		table_name：表格文件名，string型
 *		inform：info结构体，是引用
 *		type:	3 是 找出所有 < inform .value 的记录;
 * 				4 是 找出所有 <= inform .value 的记录;
 *	     		1 是 找出所有 > inform .value; 的记录；
 *				2 是 找出所有 >= inform .value 的记录;
 *	函数返回:
 *		满足条件的记录偏移量，存在容器vector<int>
 */
set<int> search_many(string database, string table_name, int type,
		struct index_info& inform);

/*
 *	插入一个值的函数，值和类型在参数inform结构体中：
 *	参数说明：
 *		database：数据库名，string型；
 *		table_name：表格文件名，string型
 *		inform：info结构体，是引用。
 */
void insert_one(string database, string table_name, struct index_info & inform);

/*
 * 删除一个值等于inform.value的记录
 */
void delete_one(string database, string table_name, struct index_info & inform);




int getLocation(index_info& inform, string info, int type);
void insertInLeaf(string database, string table_name,
		struct index_info & inform, int blockNum);

void insertInParent(string database, string table_name,
		struct index_info & inform, int N, string K, int N1);


int findFather(string database, string table_name, index_info inform, int n);

void deleteEntry(string database, string table_name, struct index_info & inform,
		int n, string K, int nod);

int getN(index_info& inform, bool isLeaf);

int getValueNum(string info);

int getLength(index_info inform);

/*将inform中的value位拓展到节点中相应的LENGTH的长度*/
string toLength(index_info inform);

int find_prev_leaf_sibling(string database, string table_name,
		struct index_info inform, int nodenum);
int find_next_leaf_sibling(string database, string table_name,
		struct index_info inform, int nodenum);

int find_left_child(string database, string table_name,
		struct index_info inform);

int find_right_child(string database, string table_name,
		struct index_info inform);

/*获取一个新的空块*/
int get_new_freeblocknum(string database, string table_name,
		struct index_info& inform);

/*删除一个块*/
void deleteBlock(string database, string table_name, struct index_info& inform,
		int n);

void constructList(string database, string table_name,
		struct index_info& inform);

void updateNode(blockInfo* b, string s);



#endif /* INDEXMANAGER_H_ */

