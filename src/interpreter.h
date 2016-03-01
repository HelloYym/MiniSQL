/*
 * interpreter.h
 *
 *  Created on: 2015年11月6日
 *      Author: yangyuming
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>
#include <iostream>
#include "Declaration.h"
using namespace std;


class Interpreter
{
private:
	string SQL;
	int start;
	string sql;
public:
	Interpreter();
	//读取用户输入
	string read_input();
	//验证create语句是否有效
	string create_clause(string, int);
	//验证create database语句是否有效
	string create_database(string, int);
	//验证create table语句是否有效
	string create_table(string, int);
	//获得属性
	string get_attribute(string, string);
	//验证create index语句是否有效
	string create_index(string, int);
	//验证create index on语句是否有效
	string create_index_on(string, int, string);
	//验证drop语句是否有效
	string drop_clause(string, int);
	//验证drop database语句是否有效
	string drop_database(string, int);
	//验证drop table语句是否有效
	string drop_table(string, int);
	//验证drop index语句是否有效
	string drop_index(string, int);
	//验证select 语句是否有效
	string select_clause(string, int);
	//验证select_from_where 语句是否有效
	string select_from_where(string, int, string);
	//验证insert 语句是否有效
	string insert_clause(string, int);
	//验证insert into values语句是否有效
	string insert_into_values(string, int, string);
	//验证delete语句是否有效
	string delete_clause(string, int);
	//验证 delete from where 语句是否有效
	string delete_from_where(string, int, string);
	//验证execfile语句是否有效
	string execfile_clause(string, int);
	//验证quit语句是否有效
	string quit_clause(string, int);
	//验证use语句是否有效
	string use_clause(string, int);
	//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
	string interpreter(string);
	void change(string&);
};

//获得属性组或文件组的每一项
//string get_part(string temp,string sql,string kind);


//将表达式转化为内部形式
//string get_expression(string temp,string sql);
//获取表达式组的每个表达式
//string get_each(string T,string sql,string condition);
//验证use语句是否有效
//string use_clause(string SQL,int start);




#endif /* INTERPRETER_H_ */
