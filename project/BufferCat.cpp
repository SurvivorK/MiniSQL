#include"BufferCat.h"

extern string exception_save;	//����ؼ���Ϣ�Ĵ���
extern string table_name;	//����
extern string index_id;
extern vector<string>attri_name;	//��������
extern vector<judge_attr>judger;	//���Դ�С�ж�
extern vector<string>input_value;	//insert������ֵ
extern vector<create_attr>new_table;//�������ã����������
extern string index_attr;			//����������������

BufferCat::BufferCat(){}

BufferCat::~BufferCat(){}

//input���ļ������ļ�����
//function����FILEPATH���½�һ���ļ�������д��Щ����
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

//input���ļ�����T1
//function��ɾ��T1.cat�ļ�
void BufferCat::dropFile(string filename) {
	string str = ".\\Catalog\\" + filename;
	str += ".cat";
	const char * dropPath = str.data();
	remove(dropPath);
}

//input:�ļ���·��
//output:�����ļ����Ķ���
//function:��ȡ�ļ���·���������ļ���
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

//input������
//file������.cat
	//#table_name
	//#���Ը���n
	//#����1����*����1����*����1unique(1/0)*����1primary(1/0)
	//....
	//#��������
	//#����1����*����1����
//output��#����1����*����1����*����1unique(1/0)*����1primary(1/0)
//function����ȡ����string
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

//input������
//output��#����1����*����1����
//function����ȡ����string
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

//input����������������
//function����indexд�뱾��
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

//�޸��ļ�ĳһ��
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
	//д���ļ�
	ofstream out;
	out.open(fileName);
	out.flush();
	out << strFileData;
	out.close();
}


//��ȡĳ������
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
	in.close(); 	//д���ļ�	
	ofstream out;	
	out.open(fileName);	
	out.flush();	
	out<<strFileData;
	out.close();
}


//input���������ڼ�������
//funcion���ڱ���Ϣ��ɾ��һ��index
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

	//ɾ����id+3+attri_num��
	path = ".\\Catalog\\" + table;
	path += ".cat";
	filename_char = path.data();
	DelLineData((char*)filename_char, id + 3 + attr_num);
}

//input:����
//function:����һ����Ϣ���ڵ�һ�е�����+1
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


//input������
//function���������м�¼����
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
