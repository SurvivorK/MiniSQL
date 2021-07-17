#define _CRT_SECURE_NO_WARNINGS
#include"API.h"
//extern IndexManager* myIdx;
API::API() {

}
API::~API() {
	//myIdx->~IndexManager();
	//delete myIdx;
}
API::API(string table) {//, IndexManager* &target) {
	tableName = table;
	//myIdx = target;
}

void API::dropIndex() {
	Catalog* myCat = new Catalog;
	string indexStr;
	attribute tmpattr;
	if (myCat->hasAttribute(index_attr, tmpattr)) {
		indexStr = get_standard_index(tableName, index_attr, tmpattr.type);
	}
	//传入index
	//IndexManager* myIdx = new IndexManager;
	myIdx->DropIndex(indexStr);
	myCat->dropIndex();
}

void API::createIndex() {
	Catalog* myCat = new Catalog;
	myCat->createIndex();

	string indexStr;
	attribute tmpattr;
	if (myCat->hasAttribute(index_attr, tmpattr)) {
		indexStr = get_standard_index(tableName, index_attr, tmpattr.type);
	}

	//传入index	
	//IndexManager* myIdx = new IndexManager;
	myIdx->CreateIndex(indexStr);
	vector<attribute>attribute_in_table;
	attribute_in_table = myCat->getAttribute();
	//还没想明白索引部分

	int recordNum = myCat->numRecord();//从catalog获取，只增不减
	vector<Attribute> TMP;
	TMP = transferAttri(attribute_in_table);
	Records* myRecord = new Records(tableName, TMP);

	//vector<create_attr>all_attri=myCat->getAttribute();
	int indexID;
	int attrType;
	for (int i = 0; i < attribute_in_table.size(); i++) {
		if (attribute_in_table[i].name == index_attr) {
			indexID = i;
			attrType = attribute_in_table[i].type;
			break;
		}
	}
	Record tmpRecord;
	index_node tmpnode;
	tmpnode.indexStr = indexStr;
	for (int i = 0; i < recordNum; i++) {
		if (!myRecord->get_Record(i, tmpRecord))
			continue;
		switch (attrType) {
		case -1:
			tmpnode.input_i = tmpRecord[indexID].int_data;
			break;
		case 0:
			tmpnode.input_f = tmpRecord[indexID].float_data;
			break;
		default:
			tmpnode.input_s = tmpRecord[indexID].str_data;
			break;
		}
		myIdx->Insert(indexStr, tmpnode, i);
	}
	cout << recordNum << " row(s) affected" << endl;
}

void API::deleteRecord() {
	Catalog* myCat = new Catalog;
	vector<attribute>attribute_in_table;
	attribute_in_table = myCat->getAttribute();
	//还没想明白索引部分

	int recordNum = myCat->numRecord();//从catalog获取，只增不减
	vector<Attribute> TMP;
	TMP = transferAttri(attribute_in_table);
	Records* myRecord = new Records(tableName, TMP);

	//判断条件中的属性是否存在
	attribute attri_info;
	if (judger.size() == 1 && judger[0].attr == "*") {

	}
	else {
		for (int i = 0; i < judger.size(); i++) {
			bool exist = myCat->hasAttribute(judger[i].attr, attri_info);
			if (!exist) {
				exception_save = judger[i].attr;
				throw attribute_not_exist();
			}
			judger[i].type = attri_info.type;
		}
	}


	vector<int>validRecord_id;
	vector<Record>validRecord;
	Record tmpRecord;
	for (int i = 0; i < recordNum; i++) {
		bool success_get = myRecord->get_Record(i, tmpRecord);
		if (!success_get)
			continue;
		bool tmp_valid = true;
		for (int k = 0; k < tmpRecord.size(); k++) {
			bool achieve = true;
			for (int j = 0; j < judger.size(); j++) {
				if (tmpRecord[k].attr_name == judger[j].attr) {
					if (judger[j].op == ">") {
						achieve = judge_bigger(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == ">=") {
						achieve = judge_bigger(tmpRecord[k], judger[j]);
						achieve = achieve | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "=") {
						achieve = judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<=") {
						achieve = judge_less(tmpRecord[k], judger[j]);
						achieve = achieve | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<") {
						achieve = judge_less(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<>") {
						achieve = !judge_equal(tmpRecord[k], judger[j]);
					}
					else {
						throw input_format_error();
					}
					break;
				}
			}
			success_get = success_get & achieve;
			if (!success_get)break;
		}
		if (success_get) {
			validRecord.push_back(tmpRecord);
			validRecord_id.push_back(i);
		}
	}
	//IndexManager* myIdx = new IndexManager;
	index_node tmpnode;
	string indexStr;
	for (int i = 0; i < validRecord_id.size(); i++) {
		tmpRecord = validRecord[i];
		for (int j = 0; j < tmpRecord.size(); j++) {
			if (!myCat->hasIndex(tmpRecord[j].attr_name)) {
				continue;
			}
			attribute tmpattr;
			myCat->hasAttribute(tmpRecord[j].attr_name, tmpattr);
			indexStr = get_standard_index(table_name, tmpRecord[j].attr_name, tmpattr.type);
			tmpnode.indexStr = indexStr;
			switch (tmpattr.type) {
			case -1:
				tmpnode.input_i = tmpRecord[j].int_data;
				break;
			case 0:
				tmpnode.input_f = tmpRecord[j].float_data;
				break;
			default:
				tmpnode.input_s = tmpRecord[j].str_data;
				break;
			}
			myIdx->Delete(indexStr, tmpnode);
		}
		myRecord->delete_one_record(validRecord_id[i]);
	}
	cout << validRecord_id.size() << " row(s) affected" << endl;
}

void API::dropTable() {
	Catalog* myCat = new Catalog;

	string indexStr;
	vector<index>all_index;
	all_index = myCat->getIndex();
	//IndexManager* myIdx = new IndexManager;
	for (int i = 0; i < all_index.size(); i++) {
		attribute tmpattr;
		if (myCat->hasAttribute(all_index[i].attri_name, tmpattr)) {
			indexStr = get_standard_index(tableName, all_index[i].attri_name, tmpattr.type);
		}
		//投入index删除一个索引
		myIdx->DropIndex(indexStr);
	}
	global_buffer.Buffer_flush();
	remove((this->tableName + ".table").c_str());
	myCat->dropTable();
	cout << "0 row(s) affected" << endl;
}

void API::createTable() {
	Catalog* myCat = new Catalog;
	myCat->createTable();

	//Index
	string indexStr;

	for (int i = 0; i < new_table.size(); i++) {
		if (new_table[i].primary_key) {
			//IndexManager* myIdx = new IndexManager;
			indexStr = get_standard_index(tableName, new_table[i].name, new_table[i].type);
			index_attr = new_table[i].name;
			index_id = "primary";
			Catalog* newCat = new Catalog;
			newCat->createIndex();
			myIdx->CreateIndex(indexStr);
			break;
		}
	}
	cout << "0 row(s) affected" << endl;
}



//input：basic中的attri_name/judger
void API::selectRecord() {
	int index_pos = -1;//where语句中出现第一个索引属性的位置记录
	Catalog* myCat = new Catalog;



	attribute attri_info;
	for (int i = 0; i < judger.size(); i++) {
		bool exist = myCat->hasAttribute(judger[i].attr, attri_info);
		if (!exist) {
			exception_save = judger[i].attr;
			throw attribute_not_exist();
		}
		judger[i].type = attri_info.type;
		if (myCat->hasIndex(judger[i].attr)) {
			index_pos = i;
		}
	}

	vector<Record>ansRecord;
	if (index_pos > -1) {
		//有索引
		ansRecord = selectRecord_idx(index_pos);
	}
	else {
		//暴力搜索
		ansRecord = selectRecord_noidx();
	}
	if (!check_key)
		showinfo(ansRecord);
}

void API::showinfo(vector<Record>input_Record) {
	if (attri_name.size() == 1 && attri_name[0] == "*") {
		//select*from
		//输出所有属性名
		Catalog* myCat = new Catalog;
		vector<attribute>attribute_in_table;
		attribute_in_table = myCat->getAttribute();

		cout.setf(std::ios::left);
		string line;
		line.append(attribute_in_table.size() * 15, '-');
		cout << line << endl;
		for (int i = 0; i < attribute_in_table.size(); i++) {
			cout << "|";
			cout.width(14);
			cout << attribute_in_table[i].name;
		}
		cout << endl;
		cout << line << endl;

		for (int i = 0; i < input_Record.size(); i++) {
			Record tmpRecord;
			tmpRecord = input_Record[i];

			for (int k = 0; k < tmpRecord.size(); k++) {
				if (tmpRecord[k].ktype == KTP::INT) {
					cout << "|";
					cout.width(14);
					cout << tmpRecord[k].int_data;
				}
				else if (tmpRecord[k].ktype == KTP::FLOAT) {
					cout << "|";
					cout.width(14);
					cout << tmpRecord[k].float_data;
				}
				if (tmpRecord[k].ktype == KTP::STRING) {
					cout << "|";
					cout.width(14);
					cout << tmpRecord[k].str_data;
				}
			}
			cout << endl;
		}
		cout << line << endl;
	}
	else {
		string line;
		line.append(attri_name.size() * 15, '-');
		cout << line << endl;
		for (int i = 0; i < attri_name.size(); i++) {
			cout << "|";
			cout.width(14);
			cout << attri_name[i];
		}
		cout << endl;
		cout << line << endl;
		for (int i = 0; i < input_Record.size(); i++) {
			Record tmpRecord;
			tmpRecord = input_Record[i];
			for (int j = 0; j < attri_name.size(); j++) {
				for (int k = 0; k < tmpRecord.size(); k++) {
					if (tmpRecord[k].attr_name == attri_name[j]) {
						if (tmpRecord[k].ktype == KTP::INT) {
							cout << "|";
							cout.width(14);
							cout << tmpRecord[k].int_data;
						}
						else if (tmpRecord[k].ktype == KTP::FLOAT) {
							cout << "|";
							cout.width(14);
							cout << tmpRecord[k].float_data;
						}
						if (tmpRecord[k].ktype == KTP::STRING) {
							cout << "|";
							cout.width(14);
							cout << tmpRecord[k].str_data;
						}
						break;
					}
				}
			}
			cout << endl;
		}
		cout << line << endl;
	}
	cout << input_Record.size() << " row(s) returned" << endl;
}

vector<Attribute> API::transferAttri(vector<create_attr> attribute_in_table) {
	vector<Attribute> TMP;
	Catalog* myCat = new Catalog;
	for (int i = 0; i < attribute_in_table.size(); i++) {
		Attribute newatt;
		newatt.attribute_name = attribute_in_table[i].name;
		if (attribute_in_table[i].type == -1) {
			newatt.type = KTP::INT;
		}
		else if (attribute_in_table[i].type == 0) {
			newatt.type = KTP::FLOAT;
		}
		else {
			newatt.type = KTP::STRING;
			newatt.str_len = attribute_in_table[i].type - 1;
		}

		if (attribute_in_table[i].unique) {
			newatt.is_unique = true;
		}
		else {
			newatt.is_unique = false;
		}

		if (attribute_in_table[i].primary_key) {
			newatt.is_PK = true;
		}
		else {
			newatt.is_PK = false;
		}

		if (myCat->hasIndex(newatt.attribute_name)) {
			newatt.has_idx = true;
		}
		else {
			newatt.has_idx = false;
		}
		TMP.push_back(newatt);
	}
	return TMP;
}
vector<Record> API::selectRecord_noidx() {
	Catalog* myCat = new Catalog;
	int recordNum = myCat->numRecord();//从catalog获取，只增不减
	vector<attribute>attribute_in_table;
	attribute_in_table = myCat->getAttribute();
	vector<Attribute> TMP;
	TMP = transferAttri(attribute_in_table);
	Records* myRecord = new Records(tableName, TMP);

	//判断条件中的属性是否存在
	attribute attri_info;
	for (int i = 0; i < judger.size(); i++) {
		bool exist = myCat->hasAttribute(judger[i].attr, attri_info);
		if (!exist) {
			exception_save = judger[i].attr;
			throw attribute_not_exist();
		}
		judger[i].type = attri_info.type;
	}

	vector<Record>validRecord;
	Record tmpRecord;

	bool* valid = new bool[recordNum];
	vector<Record> Record_after_index;
	for (int i = 0; i < recordNum; i++) {
		if (!myRecord->get_Record(i, tmpRecord))
			continue;
		//myRecord->get_Record(recordNum, tmpRecord);
		Record_after_index.push_back(tmpRecord);
		valid[i] = true;
	}
	for (int j = 0; j < judger.size(); j++) {
		for (int k = 0; k < attribute_in_table.size(); k++) {
			if (judger[j].attr == attribute_in_table[k].name) {
				for (int i = 0; i < Record_after_index.size(); i++) {
					tmpRecord = Record_after_index[i];
					if (!valid[i])continue;
					if (judger[j].op == ">") {
						valid[i] = judge_bigger(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == ">=") {
						valid[i] = judge_bigger(tmpRecord[k], judger[j]);
						valid[i] = valid[i] | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "=") {
						valid[i] = judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<=") {
						valid[i] = judge_less(tmpRecord[k], judger[j]);
						valid[i] = valid[i] | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<") {
						valid[i] = judge_less(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<>") {
						valid[i] = !judge_equal(tmpRecord[k], judger[j]);
					}
					else {
						throw input_format_error();
					}
				}
				break;
			}
		}
	}
	for (int i = 0; i < Record_after_index.size(); i++) {
		if (valid[i])
			validRecord.push_back(Record_after_index[i]);
	}

	/*for (int i = 0; i < recordNum; i++) {
		bool success_get = myRecord->get_Record(i, tmpRecord);
		bool tmp_valid = true;
		for (int k = 0; k < tmpRecord.size(); k++) {
			bool achieve = true;
			for (int j = 0; j < judger.size(); j++) {
				if (tmpRecord[k].attr_name == judger[j].attr) {
					if (judger[j].op == ">") {
						achieve = judge_bigger(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == ">=") {
						achieve = judge_bigger(tmpRecord[k], judger[j]);
						achieve = achieve | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "=") {
						achieve = judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<=") {
						achieve = judge_less(tmpRecord[k], judger[j]);
						achieve = achieve | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<") {
						achieve = judge_less(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<>") {
						achieve = !judge_equal(tmpRecord[k], judger[j]);
					}
					else {
						throw input_format_error();
					}
					break;
				}
			}
			success_get = success_get & achieve;
			if (!success_get)break;
		}
		if (success_get)
			validRecord.push_back(tmpRecord);
	}*/

	return validRecord;
}

vector<Record>API::selectRecord_idx(int index_pos) {
	Catalog* myCat = new Catalog;
	int recordNum = myCat->numRecord();//从catalog获取，只增不减
	vector<attribute>attribute_in_table;
	attribute_in_table = myCat->getAttribute();
	vector<Attribute> TMP;
	TMP = transferAttri(attribute_in_table);
	Records* myRecord = new Records(tableName, TMP);

	//判断条件中的属性是否存在
	attribute attri_info;
	for (int i = 0; i < judger.size(); i++) {
		bool exist = myCat->hasAttribute(judger[i].attr, attri_info);
		if (!exist) {
			exception_save = judger[i].attr;
			throw attribute_not_exist();
		}
		judger[i].type = attri_info.type;
	}

	//IndexManager* myIdx = new IndexManager;

	string indexStr;
	OP judger_op;
	string value;
	index_node tmpnode;
	for (int i = 0; i < judger.size(); i++) {
		if (myCat->hasIndex(judger[i].attr)) {
			index_pos = i;
			indexStr = get_standard_index(tableName, judger[i].attr, judger[i].type);
			tmpnode.indexStr = indexStr;
			if (judger[i].op == "=")judger_op = OP::EQUAL;
			else if (judger[i].op == "<")judger_op = OP::SMALLER;
			else if (judger[i].op == ">")judger_op = OP::GREATER;
			else if (judger[i].op == "<=")judger_op = OP::LEQ;
			else if (judger[i].op == ">=")judger_op = OP::GEQ;
			else if (judger[i].op == "<>")judger_op = OP::NEQ;
			value = judger[i].value;
			switch (judger[i].type) {
			case -1:
				tmpnode.input_i = atoi(judger[i].value.c_str());
				break;
			case 0:
				tmpnode.input_f = atof(judger[i].value.c_str());
				break;
			default:
				tmpnode.input_s = judger[i].value.substr(1, judger[i].value.length() - 2);
				break;
			}
			break;
		}
	}
	vector<int>offset_lst = myIdx->Search(indexStr, tmpnode, judger_op);
	vector<Record> validRecord;
	if (offset_lst[0] == -1) {
		exist_key = false;
		return validRecord;
	}
	else {
		exist_key = true;
	}
	//indexStr为传入的table_key_type.idx名
	//judger_op为符号
	//value为判断句的值
	//传入index->获得一组offset->传入record->获取一堆data->对data进行除了index以外的条件判断
	//******************************************
	
	Record tmpRecord;
	bool* valid = new bool[offset_lst.size()];
	vector<Record> Record_after_index;
	for (int i = 0; i < offset_lst.size(); i++) {
		if (!myRecord->get_Record(offset_lst[i], tmpRecord))
			continue;
		Record_after_index.push_back(tmpRecord);
		valid[i] = true;
	}
	for (int j = 0; j < judger.size(); j++) {
		if (j == index_pos)continue;
		for (int k = 0; k < attribute_in_table.size(); k++) {
			if (judger[j].attr == attribute_in_table[k].name) {
				for (int i = 0; i < Record_after_index.size(); i++) {
					tmpRecord = Record_after_index[i];
					if (!valid[i])continue;
					if (judger[j].op == ">") {
						valid[i] = judge_bigger(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == ">=") {
						valid[i] = judge_bigger(tmpRecord[k], judger[j]);
						valid[i] = valid[i] | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "=") {
						valid[i] = judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<=") {
						valid[i] = judge_less(tmpRecord[k], judger[j]);
						valid[i] = valid[i] | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<") {
						valid[i] = judge_less(tmpRecord[k], judger[j]);
					}
					else if (judger[j].op == "<>") {
						valid[i] = !judge_equal(tmpRecord[k], judger[j]);
					}
					else {
						throw input_format_error();
					}
				}
				break;
			}
		}
	}
	for (int i = 0; i < Record_after_index.size(); i++) {
		if (valid[i])
			validRecord.push_back(Record_after_index[i]);
	}

	/*for (int i = 0; i < offset_lst.size(); i++) {
		myRecord->get_Record(offset_lst[i], tmpRecord);
		bool success_get = myRecord->get_Record(i, tmpRecord);
		bool tmp_valid = true;
		for (int k = 0; k < tmpRecord.size(); k++) {
			bool achieve = true;
			for (int j = 0; j < judger.size(); j++) {
				if (tmpRecord[k].attr_name == judger[j].attr) {
					if (j = index_pos)break;
					if (judger[i].op == ">") {
						achieve = judge_bigger(tmpRecord[k], judger[j]);
					}
					else if (judger[i].op == ">=") {
						achieve = judge_bigger(tmpRecord[k], judger[j]);
						achieve = achieve | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[i].op == "=") {
						achieve = judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[i].op == "<=") {
						achieve = judge_less(tmpRecord[k], judger[j]);
						achieve = achieve | judge_equal(tmpRecord[k], judger[j]);
					}
					else if (judger[i].op == "<") {
						achieve = judge_less(tmpRecord[k], judger[j]);
					}
					else if (judger[i].op == "<>") {
						achieve = !judge_equal(tmpRecord[k], judger[j]);
					}
					else {
						throw input_format_error();
					}
					break;
				}
			}
			success_get = success_get & achieve;
			if (!success_get)break;
		}
		if (success_get)
			validRecord.push_back(tmpRecord);
	}*/
	return validRecord;

}

//vector<string>input_value
void API::insertRecord() {
	//调用catalog在记录中+1
	Catalog* myCat = new Catalog;
	//myCat->addRecord();
	int recordNum = myCat->numRecord();//从catalog获取，只增不减
	vector<attribute>attribute_in_table;
	attribute_in_table = myCat->getAttribute();
	vector<Attribute> TMP;
	TMP = transferAttri(attribute_in_table);
	Records* myRecord = new Records(tableName, TMP);

	if (attribute_in_table.size() < input_value.size()) {
		throw column_not_match();
	}

	Record cur;
	for (int i = 0; i < input_value.size(); i++) {
		KEY key;
		key.attr_name = attribute_in_table[i].name;
		switch (attribute_in_table[i].type) {
		case -1:
			key.ktype = KTP::INT;
			key.int_data = atoi(input_value[i].c_str());
			break;
		case 0:
			key.ktype = KTP::FLOAT;
			key.float_data = atof(input_value[i].c_str());
			break;
		default:
			key.ktype = KTP::STRING;
			key.len = attribute_in_table[i].type - 1;
			if (input_value[i][0] == '"' && input_value[i][input_value[i].size() - 1] == '"') {
			}
			else if (input_value[i][0] == '\'' && input_value[i][input_value[i].size() - 1] == '\'') {
			}
			else {
				exception_save = attribute_in_table[i].name;
				throw data_type_conflict();
			}
			input_value[i] = input_value[i].substr(1, input_value[i].size() - 2);
			if (input_value[i].size() > key.len) {
				exception_save = attribute_in_table[i].name;
				throw data_too_long();
			}
			//key.str_data = input_value[i].c_str();
			strcpy(key.str_data, input_value[i].c_str());
			break;
		}
		cur.push_back(key);
	}

	//完整性约束
	check_key = true;
	exist_key = false;
	judge_attr keyjudge;
	keyjudge.attr = "id";
	keyjudge.op = "=";
	keyjudge.value = input_value[0];
	judger.clear();
	judger.push_back(keyjudge);
	selectRecord();
	if (exist_key == true) {
		exception_save = "id";
		throw primary_key_conflict();
	}


	myRecord->insertRecord(&cur, recordNum);
	global_buffer.Buffer_flush();

	//传入index新的节点

	string indexStr;
	int input_int;
	float input_float;
	string input_str;
	//IndexManager* myIdx = new IndexManager;
	for (int i = 0; i < attribute_in_table.size(); i++) {
		if (myCat->hasIndex(attribute_in_table[i].name)) {
			index_node tmpnode;
			tmpnode.indexStr = get_standard_index(table_name, attribute_in_table[i].name, attribute_in_table[i].type);
			if (attribute_in_table[i].type == -1) {
				tmpnode.input_i = atoi(input_value[i].c_str());
			}
			else if (attribute_in_table[i].type == 0) {
				tmpnode.input_f = atof(input_value[i].c_str());
			}
			else {
				tmpnode.input_s = input_value[i].substr(0, input_value[i].length());
				cout << tmpnode.input_s << endl;
			}

			//index新增节点
			myIdx->Insert(tmpnode.indexStr, tmpnode, recordNum);
		}
	}

	myCat->addRecord();
	cout << "1 row(s) affected" << endl;
}


bool API::judge_less(KEY key, judge_attr j) {
	if (j.type == -1) {
		//int
		if (key.int_data < atoi(j.value.c_str()))
			return true;
	}
	else if (j.type == 0) {
		//float
		if (key.float_data < atof(j.value.c_str()))
			return true;
	}
	else {
		if (key.str_data < j.value.substr(1, j.value.size() - 1))
			return true;
		//char
	}
	return false;
}

bool API::judge_bigger(KEY key, judge_attr j) {
	if (j.type == -1) {
		//int
		if (key.int_data > atoi(j.value.c_str()))
			return true;
	}
	else if (j.type == 0) {
		//float
		if (key.float_data > atof(j.value.c_str()))
			return true;
	}
	else {
		if (key.str_data > j.value.substr(1, j.value.size() - 1))
			return true;
		//char
	}
	return false;
}

bool API::judge_equal(KEY key, judge_attr j) {
	if (j.type == -1) {
		//int
		if (key.int_data == atoi(j.value.c_str()))
			return true;
	}
	else if (j.type == 0) {
		//float
		if (key.float_data == atof(j.value.c_str()))
			return true;
	}
	else {
		if (key.str_data == j.value.substr(1, j.value.size() - 2))
			return true;
		//char
	}
	return false;
}


string API::get_standard_index(string table, string key, int type) {
	string stdIndex;
	stdIndex = table + "_";
	stdIndex += key;
	stdIndex += "_";
	if (type == 0) {
		stdIndex += "float";
	}
	else if (type == -1) {
		stdIndex += "int";
	}
	else {
		stdIndex += "char";
		stdIndex += to_string(type - 1);
	}
	stdIndex += ".idx";
	return stdIndex;
}

