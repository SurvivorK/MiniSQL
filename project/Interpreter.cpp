#include"Interpreter.h"
using namespace std;

/*/extern string exception_save;
string table_name;	//����
string index_id;
vector<string>attri_name;	//��������
vector<judge_attr>judger;	//���Դ�С�ж�
vector<string>input_value;	//insert������ֵ
vector<create_attr>new_table;//�������ã����������
string index_attr;			//����������������*/

Interpreter::Interpreter() {
}
//Interpreter::Interpreter(IndexManager* &target):myIdx(target){
//}

//function:��ȡһ���������Ϣ�����淶��
//exception����
void Interpreter::getQuery(string input) {
    /*string tmp;
	do {
		std::cout << ">>>";
		getline(cin, tmp);
		query = query + tmp;
		staticQuery += tmp;
		query = query + " ";
    } while (tmp[tmp.length() - 1] != ';');*/
    for(int i=0;i<input.size();i++){
        if(input[i]=='\n'){
            input.erase(i,1);
        }
    }
    query=input+"; ";
	query.erase(query.length() - 2, 1);
    //query = query + "\0";
    query.push_back('\0');
    qDebug()<<QString::fromStdString(query);
	strNormalize();
}

//function:�ַ����淶��
void Interpreter::strNormalize() {
	for (int i = 1; i < query.length(); i++) {
		if (query[i] == '*' || query[i] == '=' || query[i] == ',' || query[i] == '(' || query[i] == ')' || query[i] == '<' || query[i] == '>') {
			if (query[i - 1] != ' ')
				query.insert(i++, " ");
			if (query[i + 1] != ' ')
				query.insert(++i, " ");
		}
		if (query[i] == '\t') {
			query.erase(i, 1);
		}
	}
	bool flag = true;
	while (flag) {
		if (query[0] == ' ') {
			query.erase(0, 1);
		}
		else {
			flag = false;
		}
	}
	flag = true;
	for (int i = 1; i < query.length(); i++) {
		if (query[i] == ' ' && query[i - 1] == ' ')query.erase(i--, 1);
	}
	query = getLower(query, 0);
}

//function:����������������жϹ���
	//exception:����Ĺؼ��ֲ����ڣ��׳��쳣
void Interpreter::EXEC() {
	//string exception_save;
	int end_pos;
	string function = getWord(0, end_pos);
	end_pos++;
	try {
		if (function == "select") {
			EXEC_select();
		}
		else if (function == "drop") {
			function = getWord(end_pos, end_pos);
			end_pos++;
			function = getLower(function, 0);
			if (function == "table") {
				EXEC_drop_table();
			}
			else if (function == "index") {
				EXEC_drop_index();
			}
			else {
				throw input_format_error();
			}
		}
		else if (function == "insert") {
			EXEC_insert();
		}
		else if (function == "create") {
			function = getWord(end_pos, end_pos);
			end_pos++;
			function = getLower(function, 0);
			if (function == "table") {
				EXEC_create_table();
			}
			else if (function == "index") {
				EXEC_create_index();
			}
			else {
				throw input_format_error();
			}
		}
		else if (function == "delete") {
			EXEC_delete();
		}
		else if (function == "desc") {
			EXEC_desc();
		}
		else if (function == "execfile") {
			EXEC_openfile();
		}
		else if (function == "quit") {
			if (getWord(end_pos, end_pos) == "")
				throw exit_command();
			else
				throw input_format_error();
		}
		else {
			throw input_format_error();
		}
	}
	catch (input_format_error error) {
		//14:49:02
		//creat index index1 on company(city)
		//Error Code: 1064.
		//You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1064" << endl;
		cout << ">>>You have an error in your SQL syntax; check the manual that corresponds to SQL server version for the right syntax" << endl;
	}
	catch (primary_key_conflict error) {
		//16:21:00	
		//insert into student2 values(1080104000,'name4000',59)	
		//Error Code: 1062. 
		//Duplicate entry '1080104000' for key 'student2.PRIMARY'	0.000 sec
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1062" << endl;
		cout << ">>>Duplicate entry for key '" << exception_save << "'" << endl;

	}
	catch (attribute_not_exist error) {
		//14:49:02
		//creat index index1 on company(city)
		//Error Code: 1072.
		// Key column 'IaD' doesn't exist in table
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1072" << endl;
		cout << ">>>Key column '" << exception_save << "' doesn't exist in table" << endl;

	}
	catch (wrong_filepath error) {
		//14:49:02
		//creat index index1 on company(city)
		//D://...is not a invalid file path
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>" << exception_save << " is not an valid file path" << endl;

	}
	catch (exit_command error) {
		cout << "See you next time!" << endl;
		global_buffer.Buffer_flush();
		myIdx->SaveToFile();
		exit(0);
	}
	catch (duplicate_column error) {
		//14:49:02
		//creat table
		//Error Code: 1060. 
		//Duplicate column name 'ID'
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1060" << endl;
		cout << ">>>" << "Duplicate column name '" << exception_save << "'" << endl;
	}
	catch (table_exist error) {
		//00:47:48
		//create table company( Id int )
		//Error Code: 1050
		//Table 'company' already exists
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1050" << endl;
		cout << ">>>" << "Table '" << exception_save << "' already exists" << endl;
	}
	catch (table_not_exist error) {
		//22:51:25	
		//drop table ljy	
		//Error Code: 1051
		//Unknown table 'ljy'
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1051" << endl;
		cout << ">>>" << "Unknown table '" << exception_save << "'" << endl;
	}
	catch (index_exist error) {
		//22:51:25	
		//create index aa on table(attr)
		//Error Code: 1061
		//Duplicate key name 'aa'
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1061" << endl;
		cout << ">>>" << "Duplicate key name '" << exception_save << "'" << endl;
	}
	catch (duplicate_key_name error) {
		//22:51:25	
		//create index aa on table(attr)
		//Error Code: 1831
		//Duplicate index 'ab' defined on the table 'stude'.
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1083" << endl;
		cout << ">>>" << "Duplicate index '" << exception_save << "' defined on the table '" << getLower(table_name, 0) << "'" << endl;
	}
	catch (attri_not_key error) {
		//22:51:25	
		//create index aa on table(attr)
		//Coloumn 'attr' is not a key in table
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>" << "Coloumn  '" << exception_save << "' is not a key in '" << getLower(table_name, 0) << "'" << endl;
	}
	catch (index_not_exist error) {
		//17:02:02
		//drop index aa1 on stude
		//Error Code: 1091.
		//Can't DROP 'aa1'; check that column/key exists
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1091" << endl;
		cout << ">>>" << "Can't DROP '" << exception_save << "'; check that column/key exists" << endl;
	}
	catch (column_too_long error) {
		//09:43:07	
		//create table aaaa( name char(266))	
		//Error Code: 1074. 
		//Column length too big for column 'name' (max = 255)
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1074" << endl;
		cout << ">>>" << "Column length too big for column '" << exception_save << "'" << endl;
	}
	catch (data_type_conflict error) {
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>" << "Insert type of data into " << table_name << "." << exception_save << "is not fit" << endl;
	}
	catch (data_too_long error) {
		//11:33:37	
		//insert into stude values(111,"mm",111.1)	
		//Error Code: 1406. 
		//Data too long for column 'name' at row 1	0.000 sec
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>" << "Data too long for column '" << exception_save << "' " << endl;
	}
	catch (column_not_match error) {
		//11:30:40	
		//insert into test.stude values (15,"jak",15.55,3)	
		//Error Code: 1136. 
		//Column count doesn't match value count
		cout << ">>>time: ";
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%02d:%02d:%02d\n", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		cout << ">>>Action: " << staticQuery << endl;
		cout << ">>>Error Code: 1136" << endl;
		cout << ">>>" << "Column count doesn't match value count" << endl;
	}
}

//input:select a,b,c from table_x where condition1 and condition2
//function:֧�ֵ����Լ������ֵ/�����ѯ
//output:�׳��쳣�����selectRecord()��ȡ��¼
//exception:��ʽ���󡢱����ڡ����Բ����ڡ��������Ͳ�ƥ��
void Interpreter::EXEC_select() {
	//string table_name;	//����
	//vector<string>attri_name;	//��������
	//vector<judge_attr>judger;	//�ж�
	table_name.clear();
	attri_name.clear();
	judger.clear();
	int end_pos = 7;
	int tmp_pos = 0;
	//��÷��ص������б�
	if (getWord(end_pos, end_pos) == "*") {
		//select *
		end_pos++;
		tmp_pos = end_pos;
		attri_name.push_back("*");

	}
	else {
		end_pos = 7;
		attri_name.push_back(getWord(end_pos, end_pos));
		end_pos++;
		while (1) {
			tmp_pos = end_pos;
			if (getWord(end_pos, end_pos) != ",") {
				end_pos++;
				break;
			}
			else {
				end_pos++;
				attri_name.push_back(getWord(end_pos, end_pos));
				end_pos++;
			}
		}
	}

	if (getLower(getWord(tmp_pos, end_pos), 0) != "from")
		throw input_format_error();
	end_pos++;

	//��ȡ����
	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;



	if (getWord(end_pos, tmp_pos) != "") {
		if (getLower(getWord(end_pos, end_pos), 0) != "where")
			throw input_format_error();
		end_pos++;
		//��ȡ�ж����
		judge_attr tmpjudge;
		tmpjudge.attr = getWord(end_pos, end_pos);
		end_pos++;
		tmpjudge.op = getRelation(end_pos, end_pos);
		if (tmpjudge.op == "")
			throw input_format_error();
		tmpjudge.value = getWord(end_pos, end_pos);
		end_pos++;
		judger.push_back(tmpjudge);
		string tmpattr;
		while (1) {
			tmpattr = getWord(end_pos, end_pos);
			end_pos++;
			if (getLower(tmpattr, 0) != "and") {
				if (tmpattr != "") {
					throw input_format_error();
				}
				break;
			}
			else {
				tmpjudge.attr = getWord(end_pos, end_pos);
				end_pos++;
				tmpjudge.op = getRelation(end_pos, end_pos);
				if (tmpjudge.op == "")
					throw input_format_error();
				tmpjudge.value = getWord(end_pos, end_pos);
				end_pos++;
				judger.push_back(tmpjudge);
			}
		}


	}
	//����catalog�鿴�ñ��Ƿ���ڣ������Ƿ����
	Catalog* mycat = new Catalog;
	if (!mycat->hasTable(table_name)) {
		exception_save = table_name;
		throw table_not_exist();
	}
	attribute tmpatt;
	if (attri_name[0] == "*") {
		if (attri_name.size() != 1) {
			throw input_format_error();
		}
	}
	else {
		for (int i = 0; i < attri_name.size(); i++) {
			if (!mycat->hasAttribute(attri_name[i], tmpatt)) {
				exception_save = attri_name[i];
				throw attribute_not_exist();
			}
		}
	}

	for (int i = 0; i < judger.size(); i++) {
		if (!mycat->hasAttribute(judger[i].attr, tmpatt)) {
			exception_save = judger[i].attr;
			throw attribute_not_exist();
		}
		else {
			if (tmpatt.type == -1) {
				//int
				int value = atoi(judger[i].value.c_str());
				char* target = new char[0];
				if (judger[i].value != _itoa(value, target, 10)) {
					//���Բ�ƥ��
					exception_save = judger[i].attr;
					throw data_type_conflict();
				}
			}
			else if (tmpatt.type == 0) {
				//float
				istringstream iss(judger[i].value);
				float num;
				iss >> num;

				ostringstream oss;
				oss << num;
				string str(oss.str());

				if (judger[i].value != str) {
					//���Բ�ƥ��**********************************************
					exception_save = judger[i].value;
					throw data_type_conflict();
				}
			}
		}
	}


	//����API
	API myAPI(table_name); myAPI.myIdx = myIdx;
	myAPI.selectRecord();
	//backInfo = myAPI.selectRecord();
}


//input:drop table table_name
//output:�׳��쳣��ɹ�
//function:ɾ����
//exception:��ʽ���󡢱�����
void Interpreter::EXEC_drop_table() {
	//string tablename;
	table_name.clear();
	int end_pos = 10;

	end_pos++;
	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;
	if (getWord(end_pos, end_pos) != "") {
		throw input_format_error();
	}


	//����API
	API myAPI(table_name); myAPI.myIdx = myIdx;
	myAPI.dropTable();

}

//input:drop index i1 on table1
//output:�׳��쳣��ɹ�
//function:ɾ��һ����table1�Ͻ�i1������
//exception:��Ӧ����û����������Ӧ���Բ�����
void Interpreter::EXEC_drop_index() {
	//string index_id;
	index_id.clear();
	int end_pos = 11;

	index_id = getWord(end_pos, end_pos);
	end_pos++;
	if (getWord(end_pos, end_pos) != "on") {
		throw input_format_error();
	}
	end_pos++;
	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;

	if (getWord(end_pos, end_pos) != "") {
		throw input_format_error();
	}

	//����API
	API myAPI(table_name); myAPI.myIdx = myIdx;
	myAPI.dropIndex();
}

//input:insert into T1 values('WuZhaoHui',0001,99.99);
//output:�׳��쳣��ɹ�
//function������ֵ
//exception:��ʽ���󡢳��ȳ������ơ�������������ȷ
void Interpreter::EXEC_insert() {
	//string table_name;
	table_name.clear();
	input_value.clear();
	//vector<string>input_value;
	int end_pos;
	if (getLower(getWord(7, end_pos), 0) != "into") {
		throw input_format_error();
	}
	else {
		end_pos++;
		table_name = getLower(getWord(end_pos, end_pos), 0);
		end_pos++;
		if (getLower(getWord(end_pos, end_pos), 0) != "values") {
			throw input_format_error();
		}
		end_pos++;

		if (getWord(end_pos, end_pos) != "(") {
			throw input_format_error();
		}
		end_pos++;

		string tmpvalue;
		input_value.push_back(getValue(end_pos, end_pos));
		//end_pos++;
		while (1) {
			tmpvalue = getWord(end_pos, end_pos);
			if (tmpvalue != ",") {
				if (tmpvalue != ")") {
					throw input_format_error();
				}
				end_pos++;
				break;
			}
			else {
				end_pos++;
				input_value.push_back(getValue(end_pos, end_pos));
				//end_pos++;
			}
		}
		if (getWord(end_pos, end_pos) != "") {
			throw input_format_error();
		}
		//����API
		API myAPI(table_name); myAPI.myIdx = myIdx;
		myAPI.insertRecord();

	}
}

//input:create table T1(
//            NAME char(32),
//            ID int unique,
//            SCORE float,
//            primary key (ID));
//output:�׳��쳣��ɹ�
//function:�����ݿ��в�����Ԫ��Ϣ
//exception:��ʽ���󡢱��Ѵ���
void Interpreter::EXEC_create_table() {
	//string table_name;
	//vector<create_attr>new_table;
	create_attr new_attr;
	table_name.clear();
	new_table.clear();

	int end_pos = 13;
	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;

	if (getWord(end_pos, end_pos) != "(") {
		throw input_format_error();
	}
	end_pos++;

	string tmpstr;
	new_attr.name = getWord(end_pos, end_pos);
	end_pos++;
	new_attr.type = getType(end_pos, end_pos);
	if (new_attr.type > 256) {
		exception_save = new_attr.name;
		throw column_too_long();
	}

	if (new_attr.type == -2) {
		throw input_format_error();
	}
	end_pos++;
	tmpstr = getWord(end_pos, end_pos);
	if (tmpstr == "unique") {
		end_pos++;
		new_attr.unique = true;
	}
	else {
		new_attr.unique = false;
		end_pos = end_pos - tmpstr.length();
	}
	new_attr.primary_key = false;
	new_table.push_back(new_attr);
	while (1) {
		tmpstr = getWord(end_pos, end_pos);
		end_pos++;
		if (tmpstr != ",") {
			if (tmpstr != ")") {
				throw input_format_error();
			}
			end_pos = end_pos - tmpstr.length();
			break;
		}
		else {
			int x;
			string ifprimary = getWord(end_pos, end_pos);
			x = end_pos;
			if (getLower(ifprimary, 0) == "primary") {
				x++;
				tmpstr = getLower(getWord(x, x), 0);
				x++;
				if (tmpstr != "key") {
					throw input_format_error();
				}
				else {
					tmpstr = getWord(x, x);
					if (tmpstr != "(")throw input_format_error();
					x++;
					tmpstr = getWord(x, x);
					x++;
					bool find_key = false;
					for (int i = 0; i < new_table.size(); i++) {
						if (new_table[i].name == tmpstr) {
							new_table[i].primary_key = true;
							find_key = true;
							break;
						}
					}
					if (!find_key) {
						exception_save = tmpstr;
						throw attribute_not_exist();
					}
					if (getWord(x, x) != ")") {
						throw input_format_error();
					}
					else {
						x++;
						if (getWord(x, x) != ")") {
							throw input_format_error();
						}
						end_pos = x + 1;
						break;
					}
				}
			}
			new_attr.name = ifprimary;
			//end_pos = x;
			end_pos++;
			new_attr.type = getType(end_pos, end_pos);
			if (new_attr.type == -2) {
				throw input_format_error();
			}
			end_pos++;
			tmpstr = getLower(getWord(end_pos, end_pos), 0);
			if (tmpstr == "unique") {
				end_pos++;
				new_attr.unique = true;
			}
			else {
				new_attr.unique = false;
				end_pos = end_pos - tmpstr.length();
			}
			new_table.push_back(new_attr);
		}
	}

	if (getWord(end_pos, end_pos) != "") {
		throw input_format_error();
	}

	//������Բ��ظ�
	for (int i = 0; i < new_table.size(); i++) {
		for (int j = i + 1; j < new_table.size(); j++) {
			if (new_table[i].name == new_table[j].name) {
				exception_save = new_table[i].name;
				throw duplicate_column();
			}
		}
	}


	table_name = getLower(table_name, 0);
	//����API
	API myAPI(table_name); myAPI.myIdx = myIdx;
	myAPI.createTable();


}

//input:create index ID_index on table1(id)
//output:�׳��쳣��ɹ�
//function:�ڱ�table1��id���Խ���ID_index����
//exception:�����ڡ���Ӧ���Բ����ڡ���Ӧ�����������������������Ѵ���
void Interpreter::EXEC_create_index() {
	//string index_id;
	//string table_name;
	//string index_attr;
	index_id.clear();
	table_name.clear();
	int end_pos = 13;

	index_id = getWord(end_pos, end_pos);
	end_pos++;

	if (getLower(getWord(end_pos, end_pos), 0) != "on") {
		throw input_format_error();
	}
	end_pos++;

	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;

	if (getWord(end_pos, end_pos) != "(") {
		throw input_format_error();
	}
	end_pos++;

	index_attr = getWord(end_pos, end_pos);
	end_pos++;

	if (getWord(end_pos, end_pos) != ")") {
		throw input_format_error();
	}
	end_pos++;

	if (getWord(end_pos, end_pos) != "") {
		throw input_format_error();
	}

	//����API
	API myAPI(table_name); myAPI.myIdx = myIdx;
	myAPI.createIndex();
}

//input:delete from table1 where id=1;
//     delete * from table1;
//output:�׳��쳣��ɹ�
//function:ɾ��ȫ��Ԫ��/ɾ������������Ԫ��
//exception:��ʽ���󡢱����ڡ����Բ����ڡ����ݲ�ƥ��
void Interpreter::EXEC_delete() {
	//string table_name;	//����
	//vector<judge_attr>judger;	//�ж�
	table_name.clear();
	judger.clear();
	bool flag_all = false;

	int end_pos = 7;
	//int tmp_pos = 0;
	//��÷��ص������б�
	if (getWord(end_pos, end_pos) == "*") {
		//delete *
		judge_attr tmpjudge;
		tmpjudge.attr = "*";
		end_pos++;
		judger.push_back(tmpjudge);
		flag_all = true;
	}
	else {
		end_pos = 7;
	}

	if (getLower(getWord(end_pos, end_pos), 0) != "from")
		throw input_format_error();
	end_pos++;

	//��ȡ����
	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;

	if (!flag_all) {
		if (getLower(getWord(end_pos, end_pos), 0) != "where")
			throw input_format_error();
		end_pos++;

		//��ȡ�ж����
		judge_attr tmpjudge;
		tmpjudge.attr = getWord(end_pos, end_pos);
		end_pos++;
		tmpjudge.op = getRelation(end_pos, end_pos);
		if (tmpjudge.op == "")
			throw input_format_error();
		tmpjudge.value = getValue(end_pos, end_pos);
		//end_pos++;
		judger.push_back(tmpjudge);
		string tmpattr;
		while (1) {
			tmpattr = getWord(end_pos, end_pos);
			end_pos++;
			if (getLower(tmpattr, 0) != "and") {
				if (tmpattr != "") {
					throw input_format_error();
				}
				break;
			}
			else {
				tmpjudge.attr = getWord(end_pos, end_pos);
				end_pos++;
				tmpjudge.op = getRelation(end_pos, end_pos);
				if (tmpjudge.op == "")
					throw input_format_error();
				tmpjudge.value = getValue(end_pos, end_pos);
				//end_pos++;
				judger.push_back(tmpjudge);
			}
		}
	}
	else {
		if (getWord(end_pos, end_pos) != "") {
			throw input_format_error();
		}
	}

	//����API
	API myAPI(table_name); myAPI.myIdx = myIdx;
	myAPI.deleteRecord();
}

//input:desc table1
//function�����table1���������ԡ���������
void Interpreter::EXEC_desc() {
	int end_pos = 5;
	//string table_name;
	table_name = getLower(getWord(end_pos, end_pos), 0);
	end_pos++;

	if (getWord(end_pos, end_pos) != "") {
		throw input_format_error();
	}

	//����catalog
	Catalog* mycat = new Catalog;
	mycat->showTable();
}

//input:execfile FILEPATH
//function:��ȡ�ļ���Ϣ���������ݿ����
void Interpreter::EXEC_openfile() {
	int end_pos = 9;
	string filePath;
	filePath = getWord(end_pos, end_pos);

	if (getWord(end_pos, end_pos) != "") {
		throw input_format_error();
	}

	int pos;
	pos = filePath.find("\\");
	while (pos != -1) {
		filePath.replace(pos, string("\\").length(), "/");
		pos = filePath.find("\\");
	}

	ifstream fin(filePath);
	if (!fin.is_open()) {
		exception_save = filePath;
		throw wrong_filepath();
	}
	else {
		stringstream ss;
		//���ļ����е��ַ����뵽�ַ�������
		ss << fin.rdbuf();
		//��ȡ���е��ַ���
		string tmp_query = ss.str();

		int check_index = 0;
		for (int i = 0; i < tmp_query.length() - 1; i++) {
			if (tmp_query[i] == '\n' && tmp_query[i + 1] == '\n')
				tmp_query.erase(i, 1);
		}
		while (check_index < tmp_query.length()) {
			query = "";
			if (tmp_query[check_index] == '\n')
				check_index++;
			while (tmp_query[check_index] != ';' && check_index < tmp_query.length()) {
				if (tmp_query[check_index] == '\n') {
					check_index++;
					continue;
				}
				query = query + tmp_query[check_index++];
			}
			check_index++;
			if (query == " " || query == "")continue;
			query = query + " \0";
			cout << ">>>" << query << ";" << endl;
			staticQuery = query;
			strNormalize();
			EXEC();
		}
	}

}


//input:��������ĸ������ת�����ʿ�ʼλ��
//output:��posλ�õĵ���Сд����������ַ���
//function:���ڱ�׼��
string Interpreter::getLower(string str, int pos) {
	int i = 0;
	while (str[i] != ' ' && str[i] != '\0') {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] = str[i] + 32;
		}
		i++;
	}
	return str;
}


//input:���ʵĿ�ͷλ�ã�������βλ�õ�ַ
//output:��Ӧ���ʵ�string
//function:queryȡ��
string Interpreter::getWord(int pos, int& end_pos) {
	string ans = "";
	end_pos = pos;
	while (query[end_pos] != ' ' && end_pos < query.length() && query[end_pos] != '\0') {
		ans = ans + query[end_pos];
		end_pos++;
	}
	return ans;
}

//input:���ʵĿ�ͷλ�ã�������βλ�õ�ַ
//output:����Ӧ��ϵ����<>=
//function��ȡ����ϵ����
string Interpreter::getRelation(int pos, int& end_pos) {
	std::string PartWord = "";
	end_pos = pos;
	while (1) {
		if (query[end_pos] == ' ') {
			end_pos++;
		}
		else if (query[end_pos] == '<' || query[end_pos] == '>' || query[end_pos] == '=') {
			PartWord = PartWord + query[end_pos];
			end_pos++;
		}
		else {
			break;
		}
	}
	if (PartWord == "<" || PartWord == ">" || PartWord == "=" || PartWord == "<>" || PartWord == "<=" || PartWord == ">=")
		return PartWord;
	else
		return "";
}

//input:���ʵĿ�ͷλ�ã�������βλ�õ�ַ
//output:����
//		int		-1
//		float	0
//		char(n)	n+1
//		others -2
int Interpreter::getType(int pos, int& end_pos) {
	std::string type = getLower(getWord(pos, end_pos), 0);
	if (type == "int")
		return -1;
	else if (type == "float")
		return 0;
	else if (type == "char") {
		end_pos += 3;
		std::string length = getWord(end_pos, end_pos);
		end_pos += 2;
		return atoi(length.c_str()) + 1;
	}
	else {
		return -2;
	}
}

//function:��ȡ������λ��
int Interpreter::getBits(int num) {
	int bit = 0;
	if (num == 0)
		return 1;
	if (num < 0) {
		bit++;
		num = -num;
	}
	while (num != 0) {
		num /= 10;
		bit++;
	}
	return bit;
}
//function:��ȡ������λС����ĸ�����λ��
int Interpreter::getBits(float num) {
	int bit = 0;
	if ((int)num == 0)
		return 5;
	if (num < 0) {
		bit++;
		num = -num;
	}
	int integer_part = num;
	while (integer_part != 0) {
		bit++;
		integer_part /= 10;
	}
	return bit + 4;//Ϊ�˱���С����ĺ�λ
}

//input:��ͷλ�ã�������βλ�õ�ַ
//output:��Ӧ����ֵ�Ĵ�
//function����insert/select/delete �и�����ֵ�����֡�������������
string Interpreter::getValue(int pos, int& end_pos) {
	string ans = getWord(pos, end_pos);
	pos = ++end_pos;
	if (ans[0] == '"') {
		while (ans[ans.length() - 1] != '\"') {
			if (pos >= query.length()) {
				throw input_format_error();
			}
			ans = ans + getWord(pos, end_pos);
			pos = ++end_pos;
		}
	}
	else if (ans[0] == '\'') {
		while (ans[ans.length() - 1] != '\'') {
			if (pos >= query.length()) {
				throw input_format_error();
			}
			ans = ans + " ";
			ans += getWord(pos, end_pos);
			pos = ++end_pos;
		}
	}

	return ans;
}


void Interpreter::EXEC_doCommand(string commandStr) {
    string tmp_query = commandStr;

    int check_index = 0;
    for (int i = 0; i < tmp_query.length() - 1; i++) {
        if (tmp_query[i] == '\n' && tmp_query[i + 1] == '\n')
            tmp_query.erase(i, 1);
    }
    while (check_index < tmp_query.length()) {
        query = "";
        if (tmp_query[check_index] == '\n')
            check_index++;
        while (tmp_query[check_index] != ';' && check_index < tmp_query.length()) {
            if (tmp_query[check_index] == '\n') {
                check_index++;
                continue;
            }
            query = query + tmp_query[check_index++];
        }
        check_index++;
        if (query == " " || query == "")continue;
        query = query + " \0";
        cout << ">>>" << query << ";" << endl;
        staticQuery = query;
        strNormalize();
        EXEC();
    }
}
