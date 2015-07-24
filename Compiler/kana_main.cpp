#include"kana_comp.h"
#include<string>
#include<iostream>
#include<fstream>

int main(int argc,char** argv)
{
	using namespace std;
//	cout << "kana_system 0.01" << endl;
	ios_base::sync_with_stdio(false);
	locale::global(locale("ja_JP.utf-8"));
	wcout.imbue(locale("ja_JP.utf-8"));
	wcin.imbue(locale("ja_JP.utf-8"));
	wcerr.imbue(locale("ja_JP.utf-8"));

	//ここからファイル名の確定

	if(argc <= 1)
	{
		//コマンドライン引数が不正である場合
		wcerr << L"文法が違います" << endl;
		wcerr << L"文法は\"" << argv[0] << " [入力ファイル名] [出力ファイル名]\"です。" << endl;
		return 0;
	}
	
	//入力ファイル名の確定
	string ifname = argv[1];

	//出力ファイル名に関する処理
	string ofname;

	if(argc > 2)
	{
		ofname = argv[2];	
	}

	//出力ファイル名が未定の場合
	if(ofname.empty())
	{
		ofname = "out.s";
	}


	//関数テーブル
	vector<kana::fanc*> fanc_col;

	//ファイルストリーム
	wifstream wfin(ifname);
	wofstream wfout(ofname);

	//現在の関数id
	size_t now = 0;
	
	//入力の関する処理
	while(wfin)
	{
		wstring input_c;
		wsmatch rnm;
		getline(wfin,input_c);
		if(regex_match(input_c,rnm,wregex(L"(.*)手順。")))
		{
			//関数定義の最初だった場合
			now = fanc_col.size();
			fanc_col.push_back(new kana::fanc(rnm.str(1)));
			if(now > 0)
			{
				fanc_col[now - 1]->precompile();
				fanc_col[now - 1]->main_compile();
			}
		}
		else
		{	
			//それ以外なら
			fanc_col[now]->add_com(input_c);
		}
	}
	fanc_col[now]->main_compile();

	//出力に関する処理
	auto fe = fanc_col.end();
	for(auto fp = fanc_col.begin();fp != fe;fp++)
	{
		std::vector<std::wstring> output = (*fp)->output_com();
		auto end = output.end();
		for(auto i = output.begin();i != end;i++)
		{
			wfout << *i << endl; 
		}
	}


	for(int i = 0;i < fanc_col.size();i++)
	{
		delete fanc_col[i];
	}

	return 0;
}

