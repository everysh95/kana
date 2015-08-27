#include"comp.h"
#include"fanc.h"
#include"class.h"
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdlib>

int main(int argc,char** argv)
{
	using namespace std;
//	cout << "kana_system 0.01" << endl;
	//ios_base::sync_with_stdio(false);
	locale::global(locale("ja_JP.utf-8"));
	wcout.imbue(locale("ja_JP.utf-8"));
	wcin.imbue(locale("ja_JP.utf-8"));
	wcerr.imbue(locale("ja_JP.utf-8"));

	//ここからファイル名の確定

	if(argc <= 1)
	{
		//コマンドライン引数が不正である場合
		wcerr << L"文法が違います" << endl;
		cerr << "文法は\"" << argv[0] << " [入力ファイル名]\"です。" << endl;
		return 0;
	}
	
	//ファイル名の確定
	vector<string> ifname;
	bool ofn_flg = false,std_flg = true,asm_flg = false;
	string ofname;
	string opname;
	for(int i = 1;i < argc;i++)
	{
		string cinv = argv[i];
		if(ofn_flg)
		{
			ofname = cinv;
			ofn_flg = false;
		}
		else if(ofname.empty() && cinv == "-o")
		{
			ofn_flg = true;
		}
		else if(cinv == "-std=false")
		{
			std_flg = false;
		}
		else if(cinv == "-std=true")
		{
			std_flg = true;
		}
		else if(cinv == "-asm")
		{
			asm_flg = true;
		}
		else
		{
			ifname.push_back(cinv);
		}
	}

	ifstream stdfiles(".stdfiles");
	vector<string> std_name;
	while(stdfiles)
	{
		string buf;
		stdfiles >> buf;
		if(!buf.empty())
		{
			std_name.push_back(buf);
		}
	}

	if(std_flg)
	{
		ifname.insert(ifname.begin(),std_name.begin(),std_name.end());
	}

	//出力ファイル名が未定の場合
	if(ofname.empty() && asm_flg)
	{
		ofname = "out.s";
	}
	else if(!asm_flg)
	{
		opname = ofname;
		ofname = ".out.s";
	}

	vector<kana::fanc*> fanc_col;//関数テーブル
	vector<kana::type*> type_col;//型テーブル

	//ファイルストリーム
	wifstream wfin(ifname[0]);
	wofstream wfout(ofname);

	//現在の関数id
	size_t now = 0;
	int nline = 1;
	unsigned int now_num = 1;

	kana::comp_option::set_cpu(kana::comp_option::cpu::x64);

	//前設定
	type_col.push_back(new kana::type(L"整数",8));
	type_col.push_back(new kana::type(L"文字",4));
	
	//入力の関する処理
	while(wfin)
	{
		wstring input_c;
		wsmatch rnm;
		getline(wfin,input_c,L'。');
		input_c = kana::del_comment(input_c);
		input_c = kana::del_nt(input_c);
		if(!input_c.empty())
		input_c += L"。";
		//デバック用
		//wcout << input_c << endl;
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
				else
				{
					//wcout << L"正常処理:(" << fanc_col[now - 1]->name() << L")" << endl;
				}
				//デバック用
				//wcout << L"dm" << flush;
				suc = fanc_col[now - 1]->main_compile();
				if(!suc)
				{
					wcerr << L"不正:主変換(" << fanc_col[now - 1]->name() << L")" << endl;
				}
				else
				{
					//wcout << L"正常変換:(" << fanc_col[now - 1]->name() << L")" << endl;
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
		if(!wfin && ifname.size() > now_num)
		{
			wfin.close();
			wfin.open(ifname[now_num]);
			now_num++;
		}
	}

	{
		bool suc;
		suc = fanc_col[now]->precompile();
		if(!suc)
		{
			wcerr << L"不正:前処理(" << fanc_col[now]->name() << L")" << endl;
		}
		else
		{
			//wcout << L"正常処理:(" << fanc_col[now]->name() << L")" << endl;
		}
		suc = fanc_col[now]->main_compile();
		if(!suc)
		{
			wcerr << L"不正:主変換(" << fanc_col[now]->name() << L")" << endl;
		}
		else
		{
			//wcout << L"正常変換:(" << fanc_col[now]->name() << L")" << endl;
		}
	}

	//wcout << L"BOO" << endl;
	//出力に関する処理
	//メインメモリー
	wfout << L".bss" << endl;
	wfout << L"mm: .skip " << kana::variable_type::size_sum() << L", $0" << endl;
	//配列用メモリー
	vector<std::wstring> vtext = kana::vector_type::vector_output();
	for(int i = 0;i < vtext.size();i++)
	{
		wfout << vtext[i] << L", $0" << endl;
	}
	//プログラム本体
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


	//後処理
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

	string create_com;
	if(!asm_flg)
	{
		create_com = "gcc ";
		if(opname.empty())
		{
			create_com += ".out.s";
		}
		else
		{
			create_com += "-o " + opname;
			create_com += " .out.s";
		}
		system(create_com.c_str());
		system("rm .out.s");
	}

	return 0;
}

