/*
 * @Author: your name
 * @Date: 2020-05-30 09:37:24
 * @LastEditTime: 2020-06-02 14:04:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \miniSql\index\IndexManager.h
 */

#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "BPlusTree.h"
#include "basic.h"
using namespace std;


#define INTSIZE  4
#define FLOATSIZE 4
#define CHARSIZE 4

class IndexManager {
   private:
    map<string, BPlusTree<int>*> IntIndexMap;
    map<string, BPlusTree<float>*> FloatIndexMap;
    map<string, BPlusTree<string>*> StringIndexMap;
	map<string, int> maxid;
   public:
    IndexManager();
    ~IndexManager();
    void CreateIndex(string filename);
    void DropIndex(string filename);
	void ShowIndex(string filename);
    void SaveToFile();

    vector<int> Search(string filename, index_node val, OP op);

    //template <typename T>
    void Insert(string filename, index_node val, int blockoffset);

	//template <typename T>
	void Delete(string filename, index_node & val);

	IFStype SplitString(string str);
};



#endif