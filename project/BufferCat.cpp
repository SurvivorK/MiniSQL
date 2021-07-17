#include"BufferCat.h"

extern string exception_save;	//报错关键信息的储存
extern string table_name;	//表名
extern string index_id;
extern vector<string>attri_name;	//返回属性
extern vector<judge_attr>judger;	//属性大小判断
extern vector<string>input_value;	//insert用属性值
extern vector<create_attr>new_table;//创建表用，储存各属性
extern string index_attr;			//创建索引的属性名

BufferCat::BufferCat(){}

BufferCat::~BufferCat(){}

//input：文件名，文件内容
//function：在FILEPATH下新建一个文件，里面写这些内容
void BufferCat::newFile(string filename,vector<string>strlst) {
	string name = FILEPATH + filename + ".cat";
	const char *p = name.data();
	std::ofstream fout(p);
	if (fout) {
		for (int i = 0; i < strlst.size(); i++) {
			fout << strlst[i] << "\n";
		}
		fout.close();
	}
	name = filename + ".table";
	p = name.data();
	std::ofstream fout1(p);
	fout1.close();
}

//input：文件名如T1
//function：删除T1.cat文件
void BufferCat::dropFile(string filename) {
	string str = ".\\Catalog\\" + filename;
	str += ".cat";
	const char * dropPath = str.data();
	remove(dropPath);
}

//input:文件夹路径
//output:所有文件名的队列
//function:获取文件夹路径下所有文件名
void BufferCat::getFiles(string path,vector<string>&files) {
    QDir dir(QString::fromStdString(path));
    QStringList nameFilters;
    nameFilters << "*.cat";
    QStringList qfiles = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    for(int i=0;i<qfiles.size();i++){
        files.push_back(qfiles[i].toStdString());
    }
    return;
    /*
	string format = ".cat";
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*.cat").c_str(), &fileinfo))!=-1) {
		do {
			if ((fileinfo.attrib&_A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					getFiles(p.assign(path).append("\\").append(fileinfo.name),files);
				}
			}
			else {
				files.push_back(p.assign(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
    }*/
}

//input：表名
//file：表名.cat
	//#table_name
	//#属性个数n
	//#属性1名称*属性1类型*属性1unique(1/0)*属性1primary(1/0)
	//....
	//#索引个数
	//#索引1名称*索引1属性
//output：#属性1名称*属性1类型*属性1unique(1/0)*属性1primary(1/0)
//function：获取属性string
vector<string> BufferCat::getAttri(string table){
	vector<string>attri_lst;
	ifstream infile;
	string path = ".\\Catalog\\" + table;
	path += ".cat";
	infile.open(path.data());
	string tmpstr;
	getline(infile, tmpstr);
	getline(infile, tmpstr);//#n
	tmpstr.erase(0, 1);
	int attr_num = atoi(tmpstr.c_str());
	for (int i = 0; i < attr_num; i++) {
		getline(infile, tmpstr);
		attri_lst.push_back(tmpstr);
	}
	return attri_lst;
}

//input：表名
//output：#索引1名称*索引1属性
//function：获取索引string
vector<string>BufferCat::getIndex(string table) {
	vector < string> index_lst;
	ifstream infile;
	string path = ".\\Catalog\\" + table;
	path += ".cat";
	infile.open(path.data());
	string tmpstr;
	getline(infile, tmpstr);
	getline(infile, tmpstr);
	tmpstr.erase(0, 1);
	int attr_num = atoi(tmpstr.c_str());
	for (int i = 0; i < attr_num; i++) {
		getline(infile, tmpstr);
	}
	getline(infile, tmpstr);
	tmpstr.erase(0, 1);
	int index_num = atoi(tmpstr.c_str());
	for (int i = 0; i < index_num; i++) {
		getline(infile, tmpstr);
		index_lst.push_back(tmpstr);
	}
	return index_lst;
}

//input：表名，索引序列
//function：将index写入本地
void BufferCat::newIndex(string table, string strindex) {
	ifstream infile;
	string path = ".\\Catalog\\" + table;
	path += ".cat";
	infile.open(path.data());
	string tmpstr;
	getline(infile, tmpstr);
	getline(infile, tmpstr);
	tmpstr.erase(0, 1);
	int attr_num = atoi(tmpstr.c_str());
	infile.close();


	const char* filename_char;
	filename_char = path.data();
	const char* num_index;
	num_index = path.data();
	ReadLineData((char*)filename_char, 3 + attr_num,(char*)num_index);
	string strnum = num_index;
	strnum.erase(0, 1);
	int index_num = atoi(strnum.c_str());
	index_num++;
	
	string modify_num;
	modify_num = "#" + to_string(index_num);
	path = ".\\Catalog\\" + table;
	path += ".cat";
	filename_char = path.data();
	ModifyLineData((char*)filename_char, 3 + attr_num, (char*)modify_num.data());
	
	path = ".\\Catalog\\" + table;
	path += ".cat";
	ofstream write(path, ios::app);
	write << strindex<<"\n" ;
	write.close();

}

//修改文件某一行
void BufferCat::ModifyLineData(char* fileName, int lineNum, char* lineData)
{
	ifstream in;
	in.open(fileName);
	string strFileData = "";
	int line = 1;
	char tmpLineData[1024] = { 0 };
	while (in.getline(tmpLineData, sizeof(tmpLineData)))
	{
		if (line == lineNum)
		{
			strFileData += lineData;
			strFileData += "\n";
		}
		else
		{
			strFileData += tmpLineData;
			strFileData += "\n";
		}
		line++;
	}

	in.close();
	//写入文件
	ofstream out;
	out.open(fileName);
	out.flush();
	out << strFileData;
	out.close();
}


//读取某行数据
void BufferCat::ReadLineData(char* fileName, int lineNum, char* data) {
	ifstream in;
	in.open(fileName);
	int line = 1;
	while (in.getline(data, 1024))
	{
		if (lineNum == line)
		{
			break;
		}
		line++;
	}

	in.close();
}

void BufferCat::DelLineData(char* fileName, int lineNum) {
	ifstream in;	
	in.open(fileName);	
	string strFileData = "";	
	int line = 1;	
	char lineData[1024] = { 0 };	
	while (in.getline(lineData, sizeof(lineData))) { 
		if (line == lineNum) { 
			strFileData += "\n"; 
		} 
			
		else { 
			strFileData +=lineData;	
			strFileData += "\n"; 
		}		
		line++; 
	}	
	int i;
	for ( i = 0; i < strFileData.length()-1; i++) {
		if (strFileData[i] == '\n'&&strFileData[i+1] == '\n') {
			break;
		}
	}
	strFileData = strFileData.substr(0, i) + strFileData.substr(i + 1);
	in.close(); 	//写入文件	
	ofstream out;	
	out.open(fileName);	
	out.flush();	
	out<<strFileData;
	out.close();
}


//input：表名，第几个索引
//funcion：在表信息中删除一个index
void BufferCat::dropIndex(string table, int id) {
	ifstream infile;
	string path = ".\\Catalog\\" + table;
	path += ".cat";
	infile.open(path.data());
	string tmpstr;
	getline(infile, tmpstr);
	getline(infile, tmpstr);
	tmpstr.erase(0, 1);
	int attr_num = atoi(tmpstr.c_str());
	infile.close();

	const char* filename_char;
	filename_char = path.data();
	const char* num_index;
	num_index = path.data();
	ReadLineData((char*)filename_char, 3 + attr_num, (char*)num_index);
	string strnum = num_index;
	strnum.erase(0, 1);
	int index_num = atoi(strnum.c_str());
	index_num--;

	string modify_num;
	modify_num = "#" + to_string(index_num);
	path = ".\\Catalog\\" + table;
	path += ".cat";
	filename_char = path.data();
	ModifyLineData((char*)filename_char, 3 + attr_num, (char*)modify_num.data());

	//删除第id+3+attri_num行
	path = ".\\Catalog\\" + table;
	path += ".cat";
	filename_char = path.data();
	DelLineData((char*)filename_char, id + 3 + attr_num);
}

//input:表名
//function:新增一条信息后，在第一行的数字+1
void BufferCat::addRecord(string table) {
	int num;
	num = getRecordNum(table);
	num++;
	string modify_num = "#" + to_string(num);
	string path = ".\\Catalog\\" + table;
	path += ".cat";
	const char* filename_char = path.data();
	ModifyLineData((char*)filename_char, 1, (char*)modify_num.data());

}


//input：表名
//function：返回现有记录条数
int BufferCat::getRecordNum(string table) {
	ifstream infile;
	string path = ".\\Catalog\\" + table;
	path += ".cat";
	infile.open(path.data());
	string tmpstr;
	getline(infile, tmpstr);
	int num;
	tmpstr = tmpstr.substr(1);
	num = atoi(tmpstr.c_str());
	infile.close();
	return num;
}
