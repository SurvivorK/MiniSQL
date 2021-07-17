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

	//input��table_name, new_table
	//function����catalog�ļ����в���һ�����Ԫ��Ϣ
	//exception������Ѿ�����ͬ�����ı���ڣ����׳�table_exist�쳣
	void createTable();

	//input��table_name
	//function����catalog�ļ���ɾ��һ�����Ԫ��Ϣ
	//exception����������ڣ��׳�table_not_exist�쳣
	void dropTable();

	//input��table_name
	//output��bool
	//function�����Ҷ�Ӧ���Ƿ���ڣ����ڷ���true�������ڷ���false
	bool hasTable(string name);

	//input��table_name,attri_name
	//output��attribute
	//function�����Ҷ�Ӧ�����Ƿ���ĳһ���ԣ�����з���true�����û�з���null
	bool hasAttribute(string attri_name, attribute&back);

	//input��table_name
	//output�����Զ���(name,type,unique,primary)
	//function����ȡһ�������������
	//exception����������ڣ��׳�table_not_exist�쳣
	vector<attribute> getAttribute();

	//input��table_name��index_attr��index_id
	//function����catalog�Ķ�Ӧ���ļ��и��¶�Ӧ���������Ϣ����ָ�������Ͻ���һ��������
	//exception����������ڣ��׳�table_not_exist�쳣�������Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣��
	//�����Ӧ�����Ѿ������������׳�index_exist�쳣��
	void createIndex();

	//input��table_name��index_id
	//function��ɾ����Ӧ���ϵ�����
	//exception����������ڣ��׳�table_not_exist�쳣
	//�����Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
	//�����Ӧ����û���������׳�index_not_exist�쳣��
	void dropIndex();

	//input��table_name
	//function����ʾ�����Ϣ(���ԣ�������
	//exception����������ڣ��׳�table_not_exist�쳣
	void showTable();
	
	//input��table_name
	//output�����ر����������
	//function������ض������������
	vector<index>getIndex();

	//�ж��Ƿ�������
	bool hasIndex(string attr_name);

	//���һ����¼
	void addRecord();

	//���ؼ�¼����
	int numRecord();


private:
	//input��string
	//output��string
	//function����ȡСд
	string getLower(string);

	//input:table_name
	//output��bool
	//function:�жϱ���/�����Ƿ���Ϲ淶
	//��ĸ�����ֿ�ͷ�����»���֮���������ţ�������1���»��߻���ĸ
	bool check_format(string);
	
	//function�����ַ���str��x���зָ�
	vector<string>split(string str,string x);

	bool hastable;

	vector<index>all_index;
	
};
#endif // ! CATALOG_H
