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
	//֧��catalog�ӿڵ�buffer��
public:
	BufferCat();
	~BufferCat();

	//input���ļ������ļ�����
	//function����FILEPATH���½�һ���ļ�������д��Щ����
	void newFile(string filename, vector<string>strlst);

	//input���ļ�����T1
	//function��ɾ��T1.cat�ļ�
	void dropFile(string filename);
	
	//input:�ļ���·��
	//output:�����ļ����Ķ���
	//function:��ȡ�ļ���·���������ļ���
	void getFiles(string path,vector<string>& files);

	//input������
	//output��#����1����*����1����*����1unique(1/0)*����1primary(1/0)
	//function����ȡ����string
	vector<string> getAttri(string table);

	//input������
	//output��#����1����*����1����
	//function����ȡ����string
	vector<string>getIndex(string table);

	//input����������������
	//function����indexд�뱾��
	void newIndex(string table, string strindex);

	//input��������������
	//funcion���ڱ���Ϣ��ɾ��һ��index
	void dropIndex(string table, int id);

	//input:����
	//function:����һ����Ϣ���ڵ�һ�е�����+1
	void addRecord(string table);

	//input������
	//function���������м�¼����
	int getRecordNum(string table);

private:
	//�޸��ļ�ĳһ��λ��
	void ModifyLineData(char* fileName, int lineNum, char* lineData);
	void ReadLineData(char* fileName, int lineNum, char* data);
	void DelLineData(char* fileName, int lineNum);
};
#endif // !BUFFER_CATALOG_H

