#include"Catalog.h"
#include"basic.h"
using namespace std;

/*extern string exception_save;	//����ؼ���Ϣ�Ĵ���
extern string table_name;	//����
extern string index_id;
extern vector<string>attri_name;	//��������
extern vector<judge_attr>judger;	//���Դ�С�ж�
extern vector<string>input_value;	//insert������ֵ
extern vector<create_attr>new_table;//�������ã����������
extern string index_attr;			//����������������*/

Catalog::Catalog() {
	if (hasTable(table_name)) {
		hastable = true;
	}
	else
		hastable = false;
	if (!check_format(table_name)) {
		throw input_format_error();
		//��ʽ����ȷ
	}

	//all_index = getIndex();

}

Catalog::~Catalog() {}

//input��table_name, new_table
//function����catalog�ļ����в���һ�����Ԫ��Ϣ
//format��
	//#table_name
	//#���Ը���n
	//#����1����*����1����*����1unique(1/0)*����1primary(1/0)
	//....
	//#��������
	//#����1����*����1����
//exception������Ѿ�����ͬ�����ı���ڣ����׳�table_exist�쳣
void Catalog::createTable() {
	if (!check_format(table_name)) {
		throw input_format_error();
	}
	if (hastable) {
		exception_save = table_name;
		throw table_exist();
	}
	vector<string>input;
	string tmpstr;
	tmpstr = "#0";//�ܼ�¼������ʼ��Ϊ0
	input.push_back(tmpstr);
	tmpstr = "#" + to_string(new_table.size());
	input.push_back(tmpstr);
	//#��������*��������*����unique(1/0)*����primary(1/0)
	for (int i = 0; i < new_table.size(); i++) {
		tmpstr = "#";
		if (!check_format(new_table[i].name)) {
			throw input_format_error();
		}
		tmpstr = tmpstr + new_table[i].name;
		tmpstr += "*";
		tmpstr = tmpstr + to_string(new_table[i].type);
		tmpstr += "*";
		if (new_table[i].unique) {
			tmpstr = tmpstr + "1*";
		}
		else {
			tmpstr = tmpstr + "0*";
		}
		if (new_table[i].primary_key) {
			tmpstr = tmpstr + "1";
		}
		else {
			tmpstr = tmpstr + "0";
		}
		input.push_back(tmpstr);
	}
	tmpstr = "#0";
	input.push_back(tmpstr);
	BufferCat* mybuffer = new BufferCat;
	mybuffer->newFile(table_name, input);
}

//input��table_name
//function����catalog�ļ���ɾ��һ�����Ԫ��Ϣ
//exception����������ڣ��׳�table_not_exist�쳣
void Catalog::dropTable() {
	string str = getLower(table_name);
	if (!hastable) {
		exception_save = str;
		throw table_not_exist();
	}
	BufferCat* mybuffer = new BufferCat;
	mybuffer->dropFile(str);
}



//input��table_name
//output��bool
//function�����Ҷ�Ӧ���Ƿ���ڣ����ڷ���true�������ڷ���false
bool Catalog::hasTable(string name) {
	vector<string>files;
	name = name + ".cat";
	BufferCat* mybuffer = new BufferCat;
	mybuffer->getFiles(".\\Catalog", files);
	for (int i = 0; i < files.size(); i++) {
		if (files[i] == name) {
			return true;
		}
	}
	return false;
}

//input��table_name,attri_name
//output��bool
//function�����Ҷ�Ӧ�����Ƿ���ĳһ���ԣ�����з���true�����û�з���false
bool Catalog::hasAttribute(string attri_name, attribute& back) {
	vector<attribute>attri_lst;
	attri_lst = getAttribute();
	for (int i = 0; i < attri_lst.size(); i++) {
		if (attri_lst[i].name == attri_name) {
			back.name = attri_lst[i].name;
			back.type = attri_lst[i].type;
			back.unique = attri_lst[i].unique;
			back.primary_key = attri_lst[i].primary_key;
			return true;
		}
	}
	return false;
}


//input��table_name
//output�����Զ���(name,type,unique,primary)
//function����ȡһ���������
//exception����������ڣ��׳�table_not_exist�쳣
vector<attribute> Catalog::getAttribute() {
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}
	BufferCat* mybuffer = new BufferCat;
	vector<string>raw_data;
	vector<attribute>ans;
	raw_data = mybuffer->getAttri(table_name);
	attribute tmpattri;
	for (int i = 0; i < raw_data.size(); i++) {
		string raw_string = raw_data[i];
		//#����1����*����1����*����1unique(1/0)*����1primary(1/0)
		raw_string.erase(0, 1);//ȥ��#
		vector<string>data;
		data = split(raw_string, "*");
		tmpattri.name = data[0];
		tmpattri.type = stoi(data[1]);
		if (data[2] == "1")
			tmpattri.primary_key = true;
		else
			tmpattri.primary_key = false;
		if (data[3] == "1")
			tmpattri.unique = true;
		else
			tmpattri.unique = false;
		ans.push_back(tmpattri);
	}
	return ans;
}

//input��table_name
//output�����ر����������
//function������ض������������
vector<index>Catalog::getIndex() {
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}
	vector<string>raw_string;
	vector<index>ans;
	index tmp_index;
	BufferCat* mybuffer = new BufferCat;
	raw_string = mybuffer->getIndex(table_name);
	string tmpstr;
	for (int i = 0; i < raw_string.size(); i++) {
		tmpstr = raw_string[i];
		//#����1����*����1����*����1unique(1/0)*����1primary(1/0)
		tmpstr.erase(0, 1);//ȥ��#
		vector<string>data;
		data = split(tmpstr, "*");
		tmp_index.index_name = data[0];
		tmp_index.attri_name = data[1];
		ans.push_back(tmp_index);
	}
	return ans;
}

//input��table_name��index_attr��index_id
//function����catalog�Ķ�Ӧ���ļ��и��¶�Ӧ���������Ϣ����ָ�������Ͻ���һ��������
//exception����������ڣ��׳�table_not_exist�쳣�������Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣��
//�����Ӧ�����Ѿ������������׳�index_exist�쳣��
void Catalog::createIndex() {
	index newIndex;
	newIndex.index_name = index_id;
	newIndex.attri_name = index_attr;
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}

	//�ж������Ƿ����
	attribute exist_attr;
	if (!hasAttribute(newIndex.attri_name, exist_attr)) {
		exception_save = newIndex.attri_name;
		throw attribute_not_exist();
	}
	if ((!exist_attr.unique) && (!exist_attr.primary_key)) {
		exception_save = newIndex.attri_name;
		throw attri_not_key();
	}

	//�ж������Ƿ��ظ�
	vector<index>exist_index;
	all_index = getIndex();
	exist_index = all_index;
	for (int i = 0; i < exist_index.size(); i++) {
		if (exist_index[i].index_name == newIndex.index_name) {
			exception_save = newIndex.index_name;
			throw index_exist();
			//Error Code: 1061. Duplicate key name 'aa'
		}
		if (exist_index[i].attri_name == newIndex.attri_name) {
			exception_save = newIndex.attri_name;
			//1831 Duplicate index 'ab' defined on the table 'test.stude'. This is deprecated and will be disallowed in a future release.
			throw duplicate_key_name();
		}
	}

	string strIndex;//#������#������
	strIndex = "#" + newIndex.index_name;
	strIndex += "*";
	strIndex += newIndex.attri_name;

	BufferCat* mycat = new BufferCat;
	mycat->newIndex(table_name, strIndex);

}

//input��table_name��index_id
//function��ɾ����Ӧ��Ķ�Ӧ�����ϵ�����
//exception����������ڣ��׳�table_not_exist�쳣
//�����Ӧ���Բ����ڣ��׳�attribute_not_exist�쳣
//�����Ӧ����û���������׳�index_not_exist�쳣��
void Catalog::dropIndex() {
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}
	//�ж������Ƿ����
	vector<index>exist_index;
	bool existIndex = false;
	all_index = getIndex();
	exist_index = all_index;
	int i;
	for (i = 0; i < exist_index.size(); i++) {
		if (exist_index[i].index_name == index_id) {
			existIndex = true;
			break;
		}
	}
	if (!existIndex) {
		//Error Code: 1091. Can't DROP 'aa1'; check that column/key exists
		exception_save = index_id;
		throw index_not_exist();
	}
	BufferCat* mybuffer = new BufferCat;
	mybuffer->dropIndex(table_name, i + 1);
}

//input��table_name
//function����ʾ�����Ϣ(���ԣ�������
//exception����������ڣ��׳�table_not_exist�쳣
void Catalog::showTable() {
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}
	cout.setf(std::ios::left);
	cout << "-------------------------------------------" << endl;
	cout << "ATTRIBUTEs in " << table_name << endl;
	cout << "-------------------------------------------" << endl;
	cout.width(5);
	cout << "";
	cout.width(15);
	cout << "Field";
	cout.width(13);
	cout << "Type";
	cout.width(9);
	cout << "Key" << endl;
	cout << "-------------------------------------------" << endl;

	vector<attribute>attri_lst;
	attri_lst = getAttribute();
	for (int i = 0; i < attri_lst.size(); i++) {
		cout.width(5);
		cout << "";
		cout.width(15);
		cout << attri_lst[i].name;

		if (attri_lst[i].type == -1) {
			cout.width(13);
			cout << "Int";
		}
		else if (attri_lst[i].type == 0) {
			cout.width(13);
			cout << "Float";
		}
		else {
			cout.width(13);
			int size = attri_lst[i].type - 1;
			string str_type = "Char(" + to_string(size);
			str_type += ")";
			cout << str_type;
		}

		if (attri_lst[i].primary_key) {
			cout.width(9);
			cout << "PRIMARY";
		}
		else if (attri_lst[i].unique) {
			cout.width(9);
			cout << "UNIQUE";
		}
		else {
			cout.width(9);
			cout << "NULL";
		}
		cout << endl;
	}
	cout << "-------------------------------------------" << endl;

	vector<index>index_lst;
	all_index = getIndex();
	index_lst = all_index;
	cout << "INDEXs in " << table_name << endl;
	cout << "------------------------------------------------" << endl;
	cout.width(5);
	cout << "";
	cout.width(15);
	cout << "Table";
	cout.width(15);
	cout << "Key_name";
	cout.width(15);
	cout << "Coloumn_name" << endl;
	cout << "------------------------------------------------" << endl;
	for (int i = 0; i < index_lst.size(); i++) {
		cout.width(5);
		cout << "";
		cout.width(15);
		cout << table_name;
		cout.width(15);
		cout << index_lst[i].index_name;
		cout.width(15);
		cout << index_lst[i].attri_name << endl;
	}
	cout << "------------------------------------------------" << endl;
}


//input:table_name
//output��bool
//function:�жϱ����Ƿ���Ϲ淶
//���»���֮����������,�����Ǵ�����
bool Catalog::check_format(string str) {
	bool exist = false;
	for (int i = 0; i < str.length(); i++) {
		char tmp = str[i];
		if (tmp == '_') { exist = true; }
		else if (tmp >= 'a' && tmp <= 'z') {
			exist = true;
		}
		else if (tmp >= 'A' && tmp <= 'Z') {
			exist = true;
		}
		else if (tmp >= '0' && tmp <= '9') {
			exist = true;
		}
		else {
			return false;
		}
	}
	if (exist)
		return true;
	else
		return false;
}

//input��string
//output��string
//function����ȡСд
string Catalog::getLower(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] = str[i] - 'A' + 'a';
		}
	}
	return str;
}

//function�����ַ���str��x���зָ�
vector<string>Catalog::split(string str, string x) {
	vector<string>ans;
	str = str + x;
	string tmp;
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == x.data()[0]) {
			ans.push_back(tmp);
			tmp.clear();
		}
		else {
			tmp += str[i];
		}
	}
	return ans;
}

bool Catalog::hasIndex(string attr_name) {
	all_index = getIndex();
	for (int i = 0; i < all_index.size(); i++) {
		if (all_index[i].attri_name == attr_name) {
			return true;
		}
	}
	return false;
}

//���һ����¼
void Catalog::addRecord() {
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}
	BufferCat* mybuffer = new BufferCat;
	mybuffer->addRecord(table_name);

}

//���ؼ�¼����
int Catalog::numRecord() {
	if (!hastable) {
		exception_save = table_name;
		throw table_not_exist();
	}
	BufferCat* mybuffer = new BufferCat;
	int num = mybuffer->getRecordNum(table_name);
	return num;
}