#ifndef  CATALOG_H
#define CATALOG_H
#include<string>
#include<vector>
#include<string>
#include<iostream>
#include<iomanip>
#include"basic.h"
#include"BufferCat.h"
using namespace std;
class Catalog {
public:
	Catalog();
	~Catalog();

	//input：table_name, new_table
	//function：在catalog文件夹中插入一个表的元信息
	//exception：如果已经有相同表名的表存在，则抛出table_exist异常
	void createTable();

	//input：table_name
	//function：在catalog文件中删除一个表的元信息
	//exception：如果表不存在，抛出table_not_exist异常
	void dropTable();

	//input：table_name
	//output：bool
	//function：查找对应表是否存在，存在返回true，不存在返回false
	bool hasTable(string name);

	//input：table_name,attri_name
	//output：attribute
	//function：查找对应表中是否有某一属性，如果有返回true，如果没有返回null
	bool hasAttribute(string attri_name, attribute&back);

	//input：table_name
	//output：属性对象(name,type,unique,primary)
	//function：获取一个表的所有属性
	//exception：如果表不存在，抛出table_not_exist异常
	vector<attribute> getAttribute();

	//input：table_name，index_attr，index_id
	//function：在catalog的对应表文件中更新对应表的索引信息（在指定属性上建立一个索引）
	//exception：如果表不存在，抛出table_not_exist异常。如果对应属性不存在，抛出attribute_not_exist异常。
	//如果对应属性已经有了索引，抛出index_exist异常。
	void createIndex();

	//input：table_name，index_id
	//function：删除对应表上的索引
	//exception：如果表不存在，抛出table_not_exist异常
	//如果对应属性不存在，抛出attribute_not_exist异常
	//如果对应属性没有索引，抛出index_not_exist异常。
	void dropIndex();

	//input：table_name
	//function：显示表的信息(属性，索引）
	//exception：如果表不存在，抛出table_not_exist异常
	void showTable();
	
	//input：table_name
	//output：返回表的所有索引
	//function：获得特定表的所有索引
	vector<index>getIndex();

	//判断是否有索引
	bool hasIndex(string attr_name);

	//添加一条记录
	void addRecord();

	//返回记录条数
	int numRecord();


private:
	//input：string
	//output：string
	//function：获取小写
	string getLower(string);

	//input:table_name
	//output：bool
	//function:判断表名/列名是否符合规范
	//字母或数字开头，无下划线之外的特殊符号，至少有1个下划线或字母
	bool check_format(string);
	
	//function：对字符串str用x进行分割
	vector<string>split(string str,string x);

	bool hastable;

	vector<index>all_index;
	
};
#endif // ! CATALOG_H
