#include "Buffer.h"
using namespace std;
Buffer global_buffer;
BBlock& Buffer::read_block(string &filename, int block_id)//如果 buffer里有直接拿buffer里的，没有再去文件里找
{
		for (int i = 0; i < BLOCK_CNT; i++)
		{
			if (filename == BBuffer[i].file_name&&block_id == BBuffer[i].block_id)
			{
				return BBuffer[i];
			}
		}
		BBlock  &cur_block = GET_LRU(); // new block for data
		cur_block.init(filename, block_id);
		fstream fin;
		fin.open(filename+".table", ios::in | ios::binary);
		if (!fin.good()) // 
		{
			fin.close();
			cerr << "file open error! The File doesn't exist!" << endl;
			fstream fout;
			fout.open(filename+".table", ios::out | ios::binary);
			fout.close();
			fin.open(filename+ ".table", ios::in | ios::binary);
		}
		fin.seekg(ios::beg + block_id * BLOCK_SIZE);
		if (fin.peek() == EOF)
		{
			cerr << "read error" << endl;
		}
		else
		{  //  for(int i=0;i<3000;i++)
			//cout << cur_block.data[i];
			fin.read((char*)cur_block.data, BLOCK_SIZE);
			fin.close();
		}
		return cur_block;
}


BBlock& Buffer::GET_LRU()//假装是个LRU
{
	static int idx = 0;
	while (1)
	{
		int fl = 0;
		int tmp = BLOCK_CNT;
		int st = idx; 
		idx = (idx + 1) % BLOCK_CNT;
		while(tmp--)
		{ 
			if ( BBuffer[st].Is_pin() == 0) // 假装这句话有意义...
			{  
				fl = 1;
				if (BBuffer[st].Is_dirty() == 1)
				{
					BBuffer[st].Block_to_file();
				}
				BBuffer[st].init("", -1);
				return BBuffer[st];
			}
			st = (st + 1) % BLOCK_CNT;
		}
		if (fl == 0) cerr << "LRU failed" << endl;
	}
}
void Buffer::DeleteFile(string &filename)
{ 
	for(int i=0;i<BLOCK_CNT;i++)
    {
		if (BBuffer[i].file_name == filename)
		{
			BBuffer[i].Block_to_file();
			BBuffer[i].init("", -1);
		}
    }

}

void Buffer::Buffer_flush()
{
	for (int i = BLOCK_CNT - 1; i >= 0; i--)
	{
		BBuffer[i].Block_to_file();
	}
}