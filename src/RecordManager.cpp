/*
 * RecordManager.cpp
 *
 *  Created on: 2015年11月1日
 *      Author: yangyuming
 */
#include "RecordManager.h"

#include "BufferManager.h"
#include "Declaration.h"

bool emptyRecord(string record) {

	bool empty = true;
	for (int i = 0; i < record.size(); i++)
		if (record[i] != '#')
			empty = false;
	return empty;

}

bool compare(string s1, string s2, int type, string op) {
	int compare;
	switch (type) {
	case 0:
		compare = compareInt(s1, s2);
		break;
	case 2:
		compare = compareStr(s1, s2);
		break;
	case 1:
		compare = compareFloat(s1, s2);
		break;
	default:
		compare = -100;
		break;
	}

	if (op == "<")
		return (compare == -1);
	if (op == "<=")
		return (compare != 1);
	if (op == ">")
		return (compare == 1);
	if (op == ">=")
		return (compare != -1);
	if (op == "=")
		return (compare == 0);
	if (op == "!=")
		return (compare != 0);

	return false;
}

void getData(string DB_Name, TableInfo table, Data& data, set<int> offsetSet,
		vector<Condition>& conds) {
	const string filename = DB_Name + "_" + table.name + ".table";
	int recordLength = table.totalLength;

	set<int>::iterator offsetIter;
	int blockNum = 0;
	int blockOffset = 0;

	int recordAmount = 4096 / recordLength;

	for (offsetIter = offsetSet.begin(); offsetIter != offsetSet.end();
			offsetIter++) {

		int offset = *offsetIter;
		blockNum = offset / recordAmount;
		blockOffset = offset % recordAmount;
		blockInfo* block = readBlock(DB_Name, filename, blockNum, 0);
		string info = block->cBlock;
		info = info.substr(0.4096);

		if (!emptyRecord(
				info.substr(recordLength * blockOffset, recordLength))) {
			Record tmpRecord;
			tmpRecord.blockNum = blockNum;
			tmpRecord.offset = blockOffset;
			int start = 0;
			for (int k = 0; k < table.attrNum; k++) {
				int attrLength = table.attrs[k].length;
				string tmpColumn = info.substr(
						recordLength * blockOffset + start, attrLength);
				tmpRecord.columns.push_back(tmpColumn);
				start += attrLength;
			}
			bool flag = true;
			for (int eveCond = 0; eveCond < conds.size(); eveCond++) {

				int k = 0;
				while (table.attrs[k].name != conds[eveCond].attr)
					k++;
				bool isSatisfy = compare(tmpRecord.columns[k],
						conds[eveCond].data, table.attrs[k].type,
						conds[eveCond].compare);

				if (!isSatisfy)
					flag = false;

			}
			if (flag)
				data.records.push_back(tmpRecord);
		}
	}

}

void getData(string DB_Name, TableInfo table, Data& data,
		vector<Condition>& conds) {
	const string filename = DB_Name + "_" + table.name + ".table";
	int recordLength = table.totalLength;

	int blockAmount = 2048;
	for (int i = 0; i < blockAmount; i++) {
		blockInfo* block = readBlock(DB_Name, filename, i, 0);
		string info = block->cBlock;
		info = info.substr(0, 4096);

		if (info.size() == 0)
			break;

		int recordAmount = info.length() / recordLength;

		for (int j = 0; j < recordAmount; j++) {
			if (!emptyRecord(info.substr(recordLength * j, recordLength))) {
				Record tmpRecord;
				tmpRecord.blockNum = i;
				tmpRecord.offset = j;
				int start = 0;
				for (int k = 0; k < table.attrNum; k++) {
					int attrLength = table.attrs[k].length;
					string tmpColumn = info.substr(recordLength * j + start,
							attrLength);
					tmpRecord.columns.push_back(tmpColumn);
					start += attrLength;
				}
				bool flag = true;
				for (int eveCond = 0; eveCond < conds.size(); eveCond++) {
					int k = 0;
					while (table.attrs[k].name != conds[eveCond].attr)
						k++;
					bool isSatisfy = compare(tmpRecord.columns[k],
							conds[eveCond].data, table.attrs[k].type,
							conds[eveCond].compare);

					if (!isSatisfy)
						flag = false;

				}
				if (flag)
					data.records.push_back(tmpRecord);
			}
		}
	}
}

void select(TableInfo table, Data dataIn, Data &dataOut,
		vector<string>& columns) {

	Data data = dataIn;
	if (columns.size() == 0) // { //显示记录所有属性
		for (int i = 0; i < table.attrs.size(); i++)
			columns.push_back(table.attrs[i].name);

	for (int i = 0; i < data.records.size(); i++) {
		Record tmpRecord;
		int colNum = 0;
		for (int k = 0; k < table.attrNum; ++k) {
			if (table.attrs[k].name == columns[colNum]) {
				colNum++;
				tmpRecord.blockNum = data.records[i].blockNum;
				tmpRecord.offset = data.records[i].offset;
				tmpRecord.columns.push_back(data.records[i].columns[k]);
			}
		}
		dataOut.records.push_back(tmpRecord);
	}

}

void coutPrint(vector<string>& columns, Data& dataOut, int length) {
	for (int i = 0; i < length; i++)
		cout << "－";
	cout << endl;

	for (int i = 0; i < columns.size(); ++i)
		cout << "| " << columns[i] << '\t';

	cout << "|" << endl;
	for (int i = 0; i < dataOut.records.size(); i++) {
		for (int j = 0; j < dataOut.records[i].columns.size(); j++) {
			string s = dataOut.records[i].columns[j];
			while (s[0] == '0' and s.size() > 1)
				s = s.substr(1, s.length() - 1);
			cout << "| " << s << '\t';
		}
		cout << "|" << endl;
	}
	for (int i = 0; i < length; i++)
		cout << "－";
	cout << endl;
}

bool createIndex(string DB_Name, TableInfo table, string Attr_Name,
		string indexName) {
	Data data;
	vector<Condition> conds;
	getData(DB_Name, table, data, conds);

	int columnNum = -1;
	for (int i = 0; i < table.attrs.size(); i++)
		if (table.attrs[i].name == Attr_Name) {
			columnNum = i;
			if (!table.attrs[i].isUnique) {
				cout << "Create Index :  The attribute is not unique!!!"
						<< endl;
				return false;
			}
			break;
		}

	if (columnNum == -1) {
		cout << "Create Index :  Attribute name not exist!!!" << endl;
		return false;
	}

	int recordLength = table.totalLength;
	int recordAmount = 4096 / recordLength;

	for (int j = 0; j < data.records.size(); j++) {
		Record tmpRecord = data.records[j];
		string value = tmpRecord.columns[columnNum];
		int offset = tmpRecord.blockNum * recordAmount + tmpRecord.offset;

		index_info inform;
		inform.index_name = indexName;
		inform.offset = offset;
		inform.type = table.attrs[columnNum].type;
		inform.length = table.attrs[columnNum].length;
		inform.value = value;

		insert_one(DB_Name, table.name, inform);
	}
}

Data selectData(string DB_Name, vector<string>& columns, TableInfo table,
		vector<Condition>& conds) {
	Data dataIn, dataOut;
	vector<Condition>::iterator conditionIter;
	vector<AttrInfo>::iterator attrIter;
	set<int> offsetSet;
	for (conditionIter = conds.begin(); conditionIter != conds.end();
			conditionIter++) {
		for (attrIter = table.attrs.begin(); attrIter != table.attrs.end();
				attrIter++) {
			AttrInfo attr = *attrIter;
			Condition cond = *conditionIter;
			if (attr.name == cond.attr) {
				if (attr.indexName != "NULL") {
					index_info inform;
					inform.index_name = attr.indexName;
					inform.length = attr.length;
					inform.type = attr.type;
					inform.value = cond.data;
					set<int> resultSet;

					if (cond.compare == "!=") {
						search_one(DB_Name, table.name, inform);
						if (inform.offset >= 0
								&& offsetSet.count(inform.offset))
							offsetSet.erase(offsetSet.find(inform.offset));
					}

					if (cond.compare == "=") {
						search_one(DB_Name, table.name, inform);
						//cout << inform.offset << endl;
						if (inform.offset >= 0)
							offsetSet.insert(inform.offset);
					}

					if (cond.compare == ">") {
						resultSet = search_many(DB_Name, table.name, 1, inform);
						offsetSet.insert(resultSet.begin(), resultSet.end());
					}

					if (cond.compare == ">=") {
						resultSet = search_many(DB_Name, table.name, 2, inform);
						offsetSet.insert(resultSet.begin(), resultSet.end());
					}

					if (cond.compare == "<") {
						resultSet = search_many(DB_Name, table.name, 3, inform);
						//cout << resultSet.size() << endl;
						offsetSet.insert(resultSet.begin(), resultSet.end());
					}

					if (cond.compare == "<=") {
						resultSet = search_many(DB_Name, table.name, 4, inform);
						offsetSet.insert(resultSet.begin(), resultSet.end());
					}

				}

				break;
			}
		}
	}

	if (!offsetSet.empty())
		getData(DB_Name, table, dataIn, offsetSet, conds);
	else
		getData(DB_Name, table, dataIn, conds);

	select(table, dataIn, dataOut, columns);

	return dataOut;
}

void selectRecord(string DB_Name, vector<string>& columns, TableInfo table,
		vector<Condition>& conds) {

	Data dataOut = selectData(DB_Name, columns, table, conds);
	coutPrint(columns, dataOut, table.totalLength);
}

int NotFullBlock(string DB_Name, TableInfo table, int& blockOffset) {
	const string filename = DB_Name + "_" + table.name + ".table";
	int recordLength = table.totalLength;
	blockInfo* block;
	string info;
	for (int i = 0; i < 2048; i++) {
		block = readBlock(DB_Name, filename, i, 0);
		info = block->cBlock;

		for (int j = 0; j < 4096 / recordLength; j++)
			if (emptyRecord(info.substr(recordLength * j, recordLength))) {
				blockOffset = j;
				return i;
			}
		if (info.size() + recordLength < 4096) {
			blockOffset = -1;
			return i;
		}
	}
	return -1;
}

bool insertRecord(string DB_Name, TableInfo table, Record insertedRecord) {

	const string filename = DB_Name + "_" + table.name + ".table";

	string recordString;
	int recordLength = table.totalLength;
	int recordAmount = 4096 / recordLength;

	int blockOffset;
	int blockNum = NotFullBlock(DB_Name, table, blockOffset);

	if (blockNum == -1)
		return false;

	for (int i = 0; i < table.attrs.size(); i++)
		if (table.attrs[i].isUnique) {
			vector<string> columns;
			vector<Condition> conds;
			Condition cond;
			cond.attr = table.attrs[i].name;
			cond.compare = "=";
			cond.data = insertedRecord.columns[i];
			conds.push_back(cond);
			Data dataOut = selectData(DB_Name, columns, table, conds);
			if (dataOut.records.size() > 0) {
				cout << "The inserted attribute " << table.attrs[i].name
						<< " is not unique!!!" << endl;
				return false;
			}
		}

	for (int j = 0; j < insertedRecord.columns.size(); j++) {
		if (table.attrs[j].length != insertedRecord.columns[j].length()) {
			cout << "Attribute length can't fit table info!!!" << endl;
			return false;
		}
		if (table.attrs[j].indexName != "NULL" || table.attrs[j].isPrimeryKey) {
			index_info inform;
			inform.index_name = table.attrs[j].indexName;
			inform.type = table.attrs[j].type;
			inform.length = table.attrs[j].length;
			inform.value = insertedRecord.columns[j];
			inform.offset = recordAmount * blockNum + blockOffset;
			insert_one(DB_Name, table.name, inform);
		}
		recordString += insertedRecord.columns[j];
	}

	blockInfo* block = readBlock(DB_Name, filename, blockNum, 0);
	string info = block->cBlock;

	if (blockOffset == -1)
		info.insert(info.length(), recordString);
	else
		info.replace(recordLength * blockOffset, recordLength, recordString);

	updateNode(block, info);
	cout << "Insert successfully" << endl;
	return true;
}

string emptyRecord(int length) {
	string s;
	for (int i = 0; i < length; i++)
		s += "#";
	return s;
}

bool deleteRecord(string DB_Name, TableInfo table, vector<Condition>& conds) {

	const string filename = DB_Name + "_" + table.name + ".table";
	int recordLength = table.totalLength;
	//int recordAmount = 4096 / recordLength;

	vector<string> columns;
	Data dataOut = selectData(DB_Name, columns, table, conds);

	if (dataOut.records.size() == 0) {
		cout << "Nothing to delete!!!" << endl;
		return true;
	}
	for (int i = 0; i < dataOut.records.size(); i++) {
		Record record = dataOut.records[i];
		int blobkNum = record.blockNum;
		int blockOffset = record.offset;
		blockInfo* block = readBlock(DB_Name, filename, blobkNum, 0);
		string info = block->cBlock;
		info.replace(recordLength * blockOffset, recordLength,
				emptyRecord(recordLength));
		updateNode(block, info);
		for (int j = 0; j < record.columns.size(); j++)
			if (table.attrs[j].indexName != "NULL") {
				AttrInfo attr = table.attrs[j];
				index_info inform;
				inform.index_name = attr.indexName;
				inform.length = attr.length;
				inform.type = attr.type;
				inform.value = record.columns[j];
				delete_one(DB_Name, table.name, inform);
			}
	}
	cout << "delete successfully" << endl;
	return true;
}
