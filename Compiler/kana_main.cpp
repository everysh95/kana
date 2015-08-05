#include"kana_comp.h"
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>

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
		cerr << "文法は\"" << argv[0] << " [入力ファイル名] [出力ファイル名]\"です。" << endl;
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

	vector<kana::fanc*> fanc_col;//関数テーブル
	vector<kana::type*> type_col;//型テーブル

	//ファイルストリーム
	wifstream wfin(ifname);
	wofstream wfout(ofname);

	//現在の関数id
	size_t now = 0;
	int nline = 1;

	kana::comp_option::set_cpu(kana::comp_option::cpu::x64);

	//前設定
	type_col.push_back(new kana::type(L"整数",8));
	
	//入力の関する処理
	while(wfin)
	{
		wstring input_c;
		wsmatch rnm;
		getline(wfin,input_c);
		//デバック用
		//wcout << nline ;
		if(regex_match(input_c,rnm,wregex(L"(.*)の大きさは(.*)である。")))
		{
			//型定義だった場合
			//デバック用
			//wcout << L"t" << flush;
			int t_size;
			wistringstream iw(rnm.str(2));
			iw >> t_size;
			type_col.push_back(new kana::type(rnm.str(1),t_size));
		}
		else if(regex_match(input_c,rnm,wregex(L"(.*)手順。")))
		{
			//関数定義の最初だった場合
			//デバック用
			//wcout << L"fa" << flush;
			now = fanc_col.size();
			fanc_col.push_back(new kana::fanc(rnm.str(1)));
			if(now > 0)
			{
				bool suc;
				//デバック用
				//wcout << L"db" << flush;
				suc = fanc_col[now - 1]->precompile();
				if(!suc)
				{
					wcerr << L"不正:前処理(" <<  fanc_col[now - 1]->name() << L")" << endl;
				}
				//デバック用
				//wcout << L"dm" << flush;
				suc = fanc_col[now - 1]->main_compile();
				if(!suc)
				{
					wcerr << L"不正:主変換(" << fanc_col[now - 1]->name() << L")" << endl;
				}
			}
		}
		else
		{	
			//それ以外なら
			//デバック用
			if(fanc_col[now]->add_com(input_c))
			{
				//wcout << L"fc" << flush;
			}
			
		}
		//デバック用
		//wcout << nline << L":" << input_c << endl;
		nline++;
	}

	{
		bool suc;
		suc = fanc_col[now]->precompile();
		if(!suc)
		{
			wcerr << L"不正:前処理(" << fanc_col[now]->name() << L")" << endl;
		}
		suc = fanc_col[now]->main_compile();
		if(!suc)
		{
			wcerr << L"不正:主変換(" << fanc_col[now]->name() << L")" << endl;
		}
	}

	//wcout << L"BOO" << endl;
	//出力に関する処理
	wfout << L".text\n.global main" << endl;
	auto fe = fanc_col.end();
	for(auto fp = fanc_col.begin();fp != fe;fp++)
	{
		if((*fp)->name() == L"主な")
		{
			wfout << L"main:" << endl;
		}
		std::vector<std::wstring> output = (*fp)->output_com();
		auto end = output.end();
		for(auto i = output.begin();i != end;i++)
		{
			wfout << *i << endl; 
		}
	}
	//wcout << L"EOO" << endl;


	for(int i = 0;i < fanc_col.size();i++)
	{
		if(fanc_col[i] != nullptr)
		delete fanc_col[i];
	}

	for(int i = 0;i < type_col.size();i++)
	{
		if(type_col[i] != nullptr)
		delete type_col[i];
	}
	return 0;
}

