#ifndef attribute_H
#define attribute_H
#pragma once
#include <cstdio>
#include <string>
#include <vector>
#include"BBlock.h"
using namespace std;
class Attribute
{
public:
	 Attribute() {};
	 KTP type;
	 int str_len = 0;
	 string  attribute_name;
	 bool is_PK = 0;
	 bool is_unique = 0;
	 bool has_idx = 0;
	 int getSize()
	 {
		 switch (type)
		 {
		 case KTP::INT:
		 {
			 return sizeof(int);
		 }
		 case KTP::FLOAT:
		 {
			 return sizeof(float);
		 }
		 case KTP::STRING:
		 {
			 return str_len * sizeof(char);
		 }
		 }
	 }
};
#endif 