#include"record.h"
#include "Buffer.h"
extern Buffer global_buffer;
bool pd(KEY val,string attName, Record tmp_record)
{
	for (auto itt = tmp_record.begin(); itt != tmp_record.end(); itt++)
	{
		if (itt->attr_name == attName)
		{
			return (*itt) == val;
			break;
		}
	}
}
bool is_unique(KEY val, vector<Attribute> attr,string attname,string tableName, int record_num)// true 表示 合法
{
	Records * myRecords = new Records(tableName,attr);
	Record  tmp_record;
	for (int i = 0; i < record_num; i++)
	{
		myRecords->get_Record(i, tmp_record);
		if (pd(val,attname,tmp_record))  return false;
	}
	return true;
}

Records::Records(string table_name,vector<Attribute> P)
{
	this->tableName = table_name;
	this->sizePerRecord = 0;
//	catalog = &(cm.getCatalogByName(tblName));
//	attributes = catalog->attributes;
//	Attribute tmp; tmp.type = KTP::INT;  tmp.attribute_name = "zyh"; tmp.str_len = 3;
	attributes=P;

	for (auto it = attributes.begin(); it != attributes.end(); it++)
	{
		switch (it->type)
		{
		case KTP::INT:
		{
			sizePerRecord += sizeof(int);
			break;
		}
		case KTP::FLOAT:
		{
			sizePerRecord += sizeof(float);
			break;
		}
		case KTP::STRING:
		{
			int varcharLen = it->str_len;
			sizePerRecord += (varcharLen * sizeof(char));
			break;
		}
		case KTP::UNDEF:
		{
			assert(0);
			break;
		}
		}
	}

	(this->sizePerRecord)+=2;//留一个字节 valid
	this->maxRecordsNumPerBlock = int(BLOCK_SIZE / sizePerRecord);
}
void Records::add_block(int block_id)
{
	string filename = (this->tableName)+".table";
	FILE *fp;
	if ((fp = fopen(filename.c_str(), "r+b")) == NULL)
	{
		cout << "open file error" << endl;
		return;
	}
	fseek(fp, block_id*BLOCK_SIZE, SEEK_SET);
	char data[5000] = {0};
	fwrite(data, BLOCK_SIZE, 1, fp);
	fclose(fp);


}
bool Records::createRecordFile()
{
	fstream fout;
	fout.open((this->tableName + ".table"), ios::out | ios::binary);
	if (fout.good())
	{
		return true;
	}
	cerr << "create file error" << endl;
	return false;
}
bool Records::delete_one_record(int recordID)
{
	int  BlockID = recordID / maxRecordsNumPerBlock;//
	BBlock & block = global_buffer.read_block(tableName, BlockID);
	int offset = (recordID % maxRecordsNumPerBlock) * sizePerRecord;
	///////////////////
	string Valid = "0";
	block.Write_String(Valid, offset++);//置valid位 而不是真的删除

	return true;
	
}
bool Records::deleteAllRecords()
{
	global_buffer.Buffer_flush();
	remove((this->tableName + ".table").c_str());
	return true;
}
bool Records::insertRecord(Record *R,int recordID) //recordID为当前要插入记录的ID
{
	int  BlockID = recordID / maxRecordsNumPerBlock;/////////// 此处之后得改
	if (recordID == 0)//一块block都未被创建,要插入第一条记录时
	{
		BlockID=0;
		add_block(BlockID);// 似乎不先占坑 也是可以的
	}
	else if (recordID / maxRecordsNumPerBlock != (recordID-1)/ maxRecordsNumPerBlock)//max-now_used<sizePerRecord  该块剩下的不够再存一整条记录了待修改
	{
		BlockID = recordID / maxRecordsNumPerBlock;
		add_block(BlockID); 
	}
	BBlock & block = global_buffer.read_block(tableName,BlockID);
	//block.file_name = this->tableName;
	//block.block_id = BlockID;
	block.Set_dirty(true);
	int offset = (recordID % maxRecordsNumPerBlock) * sizePerRecord;
	///////////////////
	string Valid = "1";
	block.Write_String(Valid, offset++);//置valid位
	for (auto it = attributes.begin(); it != attributes.end(); it++)
	{
		string attrName = it->attribute_name;
		KTP attrType = it->type;

		//KEY tmp;
		switch (attrType)
		{
		case KTP::INT:
		{
			// Find the corresponding key-value tuple in the record
			for (auto itt =R->begin(); itt != R->end(); itt++)
			{
				if (itt->attr_name == attrName)
				{
					block.Write_Int(itt->int_data, offset);
					//key.int_data = itt->value.intValue;
					break;
				}
			}
			offset += sizeof(int);
			break;
		}
		case KTP::FLOAT:
		{
			// Find the corresponding key-value tuple in the Rord
			for (auto itt = R->begin(); itt != R->end(); itt++)
			{
				if (itt->attr_name == attrName)
				{
					block.Write_Float(itt->float_data, offset);
				//	key.float_data = itt->value.floatValue;
					break;
				}
			}
			offset += sizeof(float);
			break;
		}
		case KTP::STRING:
		{ 
			int stringLen = it->str_len;
			// Find the corresponding key-value tuple in the record
			for (auto itt = R->begin(); itt != R->end(); itt++)
			{
				if (itt->attr_name == attrName)
				{
					block.Write_String(itt->str_data, offset, stringLen);
					//strcpy(key.str_data, itt->value.charValue);
					break;
				}
			}
			offset += (sizeof(char) * stringLen);
		}
		}
	}

	return 1;
}

bool Records::get_Record(int recordID, Record& record)
{
	int blockID = (recordID / maxRecordsNumPerBlock);
	int offset = (recordID % maxRecordsNumPerBlock) * sizePerRecord; 

	return get_Record(blockID, offset, record);
}

bool Records::get_Record(int blockID, int offset, Record& record)
{
	record = Record();
	string tb = tableName;
	BBlock & block = global_buffer.read_block(tb, blockID);;

	char valid;
	block.Read_valid (valid, sizeof(char), offset++);
	// If the record is marked as deleted
	if (valid == '0') return false;

	for (auto it = attributes.begin(); it != attributes.end(); it++)
	{
		KEY tmp;
		tmp.attr_name = it->attribute_name;
		tmp.ktype = it->type;
		switch (it->type)
		{
		case KTP::INT:
		{
			int value;
			block.Read_Int(value, sizeof(int), offset);
			tmp.int_data=value;
			offset += sizeof(int);
			break;
		}
		case KTP::FLOAT:
		{
			float value;
			block.Read_Float(value, sizeof(float), offset);
			tmp.float_data=value;
			offset += sizeof(float);
			break;
		}
		case KTP::STRING:
		{
			char value[256];
			//string value;
			block.Read_String(value, it->str_len, offset);
			//                kvt.setValue(value, it->VARCHAR_LEN);
			//                cerr <<"STRING ewscaedsvcx" << tmp << " " << it->VARCHAR_LEN<< endl;
			for (int i = 0; i < it->str_len; i++)
			{
				tmp.str_data[i] = value[i];
			}
			offset += (sizeof(char) * it->str_len);
			break;
		}
		}
		record.push_back(tmp);
	}
	return true;
}
