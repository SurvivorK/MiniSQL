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

    //function:��ȡ�ֺŽ�β��������Ϣ�����淶��
	//exception����
    void getQuery(string);

	//function:����������������жϹ���
	//exception:����Ĺؼ��ֲ����ڣ��׳��쳣
	void EXEC();

	//input:select a,b,c from table_x where condition1 and condition2
	//function:֧�ֵ����Լ������ֵ/�����ѯ
	//output:�׳��쳣�����selectRecord()��ȡ��¼
	//exception:��ʽ���󡢱����ڡ����Բ����ڡ��������Ͳ�ƥ��
	void EXEC_select();

	//input:drop table table_name
	//output:�׳��쳣��ɹ�
	//function:ɾ����
	//exception:��ʽ���󡢱�����
	void EXEC_drop_table();

	//input:drop index ID_index
	//output:�׳��쳣��ɹ�
	//function:ɾ��һ����ID_index������
	//exception:��Ӧ����û����������Ӧ���Բ�����
	void EXEC_drop_index();

	//input:create index ID_index on table1(id)
	//output:�׳��쳣��ɹ�
	//function:�ڱ�table1��id���Խ���ID_index����
	//exception:�����ڡ���Ӧ���Բ����ڡ���Ӧ�����������������������Ѵ���
	void EXEC_create_index();

	//input:create table T1(
    //            NAME char(32),
    //            ID int unique,
    //            SCORE float,
    //            primary key (ID));
	//output:�׳��쳣��ɹ�
	//function:�����ݿ��в�����Ԫ��Ϣ
	//exception:��ʽ���󡢱��Ѵ���
	void EXEC_create_table();

	//input:insert into T1 values('WuZhaoHui',0001,99.99);
	//output:�׳��쳣��ɹ�
	//function������ֵ
	//exception:��ʽ���󡢳��ȳ������ơ�������������ȷ
	void EXEC_insert();

	//input:delete from table1 where id=1;
    //     delete * from table1;
	//output:�׳��쳣��ɹ�
	//function:ɾ��ȫ��Ԫ��/ɾ������������Ԫ��
	//exception:��ʽ���󡢱����ڡ����Բ����ڡ����ݲ�ƥ��
	void EXEC_delete();

	//input:desc table1
	//function�����table1���������ԡ���������
	void EXEC_desc();

	//input:execfile FILEPATH
	//function:��ȡ�ļ���Ϣ���������ݿ����
	void EXEC_openfile();

    void EXEC_doCommand(string commandStr);

	string backInfo;
	IndexManager* myIdx;

private:
	//����ַ���
	std::string query;
	//�����ַ������ڱ���
	std::string staticQuery;


	//function:�ַ����淶��
	void strNormalize();
	
	//input:���ʵĿ�ͷλ�ã�������βλ�õ�ַ
	//output:��Ӧ���ʵ�string
	//function:queryȡ��
	std::string getWord(int pos, int &end_pos);

	//input:��ͷλ�ã�������βλ�õ�ַ
	//output:��Ӧ����ֵ�Ĵ�
	//function����insert/select/delete �и�����ֵ�����֡�������������
	string getValue(int pos, int &end_pos);

	//input:��������ĸ������ת�����ʿ�ʼλ��
	//output:��posλ�õĵ���Сд����������ַ���
	//function:���ڱ�׼��
	string getLower(string str, int pos);

	//input:���ʵĿ�ͷλ�ã�������βλ�õ�ַ
	//output:����Ӧ��ϵ����<>=
	//function��ȡ����ϵ����
	string getRelation(int pos, int &end_pos);

	//input:���ʵĿ�ͷλ�ã�������βλ�õ�ַ
	//output:���ͣ�-1~255��
	int getType(int pos, int &end_pos);

	//function:��ȡ������λ��
	int getBits(int num);
	//function:��ȡ������λС����ĸ�����λ��
	int getBits(float num);

};







#endif // !INTERPRETER_H
