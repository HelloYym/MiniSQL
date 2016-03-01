/*
 * interpreter.cpp
 *
 *  Created on: 2015年11月6日
 *      Author: yangyuming
 */

#include "interpreter.h"

Interpreter::Interpreter()
{
	SQL="0";
	sql="0";
	start=0;
}
////////////////////////////////////////////////////////////////////////////////////////////
//读取用户输入
string Interpreter::read_input()
{
	string SQL;
	string temp;
	string str;
	bool finish = false;
	int start=0,end;
	//while(!finish)
	//{
	getline(cin,str);
	if(str.at(str.length()-1)!=';')
	{
		cout<<"syntax error: missing ; !"<<endl;
		SQL="99";
	}
	else
		while(!finish)
		{
			while(str.at(start)==' ')
				start++;
			end=str.find(' ',start);
			temp.assign(SQL,start,end-start);
			start=end+1;
			if(temp.at(temp.length()-1)!=';')
				SQL=SQL+" "+temp;
			else
			{
				SQL.at(SQL.length()-1)=' ';
				SQL+=";";
				finish=true;
			}
		}

	//将输入大写转化为小写
	for(int i=0;SQL[i]!='\0';i++)
	{
			if(SQL[i]>='A'&&SQL[i]<='Z')
			{
				SQL[i] += 32;
				continue;
			 }
	}
	cout << "命令行输入: " << SQL << endl;
	//返回用户输入
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证create database语句是否有效
string Interpreter::create_database(string SQL,int start)
{
	string temp;
	int index,end;
	//获取第三个单词
	while(SQL.at(start)==' ')
		start++;
	index=start;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.at(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.at(start)!=';'||start!=SQL.length()-1)
		{
			cout<<"error12:"<<SQL.assign(SQL,index,SQL.length()-index-2)<<"---is not a valid database name!"<<endl;
			SQL="99";
		}
		//返回create database语句的内部形式
		else
			SQL="00"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性
string Interpreter::get_attribute(string temp,string sql)
{
	int start=0,end,index;
	string T,C;
	temp+=" ";
	//获得属性名
	end=temp.find(' ',start);
	T.assign(temp,start,end-start);
	start=end+1;
	sql+=T+" ";
	//获得数据类型
	while(temp.at(start)==' ')
		start++;
	end=temp.find(' ',start);
	T.assign(temp,start,end-start);
	start=end+1;
	change(T);
	//若无，打印出错信息
	if(T.empty())
	{
		cout<<"error : error in create table statement!"<<endl;
		sql="99";
		return sql;
	}
	//若为int
	else if(T=="int")
		sql+="+";
	//若为float
	else if(T=="float")
		sql+="-";
	//其他
	else
	{
		index=T.find('(');
		C.assign(T,0,index);
		index++;
		change(C);
		//若有误，打印出错信息
		if(C.empty())
		{
			cout<<"error: "<<T<<"---is not a valid data type definition!"<<endl;
			sql="99";
			return sql;
		}
		//若为char
		else if(C=="char")
		{
			C.assign(T,index,T.length()-index-1);
			if(C.empty())
			{
				cout<<"error: the length of the data type char has not been specified!"<<endl;
				sql="99";
				return sql;
			}
			else
				sql+=C;
		}
		//若为非法信息，打印出错信息
		else
		{
			cout<<"error: "<<C<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//是否有附加信息
	while(start<temp.length()&&temp.at(start)==' ')
		start++;
	if(start<temp.length())
	{
		//若为unique定义，保存信息
		end=temp.find(' ',start);
		T.assign(temp,start,end-start);
		change(T);
		if(T=="unique")
		{
			sql+=" 1,";
		}
		//若为非法信息，打印出错信息
		else
		{
			cout<<"error: "<<temp<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//若无附加信息
	else
		sql+=" 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create table语句是否有效
string Interpreter::create_table(string SQL,int start)
{
	string temp,sql,T;
	int index,end,length;
	//获取表名
	while(SQL.at(start)==' ')
		start++;
	index=start;
	if((end=SQL.find('(',start))==-1)
	{
		cout<<"error: missing ( in the statement!"<<endl;
		SQL="99";
		return SQL;
	}
	temp.assign(SQL,start,end-start);
	start=end+1;
	if(!temp.empty())
	{
		while(SQL.at(start)==' ')
			start++;
		length=temp.length()-1;
		while(temp.at(length)==' ')
			length--;
		temp.assign(temp,0,length+1);
	}
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"error: error in create table statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//若为非法信息，打印出错信息
	else if(temp.find(' ')!=-1)
	{
		cout<<"error: "<<temp<<"---is not a valid table name!"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		sql=temp+",";
		//获取每个属性
		while((end=SQL.find(',',start))!=-1)
		{
			temp.assign(SQL,start,end-start);
			start=end+1;
			//若有空属性，打印出错信息
			if(temp.empty())
			{
				cout<<"error: error in create table statement!"<<endl;
				SQL="99";
				return SQL;
			}
			//保存属性
			else
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
					return sql;
			}
			while(SQL.at(start)==' ')
				start++;
		}
		//获得最后属性
		temp.assign(SQL,start,SQL.length()-start-1);
		length=temp.length()-1;
		while(temp.at(length)!=')'&&length>=0)
			length--;
		//若无，打印出错信息
		if(length<1)
		{
			cout<<"error: error in create table statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//存储属性
		else
		{
			temp.assign(temp,0,length);
			end=SQL.find(' ',start);
			T.assign(SQL,start,end-start);
			start=end+1;
			change(T);
			//若为主键定义
			if(T=="primary")
			{
				//判断是否有关键字key
				temp+=")";
				while(SQL.at(start)==' ')
					start++;
				end=SQL.find('(',start);
				T.assign(SQL,start,end-start);
				start=end+1;
				length=T.length()-1;
				while(T.at(length)==' ')
					length--;
				T.assign(T,0,length+1);
				change(T);
				//若为空，打印出错信息
				if(T.empty())
				{
					cout<<"syntax error: syntax error in create table statement!"<<endl;
					cout<<"\t missing key word key!"<<endl;
					SQL="99";
					return SQL;
				}
				//若有，继续验证
				else if(T=="key")
				{
					//获取主键属性名
					while(SQL.at(start)==' ')
						start++;
					end=SQL.find(')',start);
					T.assign(SQL,start,end-start);
					length=T.length()-1;
					while(T.at(length)==' ')
						length--;
					T.assign(T,0,length+1);
					//若无，打印出错信息
					if(T.empty())
					{
						cout<<"error : missing primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//若为非法信息，打印出错信息
					else if(T.find(' ')!=-1)
					{
						cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//保存主键
					else
					{
						sql+=T+" #,";
						SQL="01"+sql;
					}
				}
				//若为非法信息，打印出错信息
				else
				{
					cout<<"error : "<<T<<"---is not a valid key word!"<<endl;
					SQL="99";
					return SQL;
				}
			}
			//若为一般属性
			else
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
				{
					SQL="99";
					return SQL;
				}
				else
					SQL="01"+sql;
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index on语句是否有效
string Interpreter::create_index_on(string SQL,int start,string sql)
{
	string temp;
	int end,length;
	//获取表名
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find('(',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		cout<<"\t missing ( !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//检验是否为有效文件名
		length=temp.length()-1;
		while(temp.at(length)==' ')
			length--;
		temp.assign(temp,0,length+1);
		//有效
		if(temp.find(' ')==-1)
		{
			sql+=" "+temp;
			//获取属性名
			while(SQL.at(start)=='('||SQL.at(start)==' ')
				start++;
			end=SQL.find(')',start);
			temp.assign(SQL,start,end-start);
			start=end+1;
			//若无，打印出错信息
			if(temp.empty())
			{
				cout<<"syntax error: syntax error for create index statement!"<<endl;
				cout<<"\t missing ) !"<<endl;
				SQL="99";
				return SQL;
			}
			else
			{
				//检验是否为有效属性名
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				//有效
				if(temp.find(' ')==-1)
				{
					sql+=" "+temp;
					while(SQL.at(start)==' ')
						start++;
					if(SQL.at(start)!=';'||start!=SQL.length()-1)
					{
						cout<<"syntax error: syntax error for quit!"<<endl;
						SQL="99";
						return SQL;
					}
					//返回create index语句的内部形式
					else
						SQL="02"+sql;
				}
				//无效,打印出错信息
				else
				{
					cout<<"error:"<<" "<<temp<<"---is not a valid attribute name!"<<endl;
					SQL="99";
					return SQL;
				}
			}
		}
		//无效,打印出错信息
		else
		{
			cout<<"error:"<<" "<<temp<<"---is not a valid table name!"<<endl;
			SQL="99";
			return SQL;
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index语句是否有效
string Interpreter::create_index(string SQL,int start)
{
	string temp,sql;
	int end;
	//获取第三个单词
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		SQL="99";
	}
	else
	{
		sql=temp;
		//获取第四个单词
		while(SQL.at(start)==' ')
			start++;
		end=SQL.find(' ',start);
		temp.assign(SQL,start,end-start);
		start=end+1;
		change(temp);
		//若无，打印出错信息
		if(temp.empty())
		{
			cout<<"syntax error: syntax error for create index statement!"<<endl;
			SQL="99";
		}
		//若为on,继续验证
		else if(temp=="on")
			SQL=create_index_on(SQL,start,sql);
		//若为非法信息，打印非法信息
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			SQL="99";
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
string Interpreter::create_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取第二个单词
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	change(temp);
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for create statement!"<<endl;
		SQL="99";
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=create_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=create_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=create_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回create语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
string Interpreter::drop_database(string SQL,int start)
{
	string temp;
	int index,end;
	//获取第三个单词
	while(SQL.at(start)==' ')
		start++;
	index=start;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"error: database name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.at(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.at(start)!=';'||start!=SQL.length()-1)
		{
			cout<<"error:"<<SQL.assign(SQL,index,SQL.length()-index-2)<<"---is not a valid database name!"<<endl;
			SQL="99";
		}
		//返回drop database语句的内部形式
		else
			SQL="10"+temp;
	}
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
string Interpreter::drop_table(string SQL,int start)
{
	string temp;
	int index,end;
	//获取第三个单词
	while(SQL.at(start)==' ')
		start++;
	index=start;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"error: table name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.at(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.at(start)!=';'||start!=SQL.length()-1)
		{
			cout<<"error:"<<SQL.assign(SQL,index,SQL.length()-index-2)<<"---is not a valid table name!"<<endl;
			SQL="99";
		}
		//返回drop table语句的内部形式
		else
			SQL="11"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
string Interpreter::drop_index(string SQL,int start)
{
	string temp;
	int index,end;
	//获取第三个单词
	while(SQL.at(start)==' ')
		start++;
	index=start;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"error: index name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.at(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.at(start)!=';'||start!=SQL.length()-1)
		{
			cout<<"error:"<<SQL.assign(SQL,index,SQL.length()-index-2)<<"---is not a valid index name!"<<endl;
			SQL="99";
		}
		//返回drop index语句的内部形式
		else
			SQL="12"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop语句是否有效
string Interpreter::drop_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取第二个单词
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	change(temp);
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for drop statement!"<<endl;
		SQL="99";
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=drop_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=drop_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=drop_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回drop语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证select from where语句是否有效
string Interpreter::select_from_where(string SQL,int start,string sql)
{
	string temp,Temp;
	int end,length,end1,end2,end3,end4,k;
	bool finish=false;
	//获取条件
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(';',start);
	temp.assign(SQL,start,end-start);
	//start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for select statement!"<<endl;
		cout<<"\t missing ; !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//检验是否为有效条件
		length=temp.length()-1;
		while(temp.at(length)==' ')
			length--;
		temp.assign(temp,0,length+1);
		//单条件
		if(temp.find(' ')==-1&&temp.length()>1)
		{
			//while(SQL.at(start)==' ')
				//start++;
			end1=SQL.find('<',start);
			end2=SQL.find('>',start);
			end3=SQL.find('=',start);
			end4=SQL.find('!',start);
			if(end1>0&&end2<0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end1-start);
				start=end1+1;
				sql+=","+temp+" <";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end2-start);
				start=end2+1;
				sql+=","+temp+" >";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start);
				start=end3+1;
				sql+=","+temp+" =";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				start=end3+1;
				sql+=","+temp+" <=";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				start=end3+1;
				sql+=","+temp+" >=";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2>0&&end3<0&&end4<0)
			{
				if(end1<end2)
				{
					temp.assign(SQL,start,end2-start-1);
					start=end2+1;
					sql+=","+temp+" <>";
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
				else
				{
					temp.assign(SQL,start,end1-start-1);
					start=end1+1;
					sql+=","+temp+" ><";
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
			}
			else if(end1<0&&end2<0&&end3>0&&end4>0)
			{
				temp.assign(SQL,start,end3-start-1);
				start=end3+1;
				sql+=","+temp+" !=";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else
			{
				cout<<"error:"<<" missing > < = <= >= <> >< != !"<<endl;
				SQL="99";
				return SQL;
			}

		}
		//无效,打印出错信息
		else if(temp.find(' ')==-1)
		{
			cout<<"error:"<<" "<<temp<<"---iss not a valid condition!"<<endl;
			SQL="99";
			return SQL;
		}
		//含空格的单属性
		else if(temp.find("and")==-1)
		{
			end1=SQL.find('<',start);
			end2=SQL.find('>',start);
			end3=SQL.find('=',start);
			end4=SQL.find('!',start);
			if(end1>0&&end2<0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end1-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end1+1;
				sql+=","+temp+" <";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end2-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end2+1;
				sql+=","+temp+" >";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" =";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" <=";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" >=";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2>0&&end3<0&&end4<0)
			{
				if(end1<end2)
				{
					temp.assign(SQL,start,end2-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end2+1;
					sql+=","+temp+" <>";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
				else
				{
					temp.assign(SQL,start,end1-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end1+1;
					sql+=","+temp+" <>";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
			}
			else if(end1<0&&end2<0&&end3>0&&end4>0)
			{
				temp.assign(SQL,start,end3-start-1);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" !=";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else
			{
				cout<<"error:"<<" missing > < = <= >= <> >< != !"<<endl;
				SQL="99";
				return SQL;
			}
		}
		//多条件
		else if(temp.find("and")>0)
		{
			sql+=",";
			do{
				if(SQL.find("and",start)!=-1)
				{
					end=SQL.find("and",start)+3;
					Temp.assign(SQL,start,end-start-3);
					k=3;
				}
				else
				{
					end=SQL.find(';',start);
					Temp.assign(SQL,start,end-start);
					finish=true;
					k=0;
				}
				end1=Temp.find('<',0);
				end2=Temp.find('>',0);
				end3=Temp.find('=',0);
				end4=Temp.find('!',0);
				if(end1>0&&end2<0&&end3<0&&end4<0)
				{
					end1=SQL.find('<',start);
					temp.assign(SQL,start,end1-start);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end1+1;
					sql+=temp+" < ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1<0&&end2>0&&end3<0&&end4<0)
				{
					end2=SQL.find('>',start);
					temp.assign(SQL,start,end2-start);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end2+1;
					sql+=temp+" > ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1<0&&end2<0&&end3>0&&end4<0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" = ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1>0&&end2<0&&end3>0&&end4<0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" <= ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1<0&&end2>0&&end3>0&&end4<0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" >= ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1>0&&end2>0&&end3<0&&end4<0)
				{
					if(end1<end2)
					{
						end2=SQL.find('>',start);
						temp.assign(SQL,start,end2-start-1);
						length=temp.length()-1;
						while(temp.at(length)==' ')
							length--;
						temp.assign(temp,0,length+1);
						start=end2+1;
						sql+=temp+" <> ";
						while(SQL.at(start)==' ')
							start++;
						temp.assign(SQL,start,end-start-k);
						sql+=temp;
					}
					else
					{
						end1=SQL.find('<',start);
						temp.assign(SQL,start,end1-start-1);
						length=temp.length()-1;
						while(temp.at(length)==' ')
							length--;
						temp.assign(temp,0,length+1);
						start=end1+1;
						sql+=temp+" >< ";
						while(SQL.at(start)==' ')
							start++;
						temp.assign(SQL,start,end-start-k);
						sql+=temp;
					}
				}
				else if(end1<0&&end2<0&&end3>0&&end4>0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" != ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else
				{
					cout<<"error:"<<" missing > < = <= >= <> >< != !"<<endl;
					SQL="99";
					return SQL;
				}
				if(end<(SQL.length()-1))
				{
					start=end+1;
					while(SQL.at(start)==' ')
						start++;
				}
				sql+=" and ";
			}while(!finish);
			sql.assign(sql,0,sql.length()-5);
		}
		else
		{
			cout<<"error:"<<" wrong condition"<<endl;
			SQL="99";
			return SQL;
		}
	}
	SQL = "20" + sql;
	return SQL;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//验证select语句是否有效
string Interpreter::select_clause(string SQL,int start)
{
	string temp,temp1,sql;
	int end,length;
	bool finish = false;
	//获取第二个单词
	if(SQL.find(',')==-1)
	{
		while(SQL.at(start)==' ')
			start++;
		end=SQL.find(' ',start);
		temp.assign(SQL,start,end-start);
		start=end+1;
		//若无，打印出错信息
		if(temp.empty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			SQL="99";
		}
		else if(temp=="*")
		{
			//获取第三个单词
			while(SQL.at(start)==' ')
				start++;
			end=SQL.find(' ',start);
			temp.assign(SQL,start,end-start);
			start=end+1;
			//若无，打印出错信息
			if(temp.empty())
			{
				cout<<"syntax error: syntax error for select statement!"<<endl;
				SQL="99";
			}
			else if(temp=="from")
			{
				//获取表名
				while(SQL.at(start)==' ')
					start++;
				end=SQL.find(' ',start);
				temp.assign(SQL,start,end-start);
				start=end+1;
				//若无，打印出错信息
				if(temp.empty())
				{
					cout<<"syntax error: syntax error for select statement!"<<endl;
					SQL="99";
				}
				else
				{
					sql=temp;
					//获取第五个单词
					while(SQL.at(start)==' ')
						start++;
					end=SQL.find(' ',start);
					temp.assign(SQL,start,end-start);
					start=end+1;
					//若无，则为select * from 表名格式
					if(temp==";")
						SQL="20"+sql;
					else if(temp=="where")
						SQL=select_from_where(SQL,start,sql);
					//若为非法信息，打印非法信息
					else
					{
						cout<<"syntax error:"<<" "<<temp<<"---is not a valid key world!"<<endl;
						SQL="99";
					}
				}

			}
			//若为非法信息，打印非法信息
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				SQL="99";
			}

		}
		//单属性
		else
		{
			sql=temp;
			//获取第三个单词
			while(SQL.at(start)==' ')
				start++;
			end=SQL.find(' ',start);
			temp.assign(SQL,start,end-start);
			start=end+1;
			change(temp);
			//若无，打印出错信息
			if(temp.empty())
			{
				cout<<"syntax error: syntax error for select statement!"<<endl;
				SQL="99";
			}
			else if(temp=="from")
			{
				//获取第表名
				while(SQL.at(start)==' ')
					start++;
				end=SQL.find(' ',start);
				temp.assign(SQL,start,end-start);
				start=end+1;
				//若无，打印出错信息
				if(temp.empty())
				{
					cout<<"syntax error: syntax error for select statement!"<<endl;
					SQL="99";
				}
				else
				{
					sql=sql+' '+temp;
					//获取第五个单词
					while(SQL.at(start)==' ')
						start++;
					end=SQL.find(' ',start);
					temp.assign(SQL,start,end-start);
					start=end+1;
					change(temp);
					//若无，则为select 属性 from 表名格式
					if(temp==";")
						SQL="20"+sql;
					else if(temp=="where")
						SQL=select_from_where(SQL,start,sql);
					//若为非法信息，打印非法信息
					else
					{
						cout<<"syntax error:"<<" "<<temp<<"---is not a valid key world!"<<endl;
						SQL="99";
					}
				}

			}
			//若为非法信息，打印非法信息
			else
			{
				cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
				SQL="99";
			}

		}
	}
	//若为多属性
	else
	{
		//获取属性
		while(!finish)
		{
			while(SQL.at(start)==' ')
				start++;
			if((end=SQL.find(',',start))==-1)
			{
				end=SQL.find(' ',start);
				temp.assign(SQL,start,end-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end+1;
				sql+=temp+" ";
				finish=true;
			}
			else
			{
				end=SQL.find(',',start);
				temp.assign(SQL,start,end-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end+1;
				sql+=temp+',';
			}

		}
		//获取第三个单词
		while(SQL.at(start)==' ')
			start++;
		end=SQL.find(' ',start);
		temp.assign(SQL,start,end-start);
		start=end+1;
		change(temp);
		//若无，打印出错信息
		if(temp.empty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			SQL="99";
		}
		else if(temp=="from")
		{
			//获取表名
			while(SQL.at(start)==' ')
				start++;
			end=SQL.find(' ',start);
			temp.assign(SQL,start,end-start);
			start=end+1;
			change(temp);
			//若无，打印出错信息
			if(temp.empty())
			{
				cout<<"syntax error: syntax error for select statement!"<<endl;
				SQL="99";
			}
			else
			{
				sql+=temp;
				//获取第五个单词
				while(SQL.at(start)==' ')
					start++;
				end=SQL.find(' ',start);
				temp.assign(SQL,start,end-start);
				start=end+1;
				//若无，则为select 多属性 from 表名格式
				if(temp==";")
					SQL="20"+sql;
				else if(temp=="where")
					SQL=select_from_where(SQL,start,sql);
				//若为非法信息，打印非法信息
				else
				{
					cout<<"syntax error:"<<" "<<temp<<"---is not a valid key world!"<<endl;
					SQL="99";
				}
			}

		}
		//若为非法信息，打印非法信息
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			SQL="99";
		}

	}
	//返回select语句的内部形式
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert_into_values语句是否有效
string Interpreter::insert_into_values(string SQL,int start,string sql)
{
	string temp;
	int end;
	while(SQL.at(start)=='(')
		start++;
	//获取每个值
	while((end=SQL.find(',',start))!=-1)
	{
		temp.assign(SQL,start,end-start);
		start=end+1;
		//若有空值，打印出错信息
		if(temp.empty())
		{
			cout<<"error: error in insert into values statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//保存值
		else
			sql+=" "+temp;
		while(SQL.at(start)==' ')
			start++;
	}
	end = SQL.find(')', start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若有空值，打印出错信息
	if(temp.empty())
	{
		cout<<"error: error in insert into values statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//保存值
	else
		sql+=" "+temp;
	SQL="30"+sql;
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert语句是否有效
string Interpreter::insert_clause(string SQL,int start)
{
	string temp,sql;
	int end;
	//获取表名
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for insert statement!"<<endl;
		SQL="99";
	}
	else
	{
		sql=temp;
		//获取第四个单词
		while(SQL.at(start)==' ')
			start++;
		end=SQL.find('(',start);
		temp.assign(SQL,start,end-start);
		start=end+1;
		change(temp);
		//若无，打印出错信息
		if(temp.empty())
		{
			cout<<"syntax error: syntax error for insert statement!"<<endl;
			SQL="99";
		}
		//若为on,继续验证
		else if(temp=="values")
			SQL=insert_into_values(SQL,start,sql);
		//若为非法信息，打印非法信息
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			SQL="99";
		}
	}
	return SQL;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//验证delete from where语句是否有效
string Interpreter::delete_from_where(string SQL,int start,string sql)
{
	string temp,Temp;
	int end,length,end1,end2,end3,end4,k;
	bool finish=false;
	//获取条件
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(';',start);
	temp.assign(SQL,start,end-start);
	//start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for select statement!"<<endl;
		cout<<"\t missing ; !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//检验是否为有效条件
		length=temp.length()-1;
		while(temp.at(length)==' ')
			length--;
		temp.assign(temp,0,length+1);
		//单条件
		if(temp.find(' ')==-1&&temp.length()>1)
		{
			//while(SQL.at(start)==' ')
				//start++;
			end1=SQL.find('<',start);
			end2=SQL.find('>',start);
			end3=SQL.find('=',start);
			end4=SQL.find('!',start);
			if(end1>0&&end2<0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end1-start);
				start=end1+1;
				sql+=","+temp+" <";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end2-start);
				start=end2+1;
				sql+=","+temp+" >";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start);
				start=end3+1;
				sql+=","+temp+" =";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				start=end3+1;
				sql+=","+temp+" <=";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				start=end3+1;
				sql+=","+temp+" >=";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2>0&&end3<0&&end4<0)
			{
				if(end1<end2)
				{
					temp.assign(SQL,start,end2-start-1);
					start=end2+1;
					sql+=","+temp+" <>";
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
				else
				{
					temp.assign(SQL,start,end1-start-1);
					start=end1+1;
					sql+=","+temp+" ><";
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
			}
			else if(end1<0&&end2<0&&end3>0&&end4>0)
			{
				temp.assign(SQL,start,end3-start-1);
				start=end3+1;
				sql+=","+temp+" !=";
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else
			{
				cout<<"error:"<<" missing > < = <= >= <> >< != !"<<endl;
				SQL="99";
				return SQL;
			}

		}
		//无效,打印出错信息
		else if(temp.find(' ')==-1)
		{
			cout<<"error:"<<" "<<temp<<"---iss not a valid condition!"<<endl;
			SQL="99";
			return SQL;
		}
		//含空格的单属性
		else if(temp.find('and')==-1)
		{
			end1=SQL.find('<',start);
			end2=SQL.find('>',start);
			end3=SQL.find('=',start);
			end4=SQL.find('!',start);
			if(end1>0&&end2<0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end1-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end1+1;
				sql+=","+temp+" <";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3<0&&end4<0)
			{
				temp.assign(SQL,start,end2-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end2+1;
				sql+=","+temp+" >";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" =";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2<0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" <=";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1<0&&end2>0&&end3>0&&end4<0)
			{
				temp.assign(SQL,start,end3-start-1);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" >=";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else if(end1>0&&end2>0&&end3<0&&end4<0)
			{
				if(end1<end2)
				{
					temp.assign(SQL,start,end2-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end2+1;
					sql+=","+temp+" <>";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
				else
				{
					temp.assign(SQL,start,end1-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end1+1;
					sql+=","+temp+" <>";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start);
					sql+=" "+temp;
				}
			}
			else if(end1<0&&end2<0&&end3>0&&end4>0)
			{
				temp.assign(SQL,start,end3-start-1);
				length=temp.length()-1;
				while(temp.at(length)==' ')
					length--;
				temp.assign(temp,0,length+1);
				start=end3+1;
				sql+=","+temp+" !=";
				while(SQL.at(start)==' ')
					start++;
				temp.assign(SQL,start,end-start);
				sql+=" "+temp;
			}
			else
			{
				cout<<"error:"<<" missing > < = <= >= <> >< != !"<<endl;
				SQL="99";
				return SQL;
			}
		}
		//多条件
		else if(temp.find("and")>0)
		{
			sql+=",";
			do{
				if(SQL.find("and",start)!=-1)
				{
					end=SQL.find("and",start)+3;
					Temp.assign(SQL,start,end-start-3);
					k=3;
				}
				else
				{
					end=SQL.find(';',start);
					Temp.assign(SQL,start,end-start);
					finish=true;
					k=0;
				}
				end1=Temp.find('<',0);
				end2=Temp.find('>',0);
				end3=Temp.find('=',0);
				end4=Temp.find('!',0);
				if(end1>0&&end2<0&&end3<0&&end4<0)
				{
					end1=SQL.find('<',start);
					temp.assign(SQL,start,end1-start);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end1+1;
					sql+=temp+" < ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1<0&&end2>0&&end3<0&&end4<0)
				{
					end2=SQL.find('>',start);
					temp.assign(SQL,start,end2-start);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end2+1;
					sql+=temp+" > ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1<0&&end2<0&&end3>0&&end4<0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" = ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1>0&&end2<0&&end3>0&&end4<0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" <= ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1<0&&end2>0&&end3>0&&end4<0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" >= ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else if(end1>0&&end2>0&&end3<0&&end4<0)
				{
					if(end1<end2)
					{
						end2=SQL.find('>',start);
						temp.assign(SQL,start,end2-start-1);
						length=temp.length()-1;
						while(temp.at(length)==' ')
							length--;
						temp.assign(temp,0,length+1);
						start=end2+1;
						sql+=temp+" <> ";
						while(SQL.at(start)==' ')
							start++;
						temp.assign(SQL,start,end-start-k);
						sql+=temp;
					}
					else
					{
						end1=SQL.find('<',start);
						temp.assign(SQL,start,end1-start-1);
						length=temp.length()-1;
						while(temp.at(length)==' ')
							length--;
						temp.assign(temp,0,length+1);
						start=end1+1;
						sql+=temp+" >< ";
						while(SQL.at(start)==' ')
							start++;
						temp.assign(SQL,start,end-start-k);
						sql+=temp;
					}
				}
				else if(end1<0&&end2<0&&end3>0&&end4>0)
				{
					end3=SQL.find('=',start);
					temp.assign(SQL,start,end3-start-1);
					length=temp.length()-1;
					while(temp.at(length)==' ')
						length--;
					temp.assign(temp,0,length+1);
					start=end3+1;
					sql+=temp+" != ";
					while(SQL.at(start)==' ')
						start++;
					temp.assign(SQL,start,end-start-k);
					sql+=temp;
				}
				else
				{
					cout<<"error:"<<" missing > < = <= >= <> >< != !"<<endl;
					SQL="99";
					return SQL;
				}
				if(end<(SQL.length()-1))
				{
					start=end+1;
					while(SQL.at(start)==' ')
						start++;
				}
				sql+=" and ";
			}while(!finish);
			sql.assign(sql,0,sql.length()-5);
		}
		else
		{
			cout<<"error:"<<" wrong condition"<<endl;
			SQL="99";
			return SQL;
		}
	}
	SQL = "40" + sql;
	return SQL;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//验证delete语句是否有效
string Interpreter::delete_clause(string SQL,int start)
{
	string temp,sql;
	int end;
	//获取第二个单词
		while(SQL.at(start)==' ')
			start++;
		end=SQL.find(' ',start);
		temp.assign(SQL,start,end-start);
		start=end+1;
		change(temp);
		//若无，打印出错信息
		if(temp.empty())
		{
			cout<<"syntax error: syntax error for select statement!"<<endl;
			SQL="99";
		}
		else if(temp=="from")
		{
			//获取表名
			while(SQL.at(start)==' ')
				start++;
			end=SQL.find(' ',start);
			temp.assign(SQL,start,end-start);
			start=end+1;
			//若无，打印出错信息
			if(temp.empty())
			{
				cout<<"syntax error: syntax error for select statement!"<<endl;
				SQL="99";
			}
			else
			{
				sql=temp;
				//获取第四个单词
				while(SQL.at(start)==' ')
					start++;
				end=SQL.find(' ',start);
				temp.assign(SQL,start,end-start);
				start=end+1;
				change(temp);
				//若无，则为delete from 表名格式
				if(temp==";")
					SQL="40"+sql;
				//若为"where",继续验证
				else if(temp=="where")
					SQL=delete_from_where(SQL,start,sql);
				//若为非法信息，打印非法信息
				else
				{
					cout<<"syntax error:"<<" "<<temp<<"---is not a valid key world!"<<endl;
					SQL="99";
				}
			}

		}
		//若为非法信息，打印非法信息
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			SQL="99";
		}

	//返回select语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证quit语句是否有效
string Interpreter::quit_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取第二个单词
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，返回quit语句的内部形式
	if(temp==";")
		SQL="50";
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证execfile语句是否有效
string Interpreter::execfile_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取文件名
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for execfile statement!"<<endl;
		SQL="99";
	}
	else
		SQL="60"+temp;
	//返回execfile语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证use database语句是否有效
string Interpreter::use_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取数据库名
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: syntax error for use database statement!"<<endl;
		SQL="99";
	}
	else
		SQL="70"+temp;
	//返回use database语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter::interpreter(string statement)
{
	string SQL;
	string temp;
	string sql;

	if(statement == "" )
	{
		SQL="99";
		return SQL;
	}
	if(statement.find_first_of(";") == -1)
	{
		SQL="98";
		return SQL;
	}
	statement = " " + statement;
	statement.insert(statement.find_first_of(";"), " ");

	int start=0,end;
	if(statement.empty())
		//获取用户输入
		SQL=read_input();
	else
		SQL=statement;
	//获取输入的第一个单词
	while(SQL.at(start)==' ')
		start++;
	end=SQL.find(' ',start);
	temp.assign(SQL,start,end-start);
	start=end+1;
	change(temp);
	//cout<<temp<<endl;
	//cout<<SQL<<endl;
	//若无输入，打印出错信息
	if(temp.empty())
	{
		cout<<"syntax error: empty statement!"<<endl;
		SQL="99";
	}
	//若为create语句
	else if(temp=="create")
		SQL=create_clause(SQL,start);
	//若为drop语句
	else if(temp=="drop")
		SQL=drop_clause(SQL,start);
	//若为select语句
	else if(temp=="select")
		SQL=select_clause(SQL,start);
	//若为insert语句
	else if(temp=="insert")
		SQL=insert_clause(SQL,start);
	//若为delete语句
	else if(temp=="delete")
		SQL=delete_clause(SQL,start);
	//若为execfile语句
	else if(temp=="execfile")
		SQL=execfile_clause(SQL,start);
	//若为quit语句
	else if(temp=="quit")
		SQL=quit_clause(SQL,start);
	//若为use语句
	else if(temp=="use")
		SQL=use_clause(SQL,start);
	//若为非法语句
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回输入语句的内部形式
	return SQL;
}


//将输入大写转化为小写
void Interpreter::change(string &SQL){
	for(unsigned int i=0;i < SQL.size();i++)
	{
		if(SQL[i]>='A'&&SQL[i]<='Z')
		{
			SQL[i] += 32;
			continue;
		 }
	}
};


