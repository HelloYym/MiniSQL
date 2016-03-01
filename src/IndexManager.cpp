//============================================================================
// Name        : IndexManager.cpp
// Author      : Yym
// Version     :
// Copyright   : ZJU
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "IndexManager.h"
#include "BufferManager.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>


using namespace std;

string int_to_str(long value, int length) {
	string s;
	stringstream ss;
	ss << value;
	ss >> s;
	while (s.length() < length)
		s = '0' + s;
	return s;
}

int str_to_int(string str) {
	int num = 0;
	stringstream ss;
	ss << str;
	ss >> num;
	return num;
}

float str_to_float(string str) {
	float num = 0;
	stringstream ss;
	ss << str;
	ss >> num;
	return num;
}

int compareStr(string str1, string str2) {
	if (str1 < str2)
		return -1;
	else if (str1 == str2)
		return 0;
	else
		return 1;
}
int compareInt(string int1, string int2) {
	int n1 = str_to_int(int1);
	int n2 = str_to_int(int2);
	if (n1 < n2)
		return -1;
	else if (n1 > n2)
		return 1;
	else
		return 0;
}
int compareFloat(string float1, string float2) {
	float n1 = str_to_float(float1);
	float n2 = str_to_float(float2);
	if (n1 < n2)
		return -1;
	else if (n1 > n2)
		return 1;
	else
		return 0;
}



int search_one(string database, string table_name, struct index_info & inform) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";

	blockInfo* node = readBlock(database, filename, 0, 1);
	string info;
	int num = 0;				//每个节点的索引值个数
	int start;
	int length = getLength(inform);

	inform.offset = -1;

	/*索引文件为空*/
	if (node->cBlock[0] == '#')
		return -7;

	/*表中无记录*/
	if (node->cBlock[0] != '!' && node->cBlock[0] != '?')
		return -8;

	/*所查找的类型错误*/
	if (inform.type < 0 || inform.type > 2)
		return -1;

	/*不断地找下一个节点，直到到达叶子节点*/
	while (node->cBlock[0] != '!') {
		info = node->cBlock;
		num = getValueNum(info);	//得到当前Node的value值的个数num
		int blocknum = 0;						//下一个节点的块号

		/*  V<node.k[i]的最小值  */
		int location = getLocation(inform, info, 0);	//找到下一个节点
		blocknum = str_to_int(info.substr(location, 3));

		node = readBlock(database, filename, blocknum, 1);
	}

	/*找到叶子节点*/
	info = node->cBlock;
	if (info[0] == '!') {
		int location = getLocation(inform, info, 1);
		/*找到了包含value的叶子节点，设置记录号，返回叶子块号*/
		if (location > 5
				&& info.substr(location - length, length) == inform.value)
			inform.offset = str_to_int(info.substr(location - 5 - length, 5));
		return node->blockNum;
	} else
		return -3;	//异常

}

set<int> search_many(string database, string table_name, int type,
		struct index_info& inform) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";

	set<int> result;

	int start = -1, end = -1;

	int length = getLength(inform);

	/*调用search_leaf找到应该包含value值的叶子块*/
	int blocknum = search_one(database, table_name, inform);
	blockInfo *leaf = readBlock(database, filename, blocknum, 1);

	string info = leaf->cBlock;

	int compare;

	int st, en;
	if (type == 1 || type == 2)
		if (info[info.size() - 1] == '#')
			st = info.size() - length - 1;
		else
			st = info.size() - length - 3;
	else if (type == 3 || type == 4)
		st = 10;

	if (inform.type == 0)
		compare = compareInt(inform.value, info.substr(st, length));
	else if (inform.type == 1)
		compare = compareFloat(inform.value, info.substr(st, length));
	else if (inform.type == 2)
		compare = compareStr(inform.value, info.substr(st, length));

	/*找出所有 >inform.value的记录*/
	if (type == 1) {
		/*该节点最后一个值都<=value*/
		if (compare >= 0)
			/*start为next兄弟节点*/
			start = find_next_leaf_sibling(database, table_name, inform,
					leaf->blockNum);
		else
			start = leaf->blockNum;
	}
	/*找出所有>=inform.value的记录*/
	else if (type == 2) {
		/*最后一个值都小于value*/
		if (compare > 0)
			/*start为next兄弟节点*/
			start = find_next_leaf_sibling(database, table_name, inform,
					leaf->blockNum);
		else
			start = leaf->blockNum;
	}
	/*找出所有 <inform.value的记录*/
	else if (type == 3) {
		/*value<=第一个值*/
		if (compare <= 0)
			end = find_prev_leaf_sibling(database, table_name, inform,
					leaf->blockNum);
		else
			end = leaf->blockNum;
	}
	/*找出所有 <= inform.value的记录*/
	else if (type == 4) {

		/*第一个值都>value*/
		if (compare < 0)
			end = find_prev_leaf_sibling(database, table_name, inform,
					leaf->blockNum);
		else
			end = leaf->blockNum;
	}

	map<string, int> valueMap;
	int valueNum;
	blockInfo* node;
	int offset;
	string search_key;
	int l;
	if (type == 1 || type == 2) {
		while (start != -1) {
			node = readBlock(database, filename, start, 1);
			info = node->cBlock;
			valueNum = getValueNum(info);
			for (int i = 0; i < valueNum; i++) {
				l = (length + 5) * i + 1 + 4;
				offset = str_to_int(info.substr(l, 5));
				search_key = info.substr(l + 5, length);
				valueMap[search_key] = offset;
			}
			start = find_next_leaf_sibling(database, table_name, inform, start);
		}
	} else {
		while (end != -1) {
			node = readBlock(database, filename, end, 1);
			info = node->cBlock;
			valueNum = getValueNum(info);
			for (int i = 0; i < valueNum; i++) {
				l = (length + 5) * i + 1 + 4;
				offset = str_to_int(info.substr(l, 5));
				search_key = info.substr(l + 5, length);
				valueMap[search_key] = offset;
			}
			end = find_prev_leaf_sibling(database, table_name, inform, end);
		}
	}

	map<string, int>::iterator iter;

	for (iter = valueMap.begin(); iter != valueMap.end(); iter++) {
		switch (type) {
		case 1:
			if (iter->first > inform.value)
				result.insert(iter->second);
			break;
		case 2:
			if (iter->first >= inform.value)
				result.insert(iter->second);
			break;
		case 3:
			if (iter->first < inform.value)
				result.insert(iter->second);
			break;
		case 4:
			if (iter->first <= inform.value)
				result.insert(iter->second);
			break;
		default:
			break;
		}
	}

	return result;
}

int getLocation(index_info& inform, string info, int type) {//type = 1 leaf,   type = 0 internal
	int length = getLength(inform);
	int start = 0;
	int k = -1;
	int num = getValueNum(info);	//得到当前Node的value值的个数num

	//找到插入位置
	for (int i = 0; i < num; i++) {
		if (type == 1)
			start = (length + 5) * i + (1 + 4 + 5);
		else
			start = (length + 3) * i + (1 + 4 + 3);
		int compare;
		switch (inform.type) {
		case 0:
			compare = compareInt(inform.value, info.substr(start, length));
			break;
		case 1:
			compare = compareFloat(inform.value, info.substr(start, length));
			break;
		case 2:
			compare = compareStr(inform.value, info.substr(start, length));
			break;
		default:
			break;
		}
		/*找到了大于K的value*/
		if (compare < 0) {
			k = i;
			break;
		}
	}
	int location;
	if (k < 0)
		k = num;
	if (type == 1)
		location = 5 + (length + 5) * k;
	else
		location = 5 + (length + 3) * k;

	return location;
}

void insertInLeaf(string database, string table_name,
		struct index_info & inform, int blockNum) {

	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	blockInfo* node = readBlock(database, filename, blockNum, 1);
	string info = node->cBlock;
	int length = getLength(inform);
	int start = 0;
	int k = -1;

	int num = getValueNum(info);	//得到当前Node的value值的个数num

	int location = getLocation(inform, info, 1);	//找到插入位置

	//插入并更新节点中value个数
	info.insert(location, int_to_str(inform.offset, 5) + toLength(inform));

	info.erase(1, 4);

	info.insert(1, int_to_str(num + 1, 4));

	updateNode(node, info);
}

void insertInParent(string database, string table_name,
		struct index_info & inform, int N, string K, int N1) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	blockInfo* root = readBlock(database, filename, 0, 1);
	int length = getLength(inform);

	/*if N is the root of the tree*/
	if (root->blockNum == N) {
		/*create new node R containing N,K1,N1*/
		int new_block = get_new_freeblocknum(database, table_name, inform);
		blockInfo* R = readBlock(database, filename, new_block, 1);
		int r = R->blockNum;

		blockInfo* node = readBlock(database, filename, N, 1);
		string info = "?0001" + int_to_str(r, 3) + K + int_to_str(N1, 3);
		R->blockNum = N;
		node->blockNum = r;
		updateNode(R, info);
		info = node->cBlock;
		updateNode(node, info);
		return;

	} else {
		/*let P=parent(N)*/
		//blockInfo* N0 = readBlock(database, filename, N, 1);
		int p = findFather(database, table_name, inform, N);

		blockInfo* P = readBlock(database, filename, p, 1);
		string info = P->cBlock;

		int n = getN(inform, true);
		/*P has less than n-1 values*/
		if (getValueNum(info) < n - 1) {
			/*insert (K1,N1)in P just after N*/
			string temp = P->cBlock;
			int num = getValueNum(temp);
			int start, end;
			temp.replace(1, 4, int_to_str(num + 1, 4));

			for (int i = 0; i <= num; i++) {
				start = (length + 3) * i + 1 + 4;
				end = start + 3 - 1;
				/*找到了匹配Node的块号*/
				if (temp.substr(start, 3) == int_to_str(N, 3)) {
					string insert = K + int_to_str(N1, 3);
					temp.insert(end + 1, insert);
					updateNode(P, temp);
					return;
				}
			}
		}
		/*	分裂 P	*/
		else {

			/*create T that can hold P,(K1,N1)*/
			/*insert (K1,N1)in T just after N*/
			string tempT = P->cBlock;
			int num = getValueNum(tempT);
			int start, end;
			/*value个数加1*/
			string a = P->cBlock;
			int n = getValueNum(a) + 1;
			tempT.replace(1, 4, int_to_str(n, 4));

			for (int i = 0; i < num + 1; i++) {
				start = (length + 3) * i + 1 + 4;
				end = start + 3 - 1;
				string insert = K + int_to_str(N1, 3);
				/*找到了匹配的块号*/
				if (tempT.substr(start, 3) == int_to_str(N, 3)) {
					tempT.insert(end + 1, insert);
					break;
				}
			}

			/*copy 1 to n/2 from T to P*/
			int halfn = ceil((n - 1) / 2.0);
			string tempP = "?" + int_to_str(halfn, 4);
			tempP += tempT.substr(5, (length + 3) * halfn + 3);
			updateNode(P, tempP);

			/*let K11=T.K n/2*/
			string K11 = tempT.substr(5 + (length + 3) * halfn + 3, length);

			/*create node P1*/
			int p1 = get_new_freeblocknum(database, table_name, inform);
			blockInfo* P1 = readBlock(database, filename, p1, 1);

			/*copy n/2 +1 to n from T to P1*/
			string tempP1 = "?" + int_to_str(n - halfn - 1, 4);
			tempP1 += tempT.substr(5 + (length + 3) * (halfn + 1),
					(length + 3) * (n - halfn - 1) + 3);
			updateNode(P1, tempP1);

			insertInParent(database, table_name, inform, p, K11, p1);

			return;
		}
	}

}

void insert_one(string database, string table_name,
		struct index_info & inform) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";

	long offset = inform.offset;
	//找到应该包含value的节点
	int l = search_one(database, table_name, inform);

	/* 该索引值已存在 */
	if (inform.offset >= 0)
		return;

	inform.offset = offset;

	/*索引文件为空时，建立空块的链表*/
	if (l == -7 || l == -8) {
		int newBlock = get_new_freeblocknum(database, table_name, inform);//此处newblock一定是0
		blockInfo* root = readBlock(database, filename, newBlock, 1);
		string temp = "!0001";
		temp += int_to_str(inform.offset, 5);	//相应记录在表文件中的记录号
		temp += toLength(inform);
		temp += "#";
		updateNode(root, temp);
		return;
	}

	blockInfo* node = readBlock(database, filename, l, 1);
	string info = node->cBlock;

	int n = getN(inform, true);

	if (getValueNum(info) < n - 1)		//如果l包含搜索码小于n-1
		insertInLeaf(database, table_name, inform, node->blockNum);
	else {

		/*Copy L (=node here) to T*/
		int length = getLength(inform);

		int t = get_new_freeblocknum(database, table_name, inform);
		blockInfo* T = readBlock(database, filename, t, 1);
		char* oldBlock = T->cBlock;

		T->cBlock = (char*) malloc(4096 + length + 6);
		strcpy(T->cBlock, node->cBlock);

		/*create node L'(=L1 here)*/
		int l1 = get_new_freeblocknum(database, table_name, inform);
		blockInfo* L1 = readBlock(database, filename, l1, 1);

		/*store node.pn*/
		string pn;
		if (info[info.length() - 1] == '#')			//last leaf end with '#'
			pn = info.substr(info.length() - 1, 1);
		else
			pn = info.substr(info.length() - 3, 3);

		insertInLeaf(database, table_name, inform, t);

		/*erase all values and pointers from node*/
		/*copy 1 to n/2 from T to L(node)*/

		int halfn = ceil((getValueNum(info) + 1) / 2.0);	//向上取整

		string tempT = T->cBlock;
		string tempL;

		tempL = "!" + int_to_str(halfn, 4);
		tempL += tempT.substr(1 + 4, (5 + length) * halfn);

		/*set L(node).pn=L1*/
		tempL += int_to_str(l1, 3);

		node = readBlock(database, filename, l, 1);
		updateNode(node, tempL);

		int n = getValueNum(tempT);
		/*update L1节点value的个数*/
		string tempL1;
		tempL1 = "!" + int_to_str(n - halfn, 4);

		/*copy n/2 +1 to n from T to L1*/
		tempL1 += tempT.substr(1 + 4 + (length + 5) * halfn,
				(n - halfn) * (length + 5));
		/*set L1(node).pn=L.pn*/
		tempL1 += pn;

		updateNode(L1, tempL1);

		//delete T
		free(T->cBlock);
		T->cBlock = oldBlock;
		deleteBlock(database, table_name, inform, t);

		/*let K1 be the smallest value in L'(=L1 here)*/
		string K1 = tempL1.substr(1 + 4 + 5, length);

		insertInParent(database, table_name, inform, l, K1, l1);
	}
}

int findFather(string database, string table_name, index_info inform, int n) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	int length = getLength(inform);

	int father;
	blockInfo* node = readBlock(database, filename, n, 1);
	string info = node->cBlock;

	string old_value = inform.value;
	string value;
	if (info[0] == '?')
		value = info.substr(8, length);
	else if (info[0] == '!')
		value = info.substr(10, length);

	inform.value = value;

	blockInfo* root = readBlock(database, filename, 0, 1);
	blockInfo* Node = root;
	if (root->blockNum == n)
		return -1;

	info = Node->cBlock;
	while (Node->blockNum != n) {
		int location = getLocation(inform, info, 0);
		int blocknum = str_to_int(info.substr(location, 3));
		father = Node->blockNum;
		Node = readBlock(database, filename, blocknum, 1);
		info = Node->cBlock;
	}

	inform.value = old_value;
	return father;
}

void delete_one(string database, string table_name,
		struct index_info & inform) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	int l = search_one(database, table_name, inform);

	deleteEntry(database, table_name, inform, l, inform.value, l);
}

void deleteEntry(string database, string table_name, struct index_info & inform,
		int n, string K, int nod) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	int length = getLength(inform);

	blockInfo *Node = readBlock(database, filename, n, 1);
	string info = Node->cBlock;
	string originN = info;
	int num = getValueNum(info);
	string tempN = Node->cBlock;

	/*delete K,P from N*/
	tempN.replace(1, 4, int_to_str(num - 1, 4));
	int start;
	for (int i = 0; i < num; i++) {
		int compare;

		if (tempN[0] == '!')
			start = (length + 5) * i + (1 + 4 + 5);
		else
			start = (length + 3) * i + (1 + 4 + 3);

		switch (inform.type) {
		case 0:
			compare = compareInt(K, info.substr(start, length));
			break;
		case 1:
			compare = compareFloat(K, info.substr(start, length));
			break;
		case 2:
			compare = compareStr(K, info.substr(start, length));
			break;
		}

		if (compare == 0) {
			if (tempN[0] == '?') {
				if (tempN.substr(start - 3, 3) == int_to_str(nod, 3)) {
					tempN.erase(start - 3, length + 3);
					break;
				} else if (tempN.substr(start + length, 3)
						== int_to_str(nod, 3)) {
					tempN.erase(start, length + 3);
					break;
				}
			} else {
				tempN.erase(start - 5, length + 5);
				break;
			}
		}
		if (compare < 0 || i == num - 1) {
			if (tempN[0] == '?') {
				if (tempN.substr(start - 3, 3) == int_to_str(nod, 3)) {
					tempN.erase(start - 3 - length, length + 3);
					break;
				} else if (tempN.substr(start - 3 - 3 - length, 3)
						== int_to_str(nod, 3)) {
					tempN.erase(start - 3 - 3 - length, length + 3);
					break;
				}
			} else {
				//value not found
				return;
			}
		}
	}

	int father = findFather(database, table_name, inform, Node->blockNum);
	updateNode(Node, tempN);

	/*if N is root*/
	if (Node->blockNum == 0) {
		/* root has only one value*/

		if (getValueNum(originN) == 1) {
			/*make the child of N the root*/
			int child = str_to_int(tempN.substr(5, 3));
			blockInfo* Child = readBlock(database, filename, child, 1);
			/*交换块号*/
			Node->blockNum = Child->blockNum;
			Child->blockNum = 0;
			/*delete N*/
			deleteBlock(database, table_name, inform, Node->blockNum);
		} else {
			/*root has more than one value*/
		}
	} else {

		/* not root */

		string info = Node->cBlock;

		tempN = Node->cBlock;

		/*如果是叶子*/
		if (info[0] == '!') {

			int leaf_least = ceil((getN(inform, true) - 1) / 2.0);//(n - 1) / 2 向上取整

			/*if has too few values*/
			if (getValueNum(info) < leaf_least) {

				/*let N1 be the previous child of parent(N)*/

				int n1 = find_prev_leaf_sibling(database, table_name, inform,
						n);

				blockInfo* N1;
				int father1;
				if (n1 != -1) {
					N1 = readBlock(database, filename, n1, 1);
					father1 = findFather(database, table_name, inform,
							N1->blockNum);
				}

				if (father == father1 && n1 != -1) {

					/*let K1 be the value between N1 and N in parent(N)*/
					string K1 = originN.substr(1 + 4 + 5, length);
					string tempN1 = N1->cBlock;

					/*if can fit in one block*/
					if (getValueNum(info) + getValueNum(tempN1)
							<= getN(inform, true) - 1) {

						tempN1.replace(1, 4,
								int_to_str(
										getValueNum(info) + getValueNum(tempN1),
										4));
						/*append all in N into N1,let N1.pn=N.pn*/
						tempN1.erase(tempN1.length() - 3, 3);
						string n0 = info.substr(5, tempN.length() - 5);
						tempN1 += n0;

						updateNode(N1, tempN1);
						deleteEntry(database, table_name, inform, father, K1,
								n);
						deleteBlock(database, table_name, inform, n);
					}
					/*redistribution*/
					else {
						/*find the last value and its offset in N1*/
						int numN1 = getValueNum(tempN1);
						string last = tempN1.substr(
								5 + (length + 5) * (numN1 - 1), length + 5);
						/*remove the last from N1*/
						tempN1.replace(1, 4,
								int_to_str(getValueNum(tempN1) - 1, 4));
						tempN1.erase(5 + (length + 5) * (numN1 - 1),
								length + 5);
						updateNode(N1, tempN1);
						/*insert the last in N1 as the first in N*/
						tempN.replace(1, 4,
								int_to_str(getValueNum(tempN) + 1, 4));
						tempN.insert(5, last);
						updateNode(Node, tempN);
						/*replace K1 in parent N by N1.Km*/
						string Km = last.substr(5, length);
						blockInfo* Father = readBlock(database, filename,
								father, 1);
						string tempF = Father->cBlock;
						num = getValueNum(tempF);
						for (int i = 0; i < num; i++) {
							start = (3 + length) * i + 8;
							/*找到后替换*/
							if (tempF.substr(start, length) == K1) {
								tempF.replace(start, length, Km);
								updateNode(Father, tempF);
								break;
							}
						}
					}
				}
				/*or next child of parent(N)*/
				else {

					n1 = find_next_leaf_sibling(database, table_name, inform,
							n);
					N1 = readBlock(database, filename, n1, 1);
					string tempN1 = N1->cBlock;
					string K1 = tempN1.substr(1 + 4 + 5, length);
					/*if can fit in one block*/
					if (getValueNum(tempN) + getValueNum(tempN1)
							<= getN(inform, true) - 1) {

						tempN.replace(1, 4,
								int_to_str(
										getValueNum(info) + getValueNum(tempN1),
										4));

						tempN.erase(tempN.size() - 3, 3);
						/*append all in N1 into N*/
						string part1 = tempN1.substr(5, tempN1.length() - 5);
						tempN += part1;

						updateNode(Node, tempN);

						deleteEntry(database, table_name, inform, father, K1,
								n1);
						/*delete N*/
						deleteBlock(database, table_name, inform, n1);
					}
					/*redistribution*/
					else {
						/*find the first in N1*/
						string first = tempN1.substr(1 + 4, length + 5);
						/*remove the first from N1*/
						tempN1.replace(1, 4,
								int_to_str(getValueNum(tempN1.substr(1, 4)) - 1,
										4));
						tempN1.erase(5, 5 + length);
						updateNode(N1, tempN1);
						/*insert the first in N1 as the last in N*/
						tempN.replace(1, 4,
								int_to_str(getValueNum(tempN) + 1, 4));
						tempN.insert(tempN.length() - 3, first);
						updateNode(Node, tempN);
						/*replace K1 in parent N by N1's first value K1*/
						string N1K1 = tempN1.substr(1 + 4 + 5, length);
						blockInfo* Father = readBlock(database, filename,
								father, 1);
						string tempF = Father->cBlock;
						num = getValueNum(tempF);
						for (int i = 0; i < num; i++) {
							start = (3 + length) * i + 8;
							/*找到后替换*/
							if (tempF.substr(start, length) == K1) {
								tempF.replace(start, length, N1K1);
								updateNode(Father, tempF);
								break;
							}
						}
					}
				}
			}
		}
		/*如果不是叶子*/
		else if (info[0] == '?') {

			int nonleaf_least = ceil(getN(inform, false) / 2.0);	//n / 2 向上取整
			/*if has too few values*/
			if (getValueNum(info) + 1 < nonleaf_least) {

				/*找到N的相邻节点*/
				blockInfo* Father = readBlock(database, filename, father, 1);
				string tempF = Father->cBlock;

				int num = getValueNum(tempF);
				int start, end;
				int n1;
				bool pre = true;
				string K1;
				for (int i = 0; i <= num; i++) {
					start = (3 + length) * i + 5;
					end = start + 2;
					if (tempF.substr(start, 3)
							== int_to_str(Node->blockNum, 3)) {

						/*N1是N的prev节点*/
						if (end != 7) {
							n1 = str_to_int(
									tempF.substr(start - length - 3, 3));
							K1 = tempF.substr(start - length, length);
							pre = true;
						}
						/*N1是N的next节点*/
						else {
							n1 = str_to_int(tempF.substr(end + length + 1, 3));
							K1 = tempF.substr(end + 1, length);
							pre = false;
						}
						break;
					}
				}

				blockInfo* N1 = readBlock(database, filename, n1, 1);
				string tempN1 = N1->cBlock;

				/*let N1 be the previous child of parent(N)*/
				if (pre) {
					/*if can fit in one block*/
					if (getValueNum(tempN) + getValueNum(tempN1)
							< getN(inform, false) - 1) {
						/*append K1 and all in N to N1 */

						tempN1.replace(1, 4,
								int_to_str(
										getValueNum(tempN1) + getValueNum(tempN)
												+ 1, 4));
						string tail = tempN.substr(5, tempN.length() - 5);
						//string head = tempN1.substr(0, 5);

						tempN1 += K1 + tail;
						updateNode(N1, tempN1);

						deleteEntry(database, table_name, inform, father, K1,
								n);
						deleteBlock(database, table_name, inform,
								Node->blockNum);
					}
					/*redistribution*/
					else {
						/*pm is the last pointer in N1*/
						string N1Pm = tempN1.substr(tempN1.length() - 3, 3);
						string N1K = tempN1.substr(tempN1.length() - 3 - length,
								length);
						/*remove N1.Km-1,N1.pm from N1*/
						tempN1.replace(1, 4,
								int_to_str(getValueNum(tempN1) - 1, 4));
						tempN1 = tempN1.substr(0, tempN1.length() - 3 - length);
						updateNode(N1, tempN1);
						/*insert N1.pm,K1 as the first in N*/
						tempN.replace(1, 4,
								int_to_str(getValueNum(tempN) + 1, 4));
						string head = tempN.substr(0, 5);
						string tail = tempN.substr(5, tempN.length() - 5);
						tempN = head + N1Pm + K1 + tail;
						updateNode(Node, tempN);
						/*replace K1 in parent(N) by N1.Km-1*/
						tempF.replace(start - length, length, N1K);
						updateNode(Father, tempF);
					}
				}
				/*or next child of parent(N)*/
				else {
					/*if can fit in one block*/
					if (getValueNum(tempN) + getValueNum(tempN)
							< getN(inform, false) - 1) {
						/*append K1 and all in N to N1 */
						tempN1.replace(1, 4,
								int_to_str(
										getValueNum(tempN1) + getValueNum(tempN)
												+ 1, 4));
						string head = tempN1.substr(0, 5);
						string tail = tempN1.substr(5, tempN1.length() - 5);
						string part = tempN.substr(5, tempN.length() - 5);
						tempN1 = head + part + K1 + tail;
						updateNode(N1, tempN1);
						deleteEntry(database, table_name, inform, father, K1,
								n);
						deleteBlock(database, table_name, inform,
								Node->blockNum);
					}
					/*redistribution*/
					else {
						/*P1 is the first pointer in N1*/
						string N1P1 = tempN1.substr(5, 3);
						string N1K1 = tempN1.substr(8, length);
						/*remove N1.K1,N1.P1 from N1*/
						tempN1.substr(1, 4) = int_to_str(
								getValueNum(tempN1) - 1, 4);
						tempN1.erase(5, length + 3);
						updateNode(N1, tempN1);
						/*insert N1.P1,K1 as the last in N*/
						tempN.replace(1, 4,
								int_to_str(getValueNum(tempN) + 1, 4));
						tempN = tempN + K1 + N1P1;
						updateNode(Node, tempN);
						tempF.replace(end + 1, length, N1K1);
						updateNode(Father, tempF);
					}
				}
			}
		}
	}

}

int getN(index_info& inform, bool isLeaf) {
	if (isLeaf)
		return (4088 / (getLength(inform) + 5)) + 1;
	else
		return (4088 / (getLength(inform) + 3)) + 1;
	//return 4;
}

int getValueNum(string info) {
	return str_to_int(info.substr(1, 4));
}

int getLength(index_info inform) {
	switch (inform.type) {
	case 0:
		return 4;
		break;
	case 1:
		return 10;
		break;
	default:
		return inform.length;
		break;
	}
}

/*将inform中的value位拓展到节点中相应的LENGTH的长度*/
string toLength(index_info inform) {
	string value = inform.value;
	while (value.length() < getLength(inform))
		value = '0' + value;
	return value;
}

int find_prev_leaf_sibling(string database, string table_name,
		struct index_info inform, int nodenum) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	int left = find_left_child(database, table_name, inform);

	/*没有前一个叶子节点*/
	if (left == nodenum)
		return -1;

	int next = find_next_leaf_sibling(database, table_name, inform, left);

	while (next != nodenum) {
		left = next;
		next = find_next_leaf_sibling(database, table_name, inform, left);
	}

	return left;
}

int find_next_leaf_sibling(string database, string table_name,
		struct index_info inform, int nodenum) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	blockInfo* node = readBlock(database, filename, nodenum, 1);
	/*读到异常块*/
	if (node == NULL)
		return -1;
	string info = node->cBlock;
	/*没有下一个节点*/
	if (info[info.length() - 1] == '#')
		return -1;
	int start = info.length() - 3;
	string next = info.substr(start, 3);
	return str_to_int(next);
}

int find_left_child(string database, string table_name,
		struct index_info inform) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	blockInfo* node = readBlock(database, filename, 0, 1);
	string left;
	string info = node->cBlock;
	while (info[0] == '?') {
		left = info.substr(5, 3);
		node = readBlock(database, filename, str_to_int(left), 1);
		info = node->cBlock;
	}
	return str_to_int(left);
}

int find_right_child(string database, string table_name,
		struct index_info inform) {
	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";
	int node = find_left_child(database, table_name, inform);
	int next = find_next_leaf_sibling(database, table_name, inform, node);
	while (next != -1) {
		node = next;
		next = find_next_leaf_sibling(database, table_name, inform, node);
	}
	return node;
}

/*获取一个新的空块*/
int get_new_freeblocknum(string database, string table_name,
		struct index_info& inform) {

	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";

	blockInfo* node;
	for (int i = 0; i < 2048; i++) {
		node = readBlock(database, filename, i, 1);
		if (node->cBlock[0] != '!' && node->cBlock[0] != '?')
			return i;
	}
	return -1;
}

/*删除一个块*/
void deleteBlock(string database, string table_name, struct index_info& inform,
		int n) {

	const string filename = database + "_" + table_name + "_"
			+ inform.index_name + ".index";

	blockInfo* node = readBlock(database, filename, n, 1);
	string info = "@";
	updateNode(node, info);

}

///*获取一个新的空块*/
//int get_new_freeblocknum(string database, string table_name,
//		struct index_info& inform) {
//
//	const string filename = database + "_" + table_name + "_"
//			+ inform.index_name + ".index";
//	blockInfo* root = readBlock(database, filename, 0, 1);
//	fileInfo* file = root->file;
//	/*索引文件为空，或文件结构不对，重新建立空块链表*/
//	if (root->cBlock[0] == '#'
//			|| (root->cBlock[0] != '!' && root->cBlock[0] != '?'))
//		constructList(database, table_name, inform);
//
//	blockInfo* node;
//	if (file->freeNum >= 0) {
//		int newFreeBlock = file->freeNum;
//		node = readBlock(database, filename, file->freeNum, 1);
//		file->freeNum = str_to_int(string(node->cBlock).substr(0, 3));
//		if (file->freeNum == 0)			//最后一个空块已经读完
//			file->freeNum = -1;
//		node->blockNum = newFreeBlock;
//		node->file = file;
//		file->recordAmount++;
//		return newFreeBlock;
//	} else {
//		return -1;			//没有空块
//	}
//}
//
///*删除一个块*/
//void deleteBlock(string database, string table_name, struct index_info& inform,
//		int n) {
//
//	const string filename = database + "_" + table_name + "_"
//			+ inform.index_name + ".index";
//
//	blockInfo* node = readBlock(database, filename, n, 1);
//	fileInfo* file = node->file;
//
//	if (node->cBlock[0] == '#'
//			|| (node->cBlock[0] != '!' && node->cBlock[0] != '?'))
//		return;
//
//	if (file->freeNum >= 0) {
//		string temp = int_to_str(file->freeNum, 3);
//		updateNode(node, temp);
//	} else {
//
//		string temp = int_to_str(0, 3);
//		updateNode(node, temp);
//	}
//
//
//
//	file->freeNum = n;
//
//
//	file->recordAmount--;
//
//	if (file->recordAmount == 0)
//		constructList(database, table_name, inform);
//
//}
//
//void constructList(string database, string table_name,
//		struct index_info& inform) {
//	const string filename = database + "_" + table_name + "_"
//			+ inform.index_name + ".index";
//
//	blockInfo* node;
//
//	for (int i = 0; i < 2048; i++) {		//块号从0开始
//		node = readBlock(database, filename, i, 1);
//		if (i < 255) {
//			string temp = int_to_str(i + 1, 3);		//空块头三个字节保存下一个空块块号
//			updateNode(node, temp);
//		} else {
//			string temp = int_to_str(0, 3);
//			updateNode(node, temp);
//		}
//	}
//	fileInfo* file = node->file;
//	file->freeNum = 0;
//	file->recordAmount = 0;
//
//}

void updateNode(blockInfo* b, string s) {
	strcpy(b->cBlock, s.c_str());
	b->dirtyBit = 1;
}

