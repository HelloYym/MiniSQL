/*
 * CatalogManager.h
 *
 *  Created on: 2015年11月6日
 *      Author: yangyuming
 */

#ifndef CATALOGMANAGER_H_
#define CATALOGMANAGER_H_
#pragma once

#include "Declaration.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class CatalogManager
{
private:
	string dbName;
	vector<TableInfo> tables;
	int tableNum;
	vector<Index> indexs;
	int indexNum;
public:
	CatalogManager(){};
	CatalogManager(string a_name){readCatalog(a_name);};
	~CatalogManager(){if(dbName != "")writeCatalog();};
	string getdbName(){return dbName;}
	int gettableNum(){return tableNum;}
	int getindexNum(){return indexNum;}
	TableInfo getTable(int tableP)
	{
		if(tableP >= tableNum)
		{
			cout << "Out of range of tables!" << endl;
			return TableInfo();
		}
		return tables[tableP];
	}
	Index getIndex(int indexP)
	{
		if(indexP >= indexNum)
		{
			cout << "Out of range of indexs1" << endl;
			return Index();
		}
		return indexs[indexP];
	}
	//读入数据字典文件
	bool readCatalog(string a_name);
	//写入数据字典文件
	bool writeCatalog();
	//检查是否有数据库正在使用
	bool isDBinUse();
	//检查数据库是否存在
	bool Exist_Database(string DB_Name);
	//检查数据库中表是否存在
	bool Exist_Table(string Table_Name, int &position);
	//检查表中属性是否存在
	bool Exist_Attr(string Attr_Name, int &position, int &tableP);
	//检查数据库中索引是否存在
	bool Exist_Index(string Index_Name, int &position);
	//检查数据类型与属性的数据类型是否可比，仅处理int与float
	bool Check_Type(int type, int length, string &data, string attr);

	//使用数据库
	bool Use_Database(string DB_Name);
	//退出数据库
	bool Exit_Database();
	//创建数据库
	void Create_Database(string DB_Name);
	//创建表
	void Create_Table(string Table_Name, vector<AttrInfo> newAttrs, vector<string> primaryKeys);
	//创建索引
	void Create_Index(string Index_Name, string Table_Name, string Attr_Name, int &tableP, int &attrP);
	//删除数据库
	void Drop_Database(string DB_Name);
	//删除表
	void Drop_Table(string Table_Name);
	//删除索引
	void Drop_Index(string Index_Name);
	//查询语句
	bool Select(string Table_Name, vector<string> Attrs, vector<Condition> &Condition, vector<int> &IndexP, int &tableP);
	//插入语句
	bool Insert(string Table_Name, int &tableP, vector<string> &Data);
	//删除表语句
	bool Delete(string Table_Name,vector<Condition> &Condition, vector<int> &IndexP, int &tableP);
	//删除表语句
	bool Delete_Table(string Table_Name);
};
#endif /* CATALOGMANAGER_H_ */
