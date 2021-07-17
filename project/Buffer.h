#ifndef Buffer_H
#define Buffer_H
#pragma once
#include"BBlock.h"
using namespace std;

class Buffer
{
public:
	Buffer() { BBuffer = new BBlock[BLOCK_CNT]; }
	~Buffer() { delete[] BBuffer; }
	BBlock& read_block(string &filename, int  block_id);
	//BBlock& get_free_Buffer();
	BBlock& GET_LRU();
	void DeleteFile(string & filename);
	void Buffer_flush();
private:
	BBlock *BBuffer;
};
extern Buffer global_buffer;

#endif