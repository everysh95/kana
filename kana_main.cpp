#include<string>
#include<iostream>
#include<fstream>
#include<kana_comp.h>

int main(int argc,char** argv)
{
	std::string xx;
	int n = 1;

	if(argc > n)
	{
		xx = argv[n];
	}


	if(xx.empty())
	{
		std::cerr << "対象となるファイルが指定されていません。" << std::endl;
		return 0;
	}


	std::wifstream input_f(xx);
	std::vector<std::string> input_col;

	while(input_f)
	{
		std::wstring input_ws;
		std::string col_in;
		getline(input_f,input_ws,L'。');
		input_ws += L"。"
		col_in = kana::filter_a(input_ws);
		input_col.push_back(input_ws);
	}

	for(auto i = input_col.begin();i != input_col.end();i++)
	{
		std::cout << *i << std::endl;
	}

	return 0;
}

