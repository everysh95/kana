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
			cout << "EOCで入力終了します。" << endl;
			kana::fanc f(L"試験");
			/*入力*/
			while(true)
			{
				std::wstring input_ws;
				std::string col_in;
				cout << "入力:";
				wcin >> input_ws;
				if(input_ws == L"EOC")
					break;
				f.add_com(input_ws);
			}
			f.precompile();
			f.main_compile();
			std::vector<std::wstring> output = f.output_cpp();
			auto end = output.end();
			for(auto i = output.begin();i != end;i++)
			{
				wcout << L"出力:" << *i << endl; 
			}
		}
	}

	return 0;
}

