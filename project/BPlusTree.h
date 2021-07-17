#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include "BPlusTreeNode.h"
#include "basic.h"
using namespace std;

#define pni pair<Node<T>*, int>




//enum Message { FAILED, DELETE, CHANGE, REPLACE, NONE };
#define FAILED  0
#define DELETE  1
#define CHANGE  2
#define REPLACE  3
#define NONE  4

template <typename T>
class BPlusTree {
    typedef Node<T>* node;
	
   private:
    
	//enum type TYPE;
    node head, tail;
    int keysz, degree, order;
    int key_cnt, node_cnt;
    string idx_file;
	T tempkey;
	

   public:
   	node root;
    BPlusTree(string _idx_file_ = "", int _keysz = 8, int _degree = 4,
              int _order = 4);
    ~BPlusTree();
    void insert(T val, int blockoffset);
    pni find(T val, node now);
	int find(T val);
    int del(node now, node brother, T Parentkey);
	int Remove(T val);
    void split_update(node now);
    void Print();

	void update(node now, T oldval, T newval);
	void write_to_file();
	void write_id(ofstream &fp, node now);
	void write(ofstream &fp, node now);
	bool read(ofstream &fp, node &now);
	bool load_from_file();
	void drop(node now);
	vector<int> searchrange(T val, OP op);
   private:
    //void init();
};

template <typename T>
BPlusTree<T>::BPlusTree(string _idx_file_, int _keysz, int _degree, int _order)
    : idx_file(_idx_file_), keysz(_keysz), degree(_degree), order(_order) {
    if (load_from_file()) return;
	key_cnt = 0;
	node_cnt = 1;
	key_cnt = 0;
	head = new Node<T>(0, order, degree, 0, NULL, NULL, NULL, true);
    tail = new Node<T>(0, order, degree, 0, NULL, NULL, NULL, true);
	root = new Node<T>(0, order, degree, 0, NULL, tail, head, true);
    head->next = root;
    tail->prev = root;
    //root = NULL;
    
}

// template <typename T>
// void BPlusTree<T>::init() {
    
//     head->next = root;
//     tail->prev = root;
//     key_cnt = 1;
//     node_cnt = 1;
// }
template <typename T>
vector<int> BPlusTree<T>::searchrange(T val, OP op){
	vector<int> res;
	switch (op){
		case OP::GREATER:{
			int pos;
			pni ret = find(val, root);
			Node<T>* o = ret.first->prev;
			for (; o != tail; o = o->next) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] > val) res.push_back(o->blkptr[j]);
			}
			return res;
		}
		case OP::SMALLER:{
			int pos;
			pni ret = find(val, root);
			Node<T>* o = ret.first->next;
			for (; o != head; o = o->prev) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] < val) res.push_back(o->blkptr[j]);
			}
			return res;
		}
		case OP::EQUAL:{
			res.push_back(find(val));
			return res;
		}
		case OP::LEQ:{
			int pos;
			pni ret = find(val, root);
			Node<T>* o = ret.first->next;
			for (; o != head; o = o->prev) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] <= val) res.push_back(o->blkptr[j]);
			}
			return res;
		}
		case OP::GEQ:{
			int pos;
			pni ret = find(val, root);
			Node<T>* o = ret.first->prev;
			for (; o != tail; o = o->next) {
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] >= val) res.push_back(o->blkptr[j]);
			}
			return res;
		}
		case OP::NEQ:{
			for (Node<T>* o = head->next; o != tail; o = o->next){
				for (int j = 0; j < o->cnt; j++) 
					if (o->keys[j] != val) res.push_back(o->blkptr[j]);
			}
			return res;
		}
	}
	return res;
}

template <typename T>
int BPlusTree<T>::find(T val){
	pni res = find(val, root);
	node now = res.first;
	int pos = res.second;
	if (pos >= now->cnt	|| now->keys[pos] != val) return -1;
	else if (now->keys[pos] == val) return now->blkptr[pos];
	return -1;
}
/**
 * @description: find a key in the b+ tree
 * @param T val: to be found
 * 		  node now: present searching node
 * @return: <node, pos>: the exact position of val
 * 			if not found, return the position < val
 */
template <typename T>
pni BPlusTree<T>::find(T val, node now) {
    typename vector<T>::iterator it;
    if (now->is_leaf) {
        // it = lower_bound(now->values.begin(), now->values.end(), val);
        // return make_pair(now, it - now->values.begin());
        int pos = -1;
        now->find(val, pos);
        return make_pair(now, pos);
    }
    // it = upper_bound(now->keys.begin(), now->keys.end(), val);
    // return find(val, now->ch[it - now->keys.begin()]);
    int pos = -1;
    if (now->find(val, pos)) pos++;
    return find(val, now->childrens[pos]);
}

/**
 * @description: insert a value into the tree
 * @param T val: to be inserted
 * @param int blockoffset : the position of the key
 * @return: None
 */
template <typename T>
void BPlusTree<T>::insert(T val, int blockoffset) {
    // if (!root) {
    //     init();
    //     root->insert(val, blockoffset);
    //     return;
    // }
    pni res = find(val, root);
    node o = res.first;
    int pos = res.second;
    if (pos < o->keys.size() && o->keys[pos] == val) {
        //puts("Error: The key already exists!");
        //printf("Key %d is duplicated\n", val);
        return;
    }
    // To be improved
    // but seems useless
    o->insert(val, blockoffset);
    key_cnt++;
    if (o->is_overfull()) {
        split_update(o);
    }
    return;
}
/**
 * @description: split recursively from the leave of the tree
 * @param node now: node is being splitted
 * @return: None
 */
template <typename T>
void BPlusTree<T>::split_update(node now) {
    node fa = now->father;
    if (now->is_overfull()) {
        T mid_val;
		node_cnt++;
        node newnow = now->split(mid_val);
        if (!fa) {
            node newroot =
                new Node<T>(0, order, degree, 0, NULL, NULL, NULL, false);
            root = newroot;
			node_cnt++;
            newroot->childrens.push_back(now);
            newroot->insert(mid_val,0,  newnow);
            newnow->father = now->father = newroot;
        } else {
            fa->insert(mid_val, 0, newnow);
            split_update(fa);
        }
    }
    else return;
}

/**
 * @description: find and delete the key from a node on the tree
 * @param T val: key to be deleted 
 * @return: None
 */
template <typename T>
void BPlusTree<T>::update(node now, T oldval, T newval){
	if (!now) return;
	int pos = 0;
	if (now->find(oldval, pos)) {now->keys[pos] = newval; return;}
	update(now->father, oldval, newval);
}

template <typename T>
int BPlusTree<T>::del(node now, node brother, T Parentkey){
	int pos = upper_bound(now->keys.begin(), now->keys.end(), tempkey) - now->keys.begin();
	bool find_flag = pos == 0 ? false : now->keys[pos - 1] == tempkey;
	//enum Message res;
	int res;
	if (now->is_leaf) {
		res = FAILED;
		if (find_flag) res = DELETE;
	}
	else {
		if (find_flag) res = del(now->childrens[pos], now->childrens[pos - 1], now->keys[pos - 1]);
		else {
			if (pos == now->cnt) res = del(now->childrens[pos], now->childrens[pos - 1], now->keys[pos - 1]);
			else res = del(now->childrens[pos], now->childrens[pos + 1], now->keys[pos]);
		}
	}
	if (pos == now->cnt || find_flag) pos--;
	//enum Message ret = NONE;
	int ret = NONE;
	T entry_replace_val = tempkey;
	bool flag = find_flag && pos == 0 && now->is_leaf;

	switch (res)
	{
	case FAILED:
		ret = FAILED;
		break;
	case CHANGE:
		//now->replace(now->childrens[pos], tempkey);
		now->keys[pos] = tempkey;
		ret = NONE;
		break;
	case DELETE:
		now->del(pos);
		if (now == root){
			if (now->cnt == 0) {
				if (now->is_leaf) {}
				else {
					root = now->childrens.front();
					root->father = NULL;
					node_cnt--;
					delete now;
				}
				
				
			}
			ret = NONE;
		}
		else {
			int lbound = (order + 1) / 2 - 1;
			if (now->cnt < lbound) {
				if (brother->cnt > lbound) { // append
					if (now->next == brother) { //right brother
						if (now->is_leaf) {
							now->keys.push_back(brother->keys.front());
							now->blkptr.push_back(brother->blkptr.front());
							now->cnt++;
							brother->keys.erase(brother->keys.begin());
							brother->blkptr.erase(brother->blkptr.begin());
							brother->cnt--;

							tempkey = brother->keys.front();
							if (flag) update(now->father, entry_replace_val, now->keys.front());
						}
						else {
							//now->insert(Parentkey, brother->childrens.front());
							now->keys.push_back(Parentkey);
							now->childrens.push_back(brother->childrens.front());
							brother->childrens.front()->father = now;
							now->cnt++;

							tempkey = brother->keys.front();

							brother->keys.erase(brother->keys.begin());
							brother->childrens.erase(brother->childrens.begin());
							brother->cnt--;
						}
						
					}
					else { // left brother
						if (now->is_leaf){
							tempkey = brother->keys.back();
							
							now->keys.insert(now->keys.begin(), tempkey);
							now->blkptr.insert(now->blkptr.begin(), brother->blkptr.back());
							now->cnt++;
							
							brother->keys.pop_back();
							brother->blkptr.pop_back();
							brother->cnt--;
							if (flag) update(now->father, entry_replace_val, now->keys.front());
						}
						else {
							tempkey = brother->keys.back();
							//now->insert(Parentkey, brother->childrens.back());
							now->keys.insert(now->keys.begin(), Parentkey);
							now->childrens.insert(now->childrens.begin(), brother->childrens.back());
							brother->childrens.back()->father = now;
							now->cnt++;

							brother->keys.pop_back();
							brother->childrens.pop_back();
							brother->cnt--;
						}
					}
					ret = CHANGE;
				}
				else { // merge
					if (now->next == brother) {
						if (!now->is_leaf) {now->keys.push_back(Parentkey); now->cnt++;}
						now->merge();
						delete brother;
						node_cnt--; 
						ret = DELETE;
						if (now->is_leaf && flag) update(now->father, entry_replace_val, now->keys.front());
					}
					else {
						
						if (!now->is_leaf) { brother->keys.push_back(Parentkey); brother->cnt++;}
						
						brother->merge();
						delete now;
						node_cnt--; 
						ret = DELETE;
						if (brother->is_leaf && flag) update(brother->father, entry_replace_val, brother->keys.front());
					}
				}
			}
			else {
				if (flag) update(now->father,entry_replace_val, now->keys.front());
			}
		}
			break;
		default:
			ret = NONE;
			break;
	}
	return ret;
}
template <typename T>
int BPlusTree<T>::Remove(T val){
	tempkey = val;
	if (!root) return 0;
	T x = T();
	return del(root, NULL, x);
}

template <typename T>
BPlusTree<T>::~BPlusTree() {
	drop(root);
	//root = null;
	//delete head;
	//delete tail;
}

template <typename T>
queue<pni> q;

template <typename T>
void BPlusTree<T>::Print() {
	/*if (!root) {puts("NULL!"); return;}
    q<T>.push(make_pair(root, 0));
    int last = 0;
    while (!q<T>.empty()) {
        pni res = q<T>.front();
        q<T>.pop();
        if (res.second != last) puts("");
        last = res.second;
        cout << *(res.first);
        //cout << *(res.first) << "(nxt is" << *(res.first->next) << ")" ;
        for (auto i : res.first->childrens)
            q<T>.push(make_pair(i, res.second + 1));
    }*/
	for (Node<T>* o = head->next; o != tail; o = o->next) {
		for (int j = 0; j < o->cnt; j++)
			cout << o->keys[j] << " ";
	}
	cout << endl;
}



template <typename T>
bool BPlusTree<T>::load_from_file() {
	//FILE *fp = fopen(idx_file.c_str(), "r");
	ifstream fin(idx_file.c_str());
	//if (fp == NULL) {fclose(fp); return 0;}
	if (!fin.is_open()){
		cout << "Index file not found!" << endl;
		return 0;
	}
	//fscanf(fp, "%d%d%d%d%d", &node_cnt, &key_cnt, &degree, &order, &keysz);
	fin >> node_cnt >> key_cnt >> degree >> order >> keysz;
	map<int, node > h;
	for (int i = 0; i < node_cnt + 2; i++) {
		int x;
		//fscanf(fp, "%d", &x); 
		fin >> x;
		tot = max(tot, x);
		node tempnode = new Node<T>(x, order, degree, 0, NULL, NULL, NULL, false);
		h[x] = tempnode;
	}
	h[0] = NULL;
	h[2] = new Node<T>(2, order, degree, 0, NULL, NULL, NULL, false);
	h[3] = new Node<T>(3, order, degree, 0, NULL, NULL, NULL, false);

	node now;
	for (int i = 1; i <= node_cnt + 2; i++){
		int id, cnt, father_id, is_leaf;
		//fscanf(fp, "%d%d%d%d", &id, &cnt, &father_id, &is_leaf);
		fin >> id >> cnt >> father_id >> is_leaf;
		now = h[id];
		now->cnt = cnt; 
		now->father = h[father_id];
		now->is_leaf = is_leaf;
		for (int i = 0; i < now->cnt; i++) {
			T tmp; fin >> tmp;
			now->keys.push_back(tmp);
		} 
        if (now->is_leaf){
			for (int i = 0; i < cnt; i++) {
				int tmp; fin >> tmp;
				now->blkptr.push_back(tmp);
			}
		}
		if (!now->is_leaf) 
			for (int i = 0; i <= cnt; i++){
				int tmp; fin >> tmp;
				now->childrens.push_back(h[tmp]);
			}
		int pre, nxt;
		fin >> pre >> nxt;
		now->next = h[nxt]; now->prev = h[pre];
		if (i == 1) root = now;
		if (id == 1) head = now;
		if (id == 2) tail = now;
		//cout << *now;
	}
	//fclose(fp);
	return true;
}

template <typename T>
void BPlusTree<T>::write_to_file() {
	//FILE *fp = fopen(idx_file.c_str(),"w");
	ofstream fout(idx_file.c_str());
	//fprintf(fp, "%d %d %d %d %d\n", node_cnt, key_cnt, degree, order, keysz);
	fout << node_cnt << " " << key_cnt << " " << degree << " " << order << " " << keysz << "\n";
	write_id(fout, root);
	write_id(fout, head);
	write_id(fout, tail);
	//fputs("\n", fp);
	fout << endl;
	write(fout, root);
	write(fout, head);
	write(fout, tail);
	//fclose(fp);
}
template <typename T>
void BPlusTree<T>::write_id(ofstream& fout, node now){
	if (!now) return;
	//fprintf(fp, "%d ", now->id);
	fout << now->id << " ";
	if (!now->is_leaf) 
		for (int i = 0; i <= now->cnt; i++) 
			write_id(fout, now->childrens[i]);
}

template <typename T>
void BPlusTree<T>::write(ofstream& fout, node now){
	//fprintf(fp, "%d %d %d %d\n", now->id, now->cnt, now->father == NULL ? 0 : now->father->id, now->is_leaf);
	fout << now->id << " " << now->cnt << " " << ( now->father == NULL ? 0 : now->father->id) << " " << now->is_leaf << endl;

	for (int i = 0; i < now->cnt; i++) fout << now->keys[i] << " ";
	if (now->is_leaf) {
		for (int i = 0; i < now->cnt; i++)
			fout << now->blkptr[i] << " "; 
			//fprintf(fp, "%d ", now->blkptr[i]);
		fout << endl;
	}
	//fputs("\n", fp);
	fout << endl;
	if (!now->is_leaf) {
		for (int i = 0; i <= now->cnt; i++) 
			fout << now->childrens[i]->id << " ";
			//fprintf(fp, "%d ", now->childrens[i]->id);
		//fputs("", fp);
		fout << endl;
	}
	//fprintf(fp, "%d %d\n", now->prev == NULL ? 0 : now->prev->id, now->next == NULL ? 0 : now->next->id);
	fout << (now->prev == NULL ? 0 : now->prev->id) << " " << (now->next == NULL ? 0 : now->next->id) << endl;
	if (!now->is_leaf)
		for (int i = 0; i <= now->cnt; i++) 
			write(fout, now->childrens[i]);
}

template <typename T>
void BPlusTree<T>::drop(node now){
	if (!now) return;
	if (!now->is_leaf) {
		for (int i = 0; i <= now->cnt; i++)
			drop(now->childrens[i]);
	}
	delete now;
	node_cnt--;
	return;
}

#endif