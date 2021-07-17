#ifndef BBlock_H
#define BBlock_H

//这里要注意 block_id是在单个文件中的id
#pragma once
#include <iostream>
#include <cassert>
#include <fstream>
#include <map>
#include <cmath>
using namespace std;
#define BLOCK_SIZE 4096
#define BLOCK_CNT 1024
//typedef char BYTE;
enum class KTP
{
	UNDEF = 0,
	INT,
	FLOAT,
	STRING
};
class BBlock
{
public:
	char *data;
	int block_id;
	string  file_name;
	BBlock() : data(NULL), pin_tag(false), dirty_tag(false), busy_tag(false), file_name(""), block_id(-1)
	{
		data = new char[BLOCK_SIZE+1];
		for (int i = 0; i <= BLOCK_SIZE; i++) data[i] = 0;
	}
	void init(string file_name, int block_id)
	{
		for (int i = 0; i <= BLOCK_SIZE; ++i) data[i] = 0;
		this->file_name = file_name, this->block_id = block_id;
		pin_tag = dirty_tag = false; 
	}
	~BBlock(){}
	bool  Is_dirty();
	bool  Is_busy();
	bool  Is_pin();
	void Set_pin(bool st);
	void Set_busy(bool st);
	void Set_dirty(bool st);
	void pre(string & file_name, int &block_id);
	void Block_to_file();
	//void Read_String(string &s, int length, int offset); // 用法:从当前块的offset位置读入长度为l的data，并记录到string里.
	void Read_String(char *c, int length, int offset); // 用法:从当前块的offset位置读入长度为l的data，并记录到string里.
	void Read_Float(float &s, int length,  int offset);
	void Read_Int(int &s,int length, int offset);
	void Read_valid(char &valid, int len, int offset);
	void Write_String(string &s, int offset); // 将当前string读进来
	void Write_String( char* c,  int offset, int len); //写出一个长度为len
	void Write_Int( int &s,  int offset);
	void Write_Float( float &s, int offset);
	// 用法:从当前块的offset位置写入一个string，长度为len(比如"aaa"长度为4),注意要保证offset+len < BLOCK_SIZE
private:
	bool pin_tag, dirty_tag, busy_tag;
};

struct KEY // KeyType Finished 但是需要传入块的 id 和 offset 用之前set一下len

{
	KTP ktype;
	string attr_name;
	int int_data, len; // datalen
	float float_data;
	char str_data[256];
	//char data[256];
	KEY()
	{
		ktype = KTP::UNDEF;
	}
	void Read(BBlock &x, int offset)
	{
		assert(ktype != KTP::UNDEF);
		//		assert(offset + len < 4096);
		if (ktype == KTP::INT) x.Read_Int(int_data, len, offset);
		else if (ktype == KTP::FLOAT) x.Read_Float(float_data, len, offset);
		else x.Read_String(str_data, len, offset);
	}
	void Write(BBlock &x, int offset) // + offset
	{
		assert(ktype != KTP::UNDEF);
		if (ktype == KTP::INT) x.Write_Int(int_data, offset);
		else if (ktype == KTP::FLOAT) x.Write_Float(float_data, offset);
		else x.Write_String(str_data, offset, len);
	}
	bool operator<(const KEY &a)const;
	bool operator>(const KEY &a)const;
	bool operator==(const KEY &a)const;
	bool operator<=(const KEY &a)const;
	bool operator>=(const KEY &a) const;
	int cmp(const KEY &a) const;
};
bool File_Exist(const string &filename);
#endif