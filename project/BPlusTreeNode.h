/*
 * @Author: your name
 * @Date: 2020-05-28 13:58:18
 * @LastEditTime: 2020-06-02 14:02:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \project\BPlusTreeNode.h
 */ 
#ifndef _BPLUSTREENODE_H_
#define _BPLUSTREENODE_H_

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "basic.h"
using namespace std;


template <typename T>
class Node {
   private:
    int order, degree;
	

   public:
    int cnt;
	int id;
    Node* father;
    vector<T> keys;
	vector<int> blkptr;
    vector<Node*> childrens;
    Node* next;
    Node* prev;
    bool is_leaf;

    Node(int id = 0, int ord = 3, int deg = 3, int _cnt = 0, Node* _fa = NULL, Node* _nxt = NULL, Node* _pre = NULL, bool is_new_leaf = false);
    ~Node();
    bool is_overfull();
    bool find(T key, int& pos);
    Node* split(T& val);
    void insert(T key, int blockoffset = 0, Node* ch = NULL);
    void del(int pos);
	void merge();

    friend ostream& operator<<(ostream& os, const Node<T>& rhs) {
        if (&rhs == NULL)
            os << "NULL";
        else {
            os << "[";
			os << "id:" << rhs.id << " " <<"nxt:" << (rhs.next == NULL ? 0 : rhs.next->id) <<"; ";
            os << "ch:(";
			for (auto i : rhs.childrens) os << i->id << ","; os <<"), ";
            if (!rhs.is_leaf){
				for (int i = 0; i < rhs.keys.size(); i++)
                os << rhs.keys[i] << (i + 1 == rhs.keys.size() ? ']' : ',');
			}
			else {
				for (int i = 0; i < rhs.keys.size(); i++)
                os << rhs.keys[i] << ":" << rhs.blkptr[i] << (i + 1 == rhs.keys.size() ? ']' : ',');
			}
		}   
		return os;
    }
};

/**
 * @description: Constructor of Node
 * @param  int ord, int deg, int _cnt, Node *fa, Node *_nxt, Node* _pre, bool
 * is_new_leaf
 * @return: None
 */
template <typename T>
Node<T>::Node(int _id, int ord, int deg, int _cnt, Node* fa, Node* _nxt, Node* _pre, bool is_new_leaf) {
    cnt = _cnt;
    father = fa;
    order = ord;
    degree = deg;
    next = _nxt;
    prev = _pre;
	if (_id == 0) id = ++tot;
	else id = _id;
    is_leaf = is_new_leaf;
    keys.clear();
	blkptr.clear(); 
    childrens.clear();
}

/**
 * @description: find the key in values or keys
 * @param {T key, int &pos} pos is the position that geq key
 * @return: True if found, False otherwise
 */
template <typename T>
bool Node<T>::find(T key, int& pos) {
    pos = 0;
    typename vector<T>::iterator it;
    it = lower_bound(keys.begin(), keys.end(), key);
    pos = it - keys.begin();

    if (it == keys.end()) return false;
    if (*it == key)
        return true;
    else
        return false;
}

template <typename T>
void Node<T>::merge(){
	Node<T>* brother = next;
	if (is_leaf)
		for (int i = 0; i < brother->cnt; i++) {
			keys.push_back(brother->keys[i]);
			blkptr.push_back(brother->blkptr[i]);
		}
	else {
		for (int i = 0; i < brother->cnt; i++) {
			keys.push_back(brother->keys[i]);
			//blkptr.push_back(brother->blkptr[i]);
		}
		for (int i = 0; i < brother->cnt + 1; i++) {
			childrens.push_back(brother->childrens[i]);
			brother->childrens[i]->father = this;
		}
	}
	cnt += brother->cnt;
	if (brother->next) brother->next->prev = this;
	next = brother->next;
	return;
}
/**
 * @description: split the vector<T> values or keys
 * @param {T &val the value which should be push up}
 * @return: the pointer of the new Node
 */
template <typename T>
Node<T>* Node<T>::split(T& val) {
    int leftsize = is_leaf ? cnt / 2 : cnt / 2;
    Node* newNode = new Node<T>(0, order, degree, cnt - leftsize - (!is_leaf), father, this->next, this, is_leaf);
	if (next) next->prev =newNode;
	next = newNode;
    if (is_leaf) {
        for (int i = leftsize; i < cnt; i++) {
            newNode->keys.push_back(keys[i]);
			newNode->blkptr.push_back(blkptr[i]);
		}
        while (keys.size() != leftsize) {
			keys.pop_back();
			blkptr.pop_back();
		}
		cnt = leftsize;
        val = *(newNode->keys.begin());
    } else {
        val = (keys[leftsize]);
        for (int i = leftsize + 1; i < cnt; i++) {
            newNode->keys.push_back(keys[i]);
			//newNode->blkptr.push_back(blkptr[i]);
		}
        for (int i = leftsize + 1; i < childrens.size(); i++)
            childrens[i]->father = newNode,
            newNode->childrens.push_back(childrens[i]);
        while (childrens.size() != leftsize + 1) childrens.pop_back();
        while (keys.size() != leftsize) keys.pop_back();
        cnt = leftsize;
    }
    return newNode;
}
template <typename T>
/**
 * @description: insert a value into vector<T> values or keys
 * @param T val to be inserted
 * @param Node* ch if present node is not a leaf
 * @return: None
 */
void Node<T>::insert(T val, int blockoffset, Node* ch) {
    typename vector<T>::iterator it;
    if (is_leaf) {
        it = lower_bound(keys.begin(), keys.end(), val);
		int pos = it - keys.begin();
        keys.insert(it, val);
		blkptr.insert(blkptr.begin() + pos, blockoffset);

    } else {
        it = lower_bound(keys.begin(), keys.end(), val);
        int tmppos = it - keys.begin();
        tmppos++;
        keys.insert(it, val);
        childrens.insert(childrens.begin() + tmppos, ch);
		ch->father = this;
    }
    cnt++;
}
template <typename T>
bool Node<T>::is_overfull() {
    if (is_leaf)
        return cnt > order;
    else
        return cnt > degree;
}

template <typename T>
void Node<T>::del(int pos) {
	if (is_leaf) {
		keys.erase(keys.begin() + pos);
		blkptr.erase(blkptr.begin() + pos);
	}
	else {
		keys.erase(keys.begin() + pos);
		childrens.erase(childrens.begin() + pos + 1);
	}
	cnt--;
	return;
}

template <typename T>
Node<T>::~Node() {}


#endif