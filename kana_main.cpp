#include"kana_comp.h"
#include<string>
#include<iostream>
#include<fstream>

int main(int argc,char** argv)
{
	std::vector<std::wstring> input_col;
	input_col = {L"abc",L"こんにちは"};
	std::vector<std::string> output_col;
	int size = input_col.size();

	for(int i = 0;i < size;i++)
	{
		std::wstring input_ws = input_col[i];
		std::string col_in;
		col_in = kana::filter_a(input_ws);
		input_col.push_back(input_ws);
		std::cout << i << ":" << input_col.size() << std::endl;
	}

	for(auto i = input_col.begin();i != input_col.end();i++)
	{
		std::wcout << *i << std::endl;
	}
	for(auto i = output_col.begin();i != output_col.end();i++)
	{
		std::cout << *i << std::endl;
	}

	return 0;
}

