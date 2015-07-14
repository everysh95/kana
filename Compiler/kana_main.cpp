#include"kana_comp.h"
#include<string>
#include<iostream>
#include<fstream>

int main(int argc,char** argv)
{
	using namespace std;
	cout << "kana_system 0.01" << endl;
	ios_base::sync_with_stdio(false);
	locale::global(locale("ja_JP.utf-8"));
	wcout.imbue(locale("ja_JP.utf-8"));
	wcin.imbue(locale("ja_JP.utf-8"));

	vector<kana::fanc*> fanc_col;
	
	while(true)
	{
		wstring input_c;
		cout << "コマンドを入力してください。" << endl;
		cout << "q:終了 i:入力 o:出力" << endl;
		wcin >> input_c;
		if(input_c == L"q")
		{
			cout << "お疲れさまでした。" << endl;
			break;
		}
		else if(input_c == L"i")
		{
			std::wstring name_str;
			cout << "関数名(動詞):" << flush;
			wcin >> name_str;
			fanc_col.push_back(new kana::fanc(name_str));
			auto f = *(fanc_col[fanc_col.size() - 1]);
			/*入力*/
			cout << "EOCで入力終了します。" << endl;
			int line = 1;
			while(true)
			{
				std::wstring input_ws,lb;
				wcout << line << L":" << flush;
				do{
					getline(wcin,input_ws);
				}while(input_ws[input_ws.size() - 1] != L'。');

				if(input_ws == L"EOC。")
					break;
				f.add_com(input_ws);
				line++;
			}
			//f.precompile();
			f.main_compile();
			*(fanc_col[fanc_col.size() - 1]) = f;
		}
		else if(input_c == L"o")
		{
			auto fe = fanc_col.end();
			int line = 1;
			for(auto fp = fanc_col.begin();fp != fe;fp++)
			{
				std::vector<std::wstring> output = (*fp)->output_com();
				auto end = output.end();
				for(auto i = output.begin();i != end;i++)
				{
					wcout << line << ":" << *i << endl; 
					line++;
				}
			}
		}
	}

	for(int i = 0;i < fanc_col.size();i++)
	{
		delete fanc_col[i];
	}

	return 0;
}

