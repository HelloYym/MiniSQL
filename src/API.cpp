/*
 * API.cpp
 *
 *  Created on: 2015年11月6日
 *      Author: yangyuming
 */
#include "API.h"

bool API::Check_Finish() {
	return finish;
}

bool API::Exec_File(string File_Name) {
	fstream file(File_Name.c_str(), ios::in);
	if (!file) {
		cout << "File " << File_Name << " does not exited!" << endl;
		return false;
	}
	string temp;
	while (getline(file, temp)) {
		Interpreter It;
		inputline += temp;
		dealString();
	}
	file.close();
	return true;
}

void API::dealString() {
	Interpreter It;
	string str;
	fstream file("");
	if (inputline.length() == 0 || (int) inputline.at(inputline.length() - 1) == 13)
		inputline.pop_back();
	str = It.interpreter(inputline);
	string type = str.substr(0, 2);
	str.erase(0, 2);
	if (type != "98") {
		inputline = "";
	}
	if (type == "99")
		return;
	//create database
	else if (type == "00") {
		string DB_Name = str;
		Cm.Create_Database(DB_Name);
	}
	//create table
	else if (type == "01") {
		vector<AttrInfo> newAttrs;
		vector<string> primaryKeys;

		int s = str.find_first_of(",");
		string Table_Name = str.substr(0, s);
		str.erase(0, s + 1);
		while (str.size() > 1 && s != -1) {
			string attrName;
			int type;
			int length;
			bool isU;

			s = str.find_first_of(" ");
			attrName = str.substr(0, s);
			str.erase(0, s + 1);

			int s0 = str.find_first_of(",");
			int s1 = str.find_first_of(" ");
			if (s1 == -1)
				s = s0;
			else if (s0 == -1)
				s = s1;
			else
				s = min(s0, s1);
			string t = str.substr(0, s);
			str.erase(0, s + 1);
			if (t == "#") {
				for (unsigned int i = 0; i < newAttrs.size(); i++)
					if (newAttrs[i].name == attrName) {
						newAttrs[i].setPrimeryKey();
						primaryKeys.push_back(attrName);
						break;
					}
			} else {

				if (t == "+") {
					type = INT;
					length = 4;
				} else if (t == "-") {
					type = FLOAT;
					length = 10;
				} else {
					type = CHAR;
					sscanf(t.c_str(), "%d", &length);
				}
				s = str.find_first_of(",");
				t = str.substr(0, s);
				str.erase(0, s + 1);
				if (t == "0")
					isU = false;
				else if (t == "1")
					isU = true;

				newAttrs.push_back(
						AttrInfo(attrName, type, length, false, isU));
			}
		}
		Cm.Create_Table(Table_Name, newAttrs, primaryKeys);
		/*for(unsigned int i = 0; i < primaryKeys.size(); i++)
		 {
		 string indexName = Table_Name + "_" + primaryKeys[i];
		 createIndex(Cm.getdbName(), Cm.getTable(Cm.gettableNum()-1), primaryKeys[i], indexName);
		 }*/
	}
	//create index
	else if (type == "02") {
		string Index_Name, Table_Name, Attr_Name;
		int tableP, attrP;

		int s = str.find_first_of(" ");
		Index_Name = str.substr(0, s);
		str.erase(0, s + 1);
		s = str.find_first_of(" ");
		Table_Name = str.substr(0, s);
		str.erase(0, s + 1);
		Attr_Name = str;

		Cm.Create_Index(Index_Name, Table_Name, Attr_Name, tableP, attrP);
		createIndex(Cm.getdbName(), Cm.getTable(tableP), Attr_Name, Index_Name);
	}
	//drop database
	else if (type == "10") {
		string DB_Name = str;
		Cm.Drop_Database(DB_Name);
	}
	//drop table
	else if (type == "11") {
		string Table_Name = str;
		Cm.Drop_Table(Table_Name);
	}
	//drop index
	else if (type == "12") {
		string Index_Name = str;
		Cm.Drop_Index(Index_Name);
	}
	//select
	else if (type == "20") {
		vector<Condition> Conditions;
		string Table_Name;
		int tableP;
		vector<string> Attrs;
		vector<int> IndexP;

		int s = str.find_first_of(",");
		while (s != str.find_last_of(",")) {
			Attrs.push_back(str.substr(0, s));
			str.erase(0, s + 1);
			s = str.find_first_of(",");
		}
		s = str.find_first_of(" ");
		if (s == str.find_last_of(" ") && s != -1) {
			s = str.find_first_of(",");
			Attrs.push_back(str.substr(0, s));
			str.erase(0, s + 1);
		}
		s = str.find_first_of(" ");
		if (s < str.find_first_of(",")) {
			s = str.find_first_of(" ");
			Attrs.push_back(str.substr(0, s));
			str.erase(0, s + 1);
		}
		s = str.find_first_of(",");
		if (s == -1)
			Table_Name = str;
		else {
			Table_Name = str.substr(0, s);
			str.erase(0, s + 1);
			s = str.find_first_of(" ");
			while (s != -1) {
				string attr = str.substr(0, s);
				str.erase(0, s + 1);
				if (attr != "and" && attr != "") {
					s = str.find_first_of(" ");
					string compare = str.substr(0, s);
					if (compare == "<>" || compare == "><")
						compare = "!=";
					str.erase(0, s + 1);
					s = str.find_first_of(" ");
					string data = str.substr(0, s);
					str.erase(0, s + 1);
					if (data[0] == '\'')
						data = data.substr(1, data.length() - 2);
					Conditions.push_back(Condition(attr, compare, data));
				}
				s = str.find_first_of(" ");
			}
		}
		if (Cm.Select(Table_Name, Attrs, Conditions, IndexP, tableP))
			selectRecord(Cm.getdbName(), Attrs, Cm.getTable(tableP),
					Conditions);
	}
	//insert��δ�꡿
	else if (type == "30") {
		int tableP;
		vector<string> Data;

		int s = str.find_first_of(" ");
		string Table_Name = str.substr(0, s);
		str.erase(0, s + 1);
		s = str.find_first_of(" ");
		while (s != -1) {
			if (str[0] != '\'')
				Data.push_back(str.substr(0, s));
			else
				Data.push_back(str.substr(1, s - 2));
			str.erase(0, s + 1);
			s = str.find_first_of(" ");
		}
		if (str[0] == '\'')
			str = str.substr(1, str.size()-2);
		Data.push_back(str);
		if (Cm.Insert(Table_Name, tableP, Data)) {
			Record insertedRecord;
			insertedRecord.columns = Data;
			insertRecord(Cm.getdbName(), Cm.getTable(tableP), insertedRecord);
		}
	}
	//delete
	else if (type == "40") {
		vector<Condition> Conditions;
		int tableP;
		vector<int> IndexP;

		int s = str.find_first_of(",");
		if (s == -1) {
			string Table_Name = str;
			Cm.Delete_Table(Table_Name);
		} else {
			string Table_Name = str.substr(0, s);
			str.erase(0, s + 1);
			s = str.find_first_of(" ");
			while (s != -1) {
				string attr = str.substr(0, s);
				str.erase(0, s + 1);
				if (attr != "and" && attr != "") {
					s = str.find_first_of(" ");
					string compare = str.substr(0, s);
					if (compare == "<>" || compare == "><")
						compare = "!=";
					str.erase(0, s + 1);
					s = str.find_first_of(" ");
					string data = str.substr(0, s);
					str.erase(0, s + 1);
					if (data[0] == '\'')
						data = data.substr(1, data.length() - 2);
					Conditions.push_back(Condition(attr, compare, data));
				}
				s = str.find_first_of(" ");
			}
			if (Cm.Delete(Table_Name, Conditions, IndexP, tableP))
				deleteRecord(Cm.getdbName(), Cm.getTable(tableP), Conditions);
		}
	}
	//quit
	else if (type == "50") {
		if (Cm.getdbName() != ""){
			quitProg(Cm.getdbName());
		}
		finish = true;
	}
	//execfile
	else if (type == "60") {
		string File_Name = str;
		Exec_File(File_Name);
	}
	//use database
	else if (type == "70") {
		string DB_Name = str;
		if (Cm.getdbName() != "")
			quitProg(Cm.getdbName());
		Cm.Use_Database(DB_Name);
	}
	return;
}
