#include<iostream>
#include"Interpreter.h"
#include"basic.h"
#include<time.h>
#include "IndexManager.h"
using namespace std;

int main() {
	std::cout << ">>> Welcome to MiniSQL" << std::endl;
	IndexManager myIdx;// = new IndexManager();
	while (1) {
		Interpreter query;
		query.myIdx = &myIdx;
		query.getQuery();
		clock_t begin, end;
		begin = clock();
		query.EXEC();
		end = clock();
		cout << "Run Time: " << end - begin << endl;
	}
	//delete myIdx;
	
	system("pause");
	return 0;
}