#ifndef INTERPRETER_H
#define INTERPRETER_H
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<sstream>
#include<cassert>
#include<fstream>
#include<ctime>
#include<Windows.h>
#include<vector>
#include"basic.h"
#include"Catalog.h"
#include"API.h"
#include"Buffer.h"

using namespace std;

class Interpreter {
public:
	Interpreter();
	//Interpreter(IndexManager* &myIdx);

    //function:获取分号结尾的输入信息，并规范化
	//exception：无
    void getQuery(string);

	//function:对输入初步解析，判断功能
	//exception:输入的关键字不存在，抛出异常
	void EXEC();

	//input:select a,b,c from table_x where condition1 and condition2
	//function:支持单表多约束，等值/区间查询
	//output:抛出异常或进入selectRecord()获取记录
	//exception:格式错误、表不存在、属性不存在、数据类型不匹配
	void EXEC_select();

	//input:drop table table_name
	//output:抛出异常或成功
	//function:删除表
	//exception:格式错误、表不存在
	void EXEC_drop_table();

	//input:drop index ID_index
	//output:抛出异常或成功
	//function:删除一个叫ID_index的索引
	//exception:对应属性没有索引、对应属性不存在
	void EXEC_drop_index();

	//input:create index ID_index on table1(id)
	//output:抛出异常或成功
	//function:在表table1的id属性建立ID_index索引
	//exception:表不存在、对应属性不存在、对应属性已有索引、该属性名已存在
	void EXEC_create_index();

	//input:create table T1(
    //            NAME char(32),
    //            ID int unique,
    //            SCORE float,
    //            primary key (ID));
	//output:抛出异常或成功
	//function:在数据库中插入表的元信息
	//exception:格式错误、表已存在
	void EXEC_create_table();

	//input:insert into T1 values('WuZhaoHui',0001,99.99);
	//output:抛出异常或成功
	//function：插入值
	//exception:格式错误、长度超过限制、插入数量不正确
	void EXEC_insert();

	//input:delete from table1 where id=1;
    //     delete * from table1;
	//output:抛出异常或成功
	//function:删除全部元组/删除符合条件的元组
	//exception:格式错误、表不存在、属性不存在、数据不匹配
	void EXEC_delete();

	//input:desc table1
	//function：输出table1的所有属性、所有索引
	void EXEC_desc();

	//input:execfile FILEPATH
	//function:读取文件信息，用于数据库操作
	void EXEC_openfile();

    void EXEC_doCommand(string commandStr);

	string backInfo;
	IndexManager* myIdx;

private:
	//存放字符串
	std::string query;
	//保存字符串用于报错
	std::string staticQuery;


	//function:字符串规范化
	void strNormalize();
	
	//input:单词的开头位置，传出结尾位置地址
	//output:对应单词的string
	//function:query取字
	std::string getWord(int pos, int &end_pos);

	//input:开头位置，传出结尾位置地址
	//output:对应属性值的串
	//function：在insert/select/delete 中负责获得值（数字、“”、‘’）
	string getValue(int pos, int &end_pos);

	//input:被操作的母串，被转换单词开始位置
	//output:将pos位置的单词小写后，输出完整字符串
	//function:用于标准化
	string getLower(string str, int pos);

	//input:单词的开头位置，传出结尾位置地址
	//output:所对应关系符号<>=
	//function：取出关系符号
	string getRelation(int pos, int &end_pos);

	//input:单词的开头位置，传出结尾位置地址
	//output:类型（-1~255）
	int getType(int pos, int &end_pos);

	//function:获取整数的位数
	int getBits(int num);
	//function:获取保留四位小数后的浮点数位数
	int getBits(float num);

};







#endif // !INTERPRETER_H
