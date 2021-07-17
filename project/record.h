#ifndef record_H
#define record_H
#pragma once
#include <cstdlib>
#include <string>
#include <vector>
#include"BBlock.h"
#include <map>
#pragma once
#include"attribute.h"

using namespace std;
// A record is a vector of key-value tuples.

//extern CatalogManager cm;

typedef vector<KEY> Record;

class Records
{
public:

	Records() {};
	// `Records` Constructor: Existing table, retrieve record content.
	Records(string table_name, vector<Attribute> P);
	// The name of the table which records belong to.
	string tableName;
	// The size of each record.
	int sizePerRecord;
	// Maximum number of records per block;
	int maxRecordsNumPerBlock;
	// A pointer to vector of all fields in the table. (Records.h)
	vector<Attribute> attributes;
	// A reference to the catalog object of the table
	//Catalog* catalog;

	// Create a new record file.
	bool createRecordFile();
	// Delete all records from a table.
	bool delete_one_record(int recordID);
	bool deleteAllRecords();
	// Insert a single record into the table.
	bool insertRecord(Record * rec,int recordID);
	//bool insertRecord(vector<ValueWithType> * values);
	// Insert multiple records into the table.
//    bool insertRecords(vector<Record> * vRec);
	// Retrieve a single record. (The record needs to be read in buffer before actually being retrieved.)
	bool get_Record(int recordID, Record& record);
	void add_block(int block_id);
private:
	bool get_Record(int blockID, int offset, Record& record);
	// Get value of a specified attribute from record
	// Value& retrieveAttrValueFromRecord(Record & rec, string attrName);
//	int getRecordSize() { return (catalog->table_block_cnt - 1) * maxRecordsNumPerBlock + catalog->usage; }
};
#endif