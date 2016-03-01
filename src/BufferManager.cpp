#include "BufferManager.h"
#include <string>
#include <stdio.h>
#include <iostream>
using namespace std;
#define DIRECTORY "./"		//depend on the data directory and code directory
//DIRECTORY\DB_name\a.index
//         \code\buffer.cpp,buffer.h
//         \code\main.cpp
//         \db.exe
//#define RECORD_DIRECTORY "../../data/record"
//#define INDEX_DIRECTORY "../../data/index"

fileInfo* filehandle = NULL;		//the file in memory
int fileactive = 0;					//max is 5,defined in buffer.h
blockInfo* blockhandle = NULL;		//the abandoned block in memory
int blockactive = 0;				//number of all blocks in memory,including both abandoned ones in blockhandle and useful ones in filehandle.max is 40,defined in buffer.h as well

blockInfo* findBlock(string DB_Name)	//return a point of block available to use
{					//blocks in blockhandle > new block > block in filehandle
	fileInfo* ptrf;
	blockInfo *ptrb, *ptrpre, *maxblock=NULL, *preblock=NULL;
	int max = -1,dirty = 100;

	if (blockhandle != NULL)		//get a block in blockhandle
	{
		ptrb = blockhandle;
		blockhandle = blockhandle->next;
		ptrb->next = NULL;	//this line can be commented(deleted)
		return ptrb;
	}
	if (blockactive < MAX_BLOCK)	//get a new block in memory
	{
		blockInfo* newb = new blockInfo;
		newb->cBlock = new char[BLOCK_LEN];
		return newb;
	}
	ptrf = filehandle;
	while (ptrf != NULL)			//get a block in filehandle by LRU algorithm
	{
		ptrb = ptrf->firstBlock;
		ptrpre = NULL;
		while (ptrb != NULL)
		{
			if (ptrb->lock == 0)			//if the block is not locked
			if (ptrb->dirtyBit <= dirty)	//replace undirty block preferentially
			if (ptrb->iTime > max)
			{
				max = ptrb->iTime;
				dirty = ptrb->dirtyBit;
				maxblock = ptrb;
				preblock = ptrpre;
			}
			ptrpre = ptrb;
			ptrb = ptrb->next;
		}
		ptrf = ptrf->next;
	}
	if (maxblock != NULL)
	{
		if (preblock == NULL)		//it means the maxblock is the firstblock
			maxblock->file->firstBlock = maxblock->next;
		else	preblock->next = maxblock->next;
		if (dirty) writeBlock(DB_Name,maxblock);
		maxblock->iTime = 0;			//reset the age of block
		return maxblock;
	}
	return NULL;
}

void replace(fileInfo * m_fileInfo, blockInfo * m_blockInfo)	//link the block with the file list
{
	if (m_fileInfo->firstBlock == NULL)
		m_fileInfo->firstBlock = m_blockInfo;
	else {
		blockInfo *ptrb = m_fileInfo->firstBlock;
		while (ptrb->next != NULL)
			ptrb = ptrb->next;
		ptrb->next = m_blockInfo;
	}
	m_blockInfo->file = m_fileInfo;
	m_blockInfo->next = NULL;			//this assiment is repeatedly called in several functions,for i'm not sure if there would be any errors without it
}


blockInfo* block_in_buffer(string DB_Name, string m_fileName, int m_blockNum, int m_fileType)	//return the block if it exists in buffer(filehandle)
{
	if (filehandle == NULL) return NULL;
	fileInfo* ptrf = filehandle;
	blockInfo* ptrb = NULL;

	while (ptrf->fileName != m_fileName)
	{
		ptrf = ptrf->next;
		if (ptrf == NULL) return NULL;
	}
	ptrb = ptrf->firstBlock;
	while (ptrb != NULL)
	{
		if (ptrb->blockNum == m_blockNum) return ptrb;
		ptrb = ptrb->next;
	}
	return NULL;
}				//i'm not sure whether search blockhandle is needed,if so,i will add this

blockInfo * get_file_block(string DB_Name, string Table_Name, int fileType, int blockNum)	//return the block according to blocknum.(char *)block->cBlock=NULL if block is new.
{
	fileInfo* ptrf = filehandle;
	ptrf = get_file_info(DB_Name, Table_Name, fileType);
	if (ptrf == NULL) return NULL;
	/*
	//this part is unneccessary in my buffer,but it's required in the doc
	blockInfo* block = block_in_buffer(DB_Name, Table_Name, blockNum, fileType);
	if (block != NULL) return block;
	*/
	//i'm not sure if it necessary to search this block in the blockhandle,that will cost more time on searching but less time on readblock from disk
	blockInfo* ptrb = ptrf->firstBlock;
	while (ptrb != NULL)
	{
		if (ptrb->blockNum == blockNum) return ptrb;
		ptrb = ptrb->next;
	}

	ptrb = findBlock(DB_Name);
	ptrb->blockNum = blockNum;			//update the block info
	ptrb->dirtyBit = 0;
	ptrb->iTime = 0;
	ptrb->lock = 0;
	replace(ptrf, ptrb);				//link block with filehandle
	return ptrb;
}

void closeDatabase(string DB_Name, bool m_flag)		//i dont know whata m_flag means
{
	fileInfo* ptrf = filehandle;
	fileInfo* tmp;
	while (ptrf != NULL)
	{
		tmp = ptrf;
		ptrf = ptrf->next;
		closeFile(DB_Name, tmp->fileName, tmp->type, m_flag);
	}
}

void closeFile(string DB_Name, string m_fileName, int m_fileType, bool m_flag)
{
	fileInfo* ptrf = filehandle;
	fileInfo* ptrpre = NULL;
	while (ptrf != NULL)
	{
		if (ptrf->type == m_fileType && ptrf->fileName == m_fileName)
		{
			fileactive--;
			blockInfo* ptrb = ptrf->firstBlock;
			while (ptrb != NULL)			//update the changed block
			{
				if (ptrb->dirtyBit) writeBlock(DB_Name, ptrb);
				ptrb = ptrb->next;
			}
			if (blockhandle==NULL)
				blockhandle = ptrf->firstBlock;
			else {						//throw the blocks into trash bin(blockhandle)
				ptrb = blockhandle;
				while (ptrb->next != NULL) ptrb = ptrb->next;
				ptrb->next = ptrf->firstBlock;
			}

			//update the file info in disk and filehandle
			if (ptrf->type == 0) Write_Table_Info(DB_Name, m_fileName, ptrf->recordAmount, ptrf->recordLength, ptrf->freeNum);
			else	Write_Index_Info(DB_Name, m_fileName, ptrf->recordAmount, ptrf->recordLength, ptrf->freeNum);
			if (ptrpre == NULL) filehandle = ptrf->next;
			else	ptrpre->next = ptrf->next;
			delete ptrf;
			break;
		}
		ptrpre = ptrf;
		ptrf = ptrf->next;
	}

}

fileInfo* get_file_info(string DB_Name, string fileName, int m_fileType)
{				//return the file requested
	fileInfo* ptrf = filehandle;
	while (ptrf != NULL)
	{
		if (ptrf->type == m_fileType && ptrf->fileName == fileName)	break;
		ptrf = ptrf->next;
	}
	if (ptrf != NULL) return ptrf;		//if it exists in filehandle
										//if not,get a new file struct
	fileInfo *newf = new fileInfo;		
	newf->type = m_fileType;
	newf->fileName = fileName;
	newf->next = NULL;
	newf->firstBlock = NULL;

	bool b = true;
	if (m_fileType == 0) b = Get_Table_Info(DB_Name, fileName, newf->recordAmount, newf->recordLength, newf->freeNum);
	if (m_fileType == 1) b = Get_Index_Info(DB_Name, fileName, newf->recordAmount, newf->recordLength, newf->freeNum);
	if (!b)
	{
		delete newf;
		return NULL;
	}

	if (fileactive < MAX_FILE_ACTIVE)	//if filehandle not full,add the new one
	{
		fileactive++;
		ptrf = filehandle;
		if (ptrf == NULL)	filehandle = newf;
		else {
			while (ptrf->next != NULL)
				ptrf = ptrf->next;
			ptrf->next = newf;
		}
	}
	else{							//if filehandle already full,replace an old one
		fileInfo* ptrf = filehandle->next;
		closeFile(DB_Name, filehandle->fileName, filehandle->type, 0);
		filehandle = newf;
		newf->next = ptrf;
	}
	return newf;
}

bool writeBlock(string DB_Name, blockInfo * block)	//write the block data into disk
{
	fileInfo* ptrf = block->file;
	FILE* file;
	string address = DIRECTORY;
	address += DB_Name;
	address += "/" + ptrf->fileName;
	file = fopen(address.c_str(), "rb+");
	if (file)
	{
		fseek(file, (block->blockNum + 1)*BLOCK_LEN, 0);		//the offset of this block
		fwrite(block->cBlock, sizeof(char), BLOCK_LEN, file);
	}
	else {
		cout<<"file not found!"<<"writeBlock error"<<endl;
		return false;
	}
	fclose(file);
	return true;
}

blockInfo* readBlock(string DB_Name, string m_fileName, int m_blockNum, int m_fileType)
{				//return the block according to these informations
	blockInfo* ptrb = block_in_buffer(DB_Name, m_fileName, m_blockNum, m_fileType);
	if (ptrb != NULL) return ptrb;			//if it already in buffer(filehandle)
	ptrb = get_file_block(DB_Name, m_fileName, m_fileType, m_blockNum);
	FILE* file;				
	string address = DIRECTORY;
	address += DB_Name;
	address += "/" + m_fileName;
	file = fopen(address.c_str(), "rb");
	if (file)
	{
		fseek(file, (m_blockNum + 1)*BLOCK_LEN, 0);		//offset of block
		fread(ptrb->cBlock, sizeof(char), BLOCK_LEN, file);
	}
	else {
		cout<<"file not found!"<<"readBlock error"<<endl;
		return NULL;
	}
	fclose(file);
	return ptrb;
}

void quitProg(string DB_Name)		//i dont know if it different from closeDB
{
	fileInfo* ptrf = filehandle;
	fileInfo* tmp;
	while (ptrf != NULL)
	{
		tmp = ptrf;
		ptrf = ptrf->next;
		closeFile(DB_Name, tmp->fileName, tmp->type, 1);
	}
}

bool Get_Table_Info(string DB_Name, string Table_Name, int &record_count, int & record_len, int & free)			//read headfile of record from disk
{
	FILE* file;
	string address = DIRECTORY;
	address += DB_Name;
	address += "/" + Table_Name;
	file = fopen(address.c_str(), "rb");
	if (file)
	{
		fseek(file, OFFSET_RECORDAMOUNT, 0);
		fread(&record_count, sizeof(int), 1, file);
		fseek(file, OFFSET_RECORDLENGTH, 0);
		fread(&record_len, sizeof(int), 1, file);
		fseek(file, OFFSET_FREENUM, 0);
		fread(&free, sizeof(int), 1, file);
	}
	else {
		cout<<"file not found!"<<"Get_Table_Info error"<<endl;
		return false;
	}
	fclose(file);
	return true;
}

bool Get_Index_Info(string DB_Name, string Index_Name, int & record_count, int & record_len, int & free)		//read head file of index from disk
{
	FILE* file;
	string address = DIRECTORY;
	address += DB_Name;
	address += "/" + Index_Name;
	file = fopen(address.c_str(), "rb");
	if (file)
	{
		fseek(file, OFFSET_RECORDAMOUNT, 0);
		fread(&record_count, sizeof(int), 1, file);
		fseek(file, OFFSET_RECORDLENGTH, 0);
		fread(&record_len, sizeof(int), 1, file);
		fseek(file, OFFSET_FREENUM, 0);
		fread(&free, sizeof(int), 1, file);
	}
	else {
		cout<<"file not found!"<<"Get_Index_Info error"<<endl;
		return false;
	}
	fclose(file);
	return true;
}

void Write_Table_Info(string DB_Name, string Table_Name, int & record_count, int & record_len, int & free)		//write the head file of record into disk
{
	FILE* file;
	string address = DIRECTORY;
	address += DB_Name;
	address += "/" + Table_Name;
	file = fopen(address.c_str(), "rb+");
	if (file)
	{
		fseek(file, OFFSET_RECORDAMOUNT, 0);
		fwrite(&record_count, sizeof(int), 1, file);
		fseek(file, OFFSET_RECORDLENGTH, 0);
		fwrite(&record_len, sizeof(int), 1, file);
		fseek(file, OFFSET_FREENUM, 0);
		fwrite(&free, sizeof(int), 1, file);
	}
	else cout<<"file not found!"<<"Write_Table_Info error"<<endl;
	fclose(file);
}

void Write_Index_Info(string DB_Name, string Index_Name, int & record_count, int & record_len, int & free)		//write head file of index into disk
{
	FILE* file;
	string address = DIRECTORY;
	address += DB_Name;
	address += "/" + Index_Name;
	file = fopen(address.c_str(), "rb+");
	if (file)
	{
		fseek(file, OFFSET_RECORDAMOUNT, 0);
		fwrite(&record_count, sizeof(int), 1, file);
		fseek(file, OFFSET_RECORDLENGTH, 0);
		fwrite(&record_len, sizeof(int), 1, file);
		fseek(file, OFFSET_FREENUM, 0);
		fwrite(&free, sizeof(int), 1, file);
	}
	else cout<<"file not found!"<<"Write_Index_Info error"<<endl;
	fclose(file);
}
