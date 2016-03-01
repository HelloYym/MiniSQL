/*
 * CatalogManager.cpp
 *
 *  Created on: 2015年11月6日
 *      Author: yangyuming
 */
#include "CatalogManager.h"
#include "Declaration.h"

bool CatalogManager::readCatalog(string a_name) {
	dbName = a_name;
	string filename = a_name + ".catalog";
	fstream fin(filename.c_str(), ios::in);
	if (!fin) {
		cout << "Database " << a_name << " does not Exist!" << endl;
		return false;
	}
	fin >> tableNum;
	for (int i = 0; i < tableNum; i++) {
		TableInfo temp_table;
		fin >> temp_table.name;
		fin >> temp_table.attrNum;
		fin >> temp_table.totalLength;
		for (int j = 0; j < temp_table.attrNum; j++) {
			AttrInfo temp_attr;
			string a_type;
			fin >> temp_attr.name;
			fin >> a_type;
			if (a_type == "int")
				temp_attr.type = INT;
			else if (a_type == "float")
				temp_attr.type = FLOAT;
			else
				temp_attr.type = CHAR;
			fin >> temp_attr.length;
			fin >> temp_attr.isPrimeryKey;
			fin >> temp_attr.isUnique;
			fin >> temp_attr.indexName;
			if (temp_attr.indexName != "NULL")
				indexs.push_back(
						Index(temp_attr.indexName, temp_table.name,
								temp_attr.name));
			temp_table.attrs.push_back(temp_attr);
		}
		tables.push_back(temp_table);
	}
	indexNum = indexs.size();
	fin.close();
	return true;
}

bool CatalogManager::writeCatalog() {
	string filename = dbName + ".catalog";
	fstream fout(filename.c_str(), ios::out);
	if (!fout) {
		cout << "Database " << dbName << " does not Exist!" << endl;
		return false;
	}
	fout << tableNum << endl;
	for (int i = 0; i < tableNum; i++) {
		fout << tables[i].name << " ";
		fout << tables[i].attrNum << " ";
		fout << tables[i].totalLength << endl;
		for (int j = 0; j < tables[i].attrNum; j++) {
			fout << tables[i].attrs[j].name << " ";
			string a_type;
			if (tables[i].attrs[j].type == INT)
				a_type.append("int");
			else if (tables[i].attrs[j].type == FLOAT)
				a_type.append("float");
			else {
				fout << "char(";
				fout << tables[i].attrs[j].length;
				a_type.append(")");
			}
			fout << a_type << " ";
			fout << tables[i].attrs[j].length << " ";
			fout << tables[i].attrs[j].isPrimeryKey << " ";
			fout << tables[i].attrs[j].isUnique << " ";
			fout << tables[i].attrs[j].indexName << endl;
		}
	}
	fout.close();
	return true;
}

bool CatalogManager::isDBinUse() {
	if (dbName.empty()) {
		cout << "No database is being used!" << endl;
		return false;
	} else
		return true;
}

bool CatalogManager::Exist_Database(string DB_Name) {
	string filename = DB_Name + ".catalog";
	fstream file(filename.c_str(), ios::in);
	if (!file)
		return false;
	file.close();
	return true;
}

bool CatalogManager::Exist_Table(string Table_Name, int &position) {
	for (int i = 0; i < tableNum; i++)
		if (tables[i].name == Table_Name) {
			position = i;
			return true;
		}
	return false;
}

bool CatalogManager::Exist_Attr(string Attr_Name, int &position, int &tableP) {
	for (int i = 0; i < tables[tableP].attrNum; i++)
		if (tables[tableP].attrs[i].name == Attr_Name) {
			position = i;
			return true;
		}
	return false;
}

bool CatalogManager::Exist_Index(string Index_Name, int &position) {
	for (int i = 0; i < indexNum; i++)
		if (indexs[i].name == Index_Name) {
			position = i;
			return true;
		}
	return false;
}

bool CatalogManager::Check_Type(int type, int length, string &data,
		string attr) {
	if (type == INT || type == FLOAT) {
		int countdot = 0;
		for (unsigned int j = 0; j < data.size(); j++) {
			if (data.at(j) == '.')
				countdot += 1;
			else if (data.at(j) < '0' || data.at(j) > '9') {
				cout << "The data typed does not match attribute" << attr << "!"
						<< endl;
				return false;
			}
		}
		if (countdot > 1 || (type == INT && (countdot > 0 || data.size() > 4))
				|| data.size() > 10) {
			cout << "The data typed does not match attribute" << attr << "!"
					<< endl;
			return false;
		}
	}
	string temp;
	for (unsigned int i = 0; i < length - data.size(); i++)
		temp += "0";
	data = temp + data;
	return true;
}

bool CatalogManager::Use_Database(string DB_Name) {
	if (!dbName.empty())
		writeCatalog();
	if (!Exist_Database(DB_Name)) {
		cout << "Database " << DB_Name << " does not Exist!" << endl;
		return false;
	}
	if (dbName == DB_Name) {
		cout << "Database " << DB_Name << " is already in use!" << endl;
		return true;
	}
	if (readCatalog(DB_Name)) {
		cout << "Database " << DB_Name << " is now in use!" << endl;
		return true;
	}
	cout << "Unknow Error!" << endl;

	return false;
}

bool CatalogManager::Exit_Database() {
	if (isDBinUse()) {
		writeCatalog();
		dbName = "";
		cout << "Database exited successfully" << endl;
		return true;
	}
	return false;
}

void CatalogManager::Create_Database(string DB_Name) {
	if (Exist_Database(DB_Name)) {
		cout << "Database " << DB_Name << " has already existed!" << endl;
		return;
	}
	string filename = DB_Name + ".catalog";
	ofstream fout(filename.c_str(), ios::out);
	fout << 0 << endl;
	fout.close();
	string instruction = "mkdir " + DB_Name;
	system(instruction.c_str());
	cout << "Database " << DB_Name << " created successfully" << endl;
	return;
}

void CatalogManager::Create_Table(string Table_Name, vector<AttrInfo> newAttrs,
		vector<string> primaryKeys) {
	if (!isDBinUse())
		return;
	string filename = dbName + "_" + Table_Name + ".table";
	string path = dbName + "//" + filename;
	fstream file;
	file.open(path.c_str(), ios::in);
	if (file) {
		file.close();
		cout << "Table " << Table_Name << " has already existed!" << endl;
		return;
	}
	file.open(path.c_str(), ios::out);
	file.seekg(4096 * 2049);
	file << "END";
	file.close();
	TableInfo table = TableInfo(Table_Name, newAttrs);
	tables.push_back(table);
	tableNum += 1;
	for (unsigned int i = 0; i < primaryKeys.size(); i++) {
		int tableP, attrP;
		//string indexName = Table_Name + "_" + primaryKeys[i];
		Create_Index(primaryKeys[i], Table_Name, primaryKeys[i], tableP, attrP);
	}
	cout << "Table " << Table_Name << " created successfully" << endl;
	writeCatalog();
	return;
}

void CatalogManager::Create_Index(string Index_Name, string Table_Name,
		string Attr_Name, int &tableP, int &attrP) {
	if (!isDBinUse())
		return;
	int indexP;
	if (!Exist_Table(Table_Name, tableP)) {
		cout << "Table " << Table_Name << " does not exist!" << endl;
		return;
	}
	if (!Exist_Attr(Attr_Name, attrP, tableP)) {
		cout << "Attribute " << Attr_Name << " does not exist!" << endl;
		return;
	}
	if (!tables[tableP].attrs[attrP].isUnique) {
		cout << "Attribute " << Attr_Name
				<< " is not unique, index can not be create!" << endl;
		return;
	}
	if (Exist_Index(Index_Name, indexP)) {
		cout << "Index " << Index_Name << " has already existed!" << endl;
		return;
	}
	if (tables[tableP].attrs[attrP].indexName != "NULL") {
		cout << "Attribute " << Attr_Name << " has already had index!" << endl;
		return;
	}
	tables[tableP].attrs[attrP].setIndex(Index_Name);
	string filename = dbName + "_" + tables[tableP].name + "_" + Index_Name
			+ ".index";
	string path = dbName + "//" + filename;
	fstream file;
	file.open(path.c_str(), ios::out);
	file.seekg(4096 * 2049);
	file << "END";
	file.close();
	Index index = Index(Index_Name, Table_Name, Attr_Name);
	indexs.push_back(index);
	indexNum += 1;
	cout << "Index " << Index_Name << " created successfully" << endl;
	writeCatalog();
	return;
}
;

void CatalogManager::Drop_Database(string DB_Name) {
	if (!Exist_Database(DB_Name)) {
		cout << "Database " << DB_Name << " does not Exist!" << endl;
		return;
	}
	string filename = DB_Name + ".catalog";
	remove(filename.c_str());
	string instruction = "rm -rf " + DB_Name;
	system(instruction.c_str());
	cout << "Database " << DB_Name << " dropped successfully" << endl;
	return;
}

void CatalogManager::Drop_Table(string Table_Name) {
	if (!isDBinUse())
		return;
	int tableP;
	if (!Exist_Table(Table_Name, tableP)) {
		cout << "Table " << Table_Name << " does not exist!" << endl;
		return;
	}
	for (int attrP = 0; attrP < tables[tableP].attrNum; attrP++)
		if (tables[tableP].attrs[attrP].indexName != "NULL")
			Drop_Index(tables[tableP].attrs[attrP].indexName);
	string filename = dbName + "_" + Table_Name + ".table";
	string path = dbName + "//" + filename;
	remove(path.c_str());
	tables.erase(tables.begin() + tableP);
	tableNum -= 1;
	cout << "Table " << Table_Name << " dropped successfully" << endl;
	writeCatalog();
	return;
}

void CatalogManager::Drop_Index(string Index_Name) {
	if (!isDBinUse())
		return;
	int tableP, attrP, indexP;
	if (!Exist_Index(Index_Name, indexP)) {
		cout << "Index " << Index_Name << " does not exist!" << endl;
		return;
	}
	Exist_Table(indexs[indexP].tableName, tableP);
	Exist_Attr(indexs[indexP].attrName, attrP, tableP);
	tables[tableP].attrs[attrP].indexName = "NULL";
	string filename = dbName + "_" + tables[tableP].name + "_" + Index_Name
			+ ".index";
	string path = dbName + "//" + filename;
	remove(path.c_str());
	indexs.erase(indexs.begin() + indexP);
	indexNum -= 1;
	cout << "Index " << Index_Name << " dropped successfully" << endl;
	writeCatalog();
	return;
}

bool CatalogManager::Select(string Table_Name, vector<string> Attrs,
		vector<Condition> &Condition, vector<int> &IndexP, int &tableP) {
	if (!isDBinUse())
		return false;
	int attrP, indexP;
	if (!Exist_Table(Table_Name, tableP)) {
		cout << "Table " << Table_Name << " does not exist!" << endl;
		return false;
	}
	//�ж������Ƿ����
	if (!Attrs.empty()) {
		for (unsigned int i = 0; i < Attrs.size(); i++) {
			if (!Exist_Attr(Attrs[i], attrP, tableP)) {
				cout << "Attribute " << Attrs[i] << " does not exist!" << endl;
				return false;
			}
		}
	}
	//�ж������Ƿ���ȷ
	if (!Condition.empty()) {
		for (unsigned int i = 0; i < Condition.size(); i++) {
			//�ж������е������Ƿ����
			if (!Exist_Attr(Condition[i].attr, attrP, tableP)) {
				cout << "Attribute " << Condition[i].attr << " does not exist!"
						<< endl;
				return false;
			}
			//�ж����������ݸ�ʽ�Ƿ�����и�ʽ
			if (!Check_Type(tables[tableP].attrs[attrP].type,
					tables[tableP].attrs[attrP].length, Condition[i].data,
					Condition[i].attr))
				return false;
			//�ж��Ƿ�����������
			if (Exist_Index(tables[tableP].attrs[attrP].indexName, indexP))
				IndexP.push_back(indexP);
		}
	}
	return true;
}

bool CatalogManager::Insert(string Table_Name, int &tableP,
		vector<string> &Data) {
	if (!isDBinUse())
		return false;
	if (!Exist_Table(Table_Name, tableP)) {
		cout << "Table " << Table_Name << " does not exist!" << endl;
		return false;
	}
	if (tables[tableP].attrNum != Data.size()) {
		cout << tables[tableP].attrNum << "       " << Data.size() << endl;
		cout << "The amount of data inputted does not match!" << endl;
		return false;
	}
	for (int i = 0; i < tables[tableP].attrNum; i++)
		if (!Check_Type(tables[tableP].attrs[i].type,
				tables[tableP].attrs[i].length, Data[i],
				tables[tableP].attrs[i].name))
			return false;
	return true;
}

bool CatalogManager::Delete(string Table_Name, vector<Condition> &Condition,
		vector<int> &IndexP, int &tableP) {
	if (!isDBinUse())
		return false;
	int attrP, indexP;
	if (!Exist_Table(Table_Name, tableP)) {
		cout << "Table " << Table_Name << " does not exist!" << endl;
		return false;
	}
	if (!Condition.empty()) {
		for (unsigned int i = 0; i < Condition.size(); i++) {
			//�ж������е������Ƿ����
			if (!Exist_Attr(Condition[i].attr, attrP, tableP)) {
				cout << "Attribute " << Condition[i].attr << " does not exist!"
						<< endl;
				return false;
			}
			if (!Check_Type(tables[tableP].attrs[attrP].type,
					tables[tableP].attrs[attrP].length, Condition[i].data,
					Condition[i].attr))
				return false;
			//�ж��Ƿ�����������
			if (Exist_Index(tables[tableP].attrs[attrP].indexName, indexP))
				IndexP.push_back(indexP);
		}
	}
	return true;
}

bool CatalogManager::Delete_Table(string Table_Name) {
	if (!isDBinUse())
		return false;
	int tableP, attrP;
	if (!Exist_Table(Table_Name, tableP)) {
		cout << "Table " << Table_Name << " does not exist!" << endl;
		return false;
	}
	for (attrP = 0; attrP < tables[tableP].attrNum; attrP++)
		if (tables[tableP].attrs[attrP].indexName != "NULL") {
			int temp_indexP;
			Exist_Index(tables[tableP].attrs[attrP].indexName, temp_indexP);
			string filename = dbName + "_" + tables[tableP].name + "_"
					+ tables[tableP].attrs[attrP].indexName + ".index";
			string path = dbName + "//" + filename;
			fstream file;
			file.open(path.c_str(), ios::out);
			file.seekg(4096 * 2049);
			file << "END";
			file.close();
		}
	string filename = dbName + "_" + Table_Name + ".table";
	string path = dbName + "//" + filename;
	fstream file;
	file.open(path.c_str(), ios::out);
	file.seekg(4096 * 2049);
	file << "END";
	cout << "Table " << Table_Name << " deleted successfully" << endl;
	writeCatalog();
	return true;
}
