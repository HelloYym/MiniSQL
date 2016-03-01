#pragma once
#include "Declaration.h"
#include <string>
using namespace std;


#define BLOCK_LEN			4096	// the size of one block
#define MAX_FILE_ACTIVE 	5		// limit the active files in the buffer
#define MAX_BLOCK			40		// the max number of the blocks

#define OFFSET_RECORDAMOUNT 4
#define OFFSET_FREENUM 8
#define OFFSET_RECORDLENGTH 12



blockInfo* findBlock(string DB_Name);

void replace(fileInfo * m_fileInfo, blockInfo * m_blockInfo);

blockInfo* block_in_buffer(string DB_Name, string m_fileName, int m_blockNum, int m_fileType);

blockInfo * get_file_block(string DB_Name, string Table_Name, int fileType, int blockNum);

void closeDatabase(string DB_Name, bool m_flag);

void closeFile(string DB_Name, string m_fileName, int m_fileType, bool m_flag);

bool writeBlock(string DB_Name, blockInfo * block);

fileInfo* get_file_info(string DB_Name, string fileName, int m_fileType);

blockInfo* readBlock(string DB_Name, string m_fileName, int m_blockNum, int m_fileType);

void quitProg(string DB_Name);

bool Get_Table_Info(string DB_Name, string Table_Name, int &record_count, int & record_len, int & free);

bool Get_Index_Info(string DB_Name, string Index_Name, int & record_count, int & record_len, int & free);

void Write_Table_Info(string DB_Name, string Table_Name, int & record_count, int & record_len, int & free);

void Write_Index_Info(string DB_Name, string Index_Name, int & record_count, int & record_len, int & free);
