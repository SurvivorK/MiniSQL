#ifndef BUFFER_CATALOG_H
#define BUFFER_CATALOG_H
#include<string>
#include<vector>
#include<string>
#include<fstream>
#include<iostream>
#include<cstdio>
#include<io.h>
#include<QDir>
#include"basic.h"

#define FILEPATH "./Catalog/"
#define FILEPATHTABLE "./Table/"
using namespace std;
class BufferCat {
	//支持catalog接口的buffer层
public:
	BufferCat();
	~BufferCat();

	//input：文件名，文件内容
	//function：在FILEPATH下新建一个文件，里面写这些内容
	void newFile(string filename, vector<string>strlst);

	//input：文件名如T1
	//function：删除T1.cat文件
	void dropFile(string filename);
	
	//input:文件夹路径
	//output:所有文件名的队列
	//function:获取文件夹路径下所有文件名
	void getFiles(string path,vector<string>& files);

	//input：表名
	//output：#属性1名称*属性1类型*属性1unique(1/0)*属性1primary(1/0)
	//function：获取属性string
	vector<string> getAttri(string table);

	//input：表名
	//output：#索引1名称*索引1属性
	//function：获取索引string
	vector<string>getIndex(string table);

	//input：表名，索引序列
	//function：将index写入本地
	void newIndex(string table, string strindex);

	//input：表名，索引名
	//funcion：在表信息中删除一个index
	void dropIndex(string table, int id);

	//input:表名
	//function:新增一条信息后，在第一行的数字+1
	void addRecord(string table);

	//input：表名
	//function：返回现有记录条数
	int getRecordNum(string table);

private:
	//修改文件某一行位置
	void ModifyLineData(char* fileName, int lineNum, char* lineData);
	void ReadLineData(char* fileName, int lineNum, char* data);
	void DelLineData(char* fileName, int lineNum);
};
#endif // !BUFFER_CATALOG_H

