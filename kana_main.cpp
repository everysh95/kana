#include"kana_comp.h"
#include<string>
#include<iostream>
#include<fstream>

int main(int argc,char** argv)
{
	using namespace std;
	cout << "kana_system 0.01" << endl;
	locale::global(locale("ja_JP.utf-8"));

	
	while(true)
	{
		wstring input_c;
		cout << "コマンドを入力してください。" << endl;
		cout << "q:終了 i:入力" << endl;
		wcin >> input_c;
		if(input_c == L"q")
		{
			cout << "お疲れさまでした。" << endl;
			break;
		}
		else if(input_c == L"i")
		{
			std::wstring input_ws;
			std::string col_in;
			cout << "入力:";
			wcin >> input_ws;
			col_in = kana::filter_a(input_ws);
			cout << "出力:" << col_in << endl; 
		}
	}

	return 0;
}

