#include<string>
#include<iostream>
#include<fstream>

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
	std::vector<std::wstring> input_col;

	while(input_f)
	{
		std::wstring input_ws;
		getline(input_f,input_ws,L'。');
		input_ws += L"。"
		input_col.push_back(input_ws);
	}
}

