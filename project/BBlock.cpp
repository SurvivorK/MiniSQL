#include"BBlock.h"
#include<cmath>
//key:
int KEY::cmp(const KEY &a) const 
{
	assert(this->ktype == a.ktype);
	assert(this->ktype != KTP::UNDEF);
	switch (this->ktype)
	{
	case KTP::INT:
		return this->int_data - a.int_data;
		break;
	case KTP::FLOAT:
		if (fabs(this->float_data - a.float_data) < 1e-7) return 0;
		else if (this->float_data - a.float_data < 0) return -1;
		else return 1;
		break;
	case KTP::STRING:
	{    if(this->len == a.len) 
	     {
		  for (int i = 0; i < a.len; i++)
		  {
			  if (this->str_data[i] > a.str_data[i]) return 1;
			  if (this->str_data[i] < a.str_data[i]) return -1;
		  }
		  return 0;
	     }
		return strcmp(this->str_data, a.str_data);
		break;
	 }
	default:
		assert(false);
		break;
	}
	return 0;
}
bool KEY ::operator<(const KEY &a)const { return this->cmp(a) < 0; }
bool KEY ::operator>(const KEY &a)const { return this->cmp(a) > 0; }
bool KEY ::operator==(const KEY &a)const { return this->cmp(a) == 0; }
bool KEY ::operator<=(const KEY &a)const { return this->cmp(a) <= 0; }
bool KEY ::operator>=(const KEY &a) const { return this->cmp(a) >= 0; }
//////////////////////////////////////////////////
//BBlock:
bool BBlock::Is_dirty() { return this->dirty_tag; }
bool BBlock::Is_pin() { return this->pin_tag; }
bool BBlock::Is_busy() { return this->busy_tag; }
void BBlock::Set_pin(bool st) { this->pin_tag = st; }
void BBlock::Set_busy(bool st) { this->busy_tag = st; }
void BBlock::Set_dirty(bool st) { this->dirty_tag = st; }
void BBlock::Block_to_file()
{
	if (this->Is_dirty() == 0) return;// not necessary to wrtie back;
	string filename = file_name+".table";
	FILE *fp;
	if ((fp = fopen(filename.c_str(), "r+b")) == NULL)
	{
		cout << "open file error" << endl;
		return;
	}
	fseek(fp, this->block_id*BLOCK_SIZE, SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, fp);
	fclose(fp);
	this->init("", -1);
}

void BBlock :: Read_String(char *c, int len, int offset)
{
	memcpy(c, (char*)(this->data + offset), len);
}
void BBlock:: Read_Float(float &s, int len, int offset)
{
	memcpy((char*)&s, (char*)(this->data + offset), len);

}
void BBlock::Read_Int(int &s, int length, int offset) // length: sizeof(int)
{
	memcpy((char*)&s, (const char*)(this->data + offset), length);
}

void BBlock::Read_valid(char &s, int len, int offset)// legnth: sizeof(char)
{
	memcpy((char*)&s, (const char*)(this->data + offset),len);
}

void BBlock::Write_String( char* c, int offset, int len)
{
	this->Set_dirty(true);
	// const char* c = s.c_str();
	memcpy(this->data + offset, c, len);
	// memset(this->data + offset + s.size(), 0, length - s.size());
}
void BBlock::Write_String(string &s,  int offset)
{
	this->Set_dirty(true);
	const char* c = s.c_str();
	memcpy(this->data + offset, c, s.size());
	// memset(this->data + offset + s.size(), 0, length - s.size()); 
}
	void BBlock::Write_Int(int &s, int offset)
{
	this->Set_dirty(true);
	memcpy(this->data + offset, (const char *)&s, sizeof(s));
	// memset(this->data + offset + strlen(c), 0, length - strlen(c)); 
}
void BBlock::Write_Float(float &s, int offset) 
{
	this->Set_dirty(true);
	memcpy(this->data + offset, (const char *)&s, sizeof(s));
	// memset(this->data + offset + strlen(c), 0, length - strlen(c));
}

bool File_Exist(const string &filename)
{
	fstream fin;
	fin.open(filename, ios::in | ios::binary);
	if (!fin.good()) return fin.close(), 0;
	return fin.close(), 1;
}