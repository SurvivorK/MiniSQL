#include "IndexManager.h"
#include "BPlusTree.h"
#include "basic.h"

// Get all existing index and initialize them
IndexManager::IndexManager() {}

IndexManager::~IndexManager() {
    
}
void IndexManager::SaveToFile() {
    for (auto i : IntIndexMap)
        if (i.second) {
            i.second->write_to_file();
            delete i.second;
        }
    for (auto i : FloatIndexMap)
        if (i.second) {
            i.second->write_to_file();
            delete i.second;
        }
    for (auto i : StringIndexMap)
        if (i.second) {
            i.second->write_to_file();
            delete i.second;
        }
}
IFStype IndexManager::SplitString(string filename) {
    string str = filename;
    if (str.find("_int.") != string::npos) return IFStype::INT;
    if (str.find("_float.") != string::npos) return IFStype::FLOAT;
    if (str.find("_char") != string::npos) return IFStype::CHAR;
    return IFStype::UNDEF;
}

void IndexManager::CreateIndex(string filename) {
    IFStype t = SplitString(filename);
    switch (t) {
    case IFStype::INT: {
        tot = 0;
        BPlusTree<int>* newtree =
            new BPlusTree<int>(filename, INTSIZE, 4, 4);

        maxid[filename] = tot;
        IntIndexMap[filename] = newtree;
        break;
    }
    case IFStype::FLOAT: {
        tot = 0;
        BPlusTree<float>* newtree1 =
            new BPlusTree<float>(filename, FLOATSIZE, 4, 4);

        maxid[filename] = tot;
        FloatIndexMap[filename] = newtree1;
        break;
    }
    case IFStype::CHAR: {
        tot = 0;
        BPlusTree<string>* newtree2 =
            new BPlusTree<string>(filename, CHARSIZE, 4, 4);

        maxid[filename] = tot;
        StringIndexMap[filename] = newtree2;
        break;
    }
    }
}

void IndexManager::DropIndex(string filename) {
    IFStype t = SplitString(filename);
    switch (t) {
    case IFStype::INT: {
        map<string, BPlusTree<int>*>::iterator it =
            IntIndexMap.find(filename);
        if (it != IntIndexMap.end()) {
            delete it->second;
            IntIndexMap.erase(it);
            map<string, int>::iterator idit = maxid.find(filename);
            maxid.erase(idit);   
        }
        remove(filename.c_str());
        break;
    }
    case IFStype::FLOAT: {
        map<string, BPlusTree<float>*>::iterator it1 =
            FloatIndexMap.find(filename);
        if (it1 != FloatIndexMap.end()) {
            delete it1->second;
            FloatIndexMap.erase(it1);

            map<string, int>::iterator idit = maxid.find(filename);
            maxid.erase(idit);
        }
        remove(filename.c_str());
        break;
    }
    case IFStype::CHAR: {
        map<string, BPlusTree<string>*>::iterator it2 =
            StringIndexMap.find(filename);
        if (it2 != StringIndexMap.end()) {
            delete it2->second;
            StringIndexMap.erase(it2);

            map<string, int>::iterator idit = maxid.find(filename);
            maxid.erase(idit);
        }
        remove(filename.c_str());
        break;
    }
    }
}
vector<int> IndexManager::Search(string filename, index_node val, OP op) {
    vector<int> res; res.clear();
    IFStype t = SplitString(filename);
    switch (t) {
    case IFStype::INT: {
        if (!IntIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<int>* newtree = IntIndexMap[filename];
        res = newtree->searchrange(val.input_i, op);
        //return newtree->searchrange(val.input_i, op);
        break;
    }
    case IFStype::FLOAT: {
        if (!FloatIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<float>* newtree1 = FloatIndexMap[filename];
        res = newtree1->searchrange(val.input_f, op);
        //return  newtree1->searchrange(val.input_f, op);
        break;
    }
    case IFStype::CHAR: {
        if (!StringIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<string>* newtree2 = StringIndexMap[filename];
        res = newtree2->searchrange(val.input_s, op);
        //return  newtree2->searchrange(val.input_s, op);
        break;
    }
    }   
    //for (auto i : res) cout << i << " "; cout << endl;
    //return res;
    return res;
}

void IndexManager::Insert(string filename, index_node val, int blockoffset) {
    IFStype t = SplitString(filename);
    switch (t) {
    case IFStype::INT: {
        if (!IntIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<int>* newtree = IntIndexMap[filename];

        tot = maxid[filename];
       // cout << tot << endl;
       // if (val.input_i == 1080100999)
       //      puts("fuck");
        newtree->insert(val.input_i, blockoffset);

        maxid[filename] = tot;
        break;
    }
    case IFStype::FLOAT: {
        if (!FloatIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<float>* newtree1 = FloatIndexMap[filename];

        tot = maxid[filename];
        newtree1->insert(val.input_f, blockoffset);
        maxid[filename] = tot;

        break;
    }
    case IFStype::CHAR: {
        if (!StringIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<string>* newtree2 = StringIndexMap[filename];
        //if (val.input_s == "name10000")
        //    puts("fuck");
        tot = maxid[filename];
        newtree2->insert(val.input_s, blockoffset);
        maxid[filename] = tot;
        break;
    }
    }
    // return true;
}

void IndexManager::Delete(string filename, index_node& val) {
    IFStype t = SplitString(filename);
    switch (t) {
    case IFStype::INT: {
        if (!IntIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<int>* newtree = IntIndexMap[filename];
        // for (auto i : val) newtree->Remove(i.input_i);
       // if (val.input_i == 1080100999)
       //     puts("fuck");
        newtree->Remove(val.input_i);
       // cout << *(newtree->root) << endl;
        break;
    }
    case IFStype::FLOAT: {
        if (!FloatIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<float>* newtree1 = FloatIndexMap[filename];
        //for (auto i : val) newtree1->Remove(i.input_f);
        newtree1->Remove(val.input_f);
        break;
    }
    case IFStype::CHAR: {
        if (!StringIndexMap.count(filename)) CreateIndex(filename);
        BPlusTree<string>* newtree2 = StringIndexMap[filename];
        //for (auto i : val) newtree2->Remove(i.input_s);
        newtree2->Remove(val.input_s);
        
        break;
    }
    }
}

void IndexManager::ShowIndex(string filename) {
    IFStype t = SplitString(filename);
    switch (t) {
    case IFStype::INT: {
        map<string, BPlusTree<int>*>::iterator it =
            IntIndexMap.find(filename);
        if (it == IntIndexMap.end()) {
            puts("Index not found!");
            return;
        }
        it->second->Print();
        break;
    }
    case IFStype::FLOAT: {
        map<string, BPlusTree<float>*>::iterator it1 =
            FloatIndexMap.find(filename);
        if (it1 == FloatIndexMap.end()) {
            puts("Index not found!");
            return;
        }
        it1->second->Print();
        break;
    }
    case IFStype::CHAR: {
        map<string, BPlusTree<string>*>::iterator it2 =
            StringIndexMap.find(filename);
        if (it2 == StringIndexMap.end()) {
            puts("Index not found!");
            return;
        }
        it2->second->Print();
        break;
    }
    }
}