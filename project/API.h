#include"Catalog.h"
#include"basic.h"
#include"attribute.h"
#include"Buffer.h"
#include"record.h"
#include"IndexManager.h"

#include<vector>

using namespace std;
class API {
public:
	API();
	API(string table);
	~API();
	void createTable();
	void dropTable();
	vector<Record> selectRecord_noidx();//无索引的select
	vector<Record>selectRecord_idx(int index_pos);//有索引的select
	void selectRecord();
	void deleteRecord();
	void createIndex();
	void dropIndex();
	void insertRecord();
	void showinfo(vector<Record>);
	IndexManager* myIdx;
private:
	string tableName;
	bool judge_less(KEY, judge_attr);
	bool judge_bigger(KEY, judge_attr);
	bool judge_equal(KEY, judge_attr);
	string get_standard_index(string table, string key,int type);
	vector<Attribute> transferAttri(vector<create_attr>);
	bool exist_key;
	bool check_key = false;
};