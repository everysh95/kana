#include "kana_comp.h"

namespace kana
{
	std::vector<type*> type::type_target;

	type::type()
	{
		type_target.push_back(this);
	}

	type::type(std::wstring t_name)
		:t_name(t_name)
	{
		type_target.push_back(this);
	}


	type::type(std::wstring t_name,std::vector<std::wstring> castable_type)
		:t_name(t_name)
	{
		type_target.push_back(this);
	}

	type::~type()
	{
		auto target_end = std::remove(type_target.begin(),type_target.end(),this);
		type_target.erase(target_end,type_target.end());
	}

	std::wstring type::type_name()
	{return t_name;}

	bool type::type_match(type t_type)
	{
		for(auto i = type_target.begin();i != type_target.end();i++)
		{
			if(t_type == **i)
				return true;
		}
		return false;
	}

	bool type::consted_type(std::wstring target)
	{
		bool ans = false;
		auto cend = consted.end();
		for(auto i = consted.begin();i != cend;i++)
		{
			ans = ans || regex_match(target,std::wregex((*i)->c_str()));
		}
		return ans;
	}

	type* type::find_from_wstr(std::wstring target)
	{
		for(auto i = type_target.begin();i != type_target.end();i++)
		{
			if(target == (*i)->type_name())
			{
				return *i;
			}
		}

		return nullptr;
	}


	bool type::operator==(const type& rhs)const
	{
		/*型の判定*/
		if(rhs.t_name == t_name)
		{
			/*名前が一致した場合*/
			return true;
		}
		else
		{
			/*名前が一致しなかった場合*/
			if(castable_types.empty())
			{
				/*キャストできない場合*/
				return false;
			}
			
			/*キャストして一致するかどうか判定*/
			auto tend = castable_types.end();
			auto rbegin = rhs.castable_types.begin();
			auto rend = rhs.castable_types.end();
			for(auto ti = castable_types.begin();ti != tend;ti++)
			{
				for(auto ri = rbegin;ri != rend;ri++)
				{
					if(*ri == *ti)
					{
						/*キャストして一致したばあい*/
						return true;
					}
				}
			}

			/*キャストして一致しなかった場合*/
			return false;
		}
	}

	fanc::fanc(std::wstring name)
		:com_name(name)
	{}

	bool fanc::add_com(std::wstring target)
	{
		std::wstring input_com = filter_str(target);
		com_contents.push_back(input_com);
		return true;
	}

	bool fanc::precompile()
	{
		using namespace std;
		com_low = L"";

		wregex 
		input_l(L"これは*である*を受け取る。",std::regex_constants::basic),
		output_l(L"これは*を返す。",std::regex_constants::basic),
		output_nl(L"ここで*を返す。",std::regex_constants::basic),
		variable_l(L"*は*である。",std::regex_constants::basic);

		wsmatch result_mws;
		vector<variable_type> 
		variables,/*変数判定用*/
		ref_v;/*引数判定用*/
		type out_type(L"");

		int ref_count = 0,out_count = 0;
		for(auto i = com_contents.begin();i != com_contents.end();i++)
		{
			/*引数の宣言であるかどうかの判定*/
			if(regex_match(*i,input_l))
			{
				/*--引数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				auto point_f = (*i).begin();
				auto point_tr = point_f + (*i).rfind(L"である");
				auto point_nl = point_tr + 3;
				auto point_nr = point_f + (*i).rfind(L"を受け取る。");
				/*引数の登録*/
				ref_v.push_back(make_pair(wstring(point_f,point_tr),type::find_from_wstr(wstring(point_nl,point_nr))));
				ref_count++;
			}
			/*返り値の判定*/
			if(regex_match(*i,output_l) && !regex_match(*i,output_nl))
			{
				/*返り値の型宣言だったときの処理*/
				auto point_l = (*i).begin();
				auto point_r = point_l + (*i).rfind(L"を返す。");
				type x(wstring(point_l,point_r));
				out_type = x;
				out_count++;
			}
			else if(regex_match(*i,output_l))
			{
				/*返り値だった場合の処理*/
				auto point_l = (*i).rfind(L"ここで") + 3;
				auto point_r = point_l + (*i).rfind(L"を返す。");
				auto vend = variables.end();
				auto refend = ref_v.end();
				wstring tar(point_l,point_r);
				bool out_l = false;
				/*返り値が変数かどうか*/
				for(auto j = variables.begin();j != vend;j++)
				{
					if((*j).first == tar)
					{
						/*返り値が変数だった場合の処理*/
						if(out_type.type_name() != L"" && *((*j).second) != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type.type_name() == L"")
						{
							out_type =  *((*j).second);
						}
						out_l = true;
					}
				}
				/*返り値が引数かどうか*/
				for(auto j = ref_v.begin();j != refend;j++)
				{
					if((*j).first == tar)
					{
						/*引数だった場合の処理*/
						if(out_type.type_name() != L"" && *((*j).second) != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type.type_name() == L"")
						{
							out_type = *((*j).second);
						}
						out_l = true;
					}		
				}
				/*定数かどうか*/
				if(out_type.type_name() != L"")
				{
					if(!out_l && !out_type.consted_type(tar))
					{
						cerr << "返り値が不正です。" << endl;
						return false;
					}
				}
			}

			if(regex_match(*i,variable_l))
			{
				/*--変数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				auto point_f = (*i).begin();
				auto point_tr = point_f + (*i).rfind(L"は");
				auto point_nl = point_tr + 1;
				auto point_nr = point_f + (*i).rfind(L"である。");
				/*変数の登録*/
				variables.push_back(make_pair(wstring(point_f,point_tr),type::find_from_wstr(wstring(point_nl,point_nr))));	
			}
		}

		return true;
	}

	std::vector<fanc*> fanc::fancs;

	bool fanc::main_compile()
	{
		/*--初期化--*/
		bool ans = true,baf_b = false,baf_b2 = true;
		auto fe = fancs.end();
		std::vector<variable_type> variables;/*変数判定用*/
		asm_command.clear();
		std::wregex 
		in_asm(L"ASM「(*)」。"),
		if_one(L"もし、(*)なら(*。)"),
		else_one(L"もし、(*)なら(*。)そうでなければ(*。)"),
		loop_b(L"(*)である限り、ここから実行。"),
		loop_e(L"(*)")

		asm_command.push_back(filter_a(com_name) + L":");

		for(int i = 0;i < com_contents.size();i++)
		{
			baf_b = false;
			for(auto j = fancs.begin();j != fe;j++)
			{
				baf_b2 = (*j)->asm_comp(com_contents[i],variables,asm_command);
				baf_b = baf_b || baf_b2;
			}
			/*--基本的な構文の処理--*/
			std::wstring asm_com;
			if(!baf_b && base_com::inline_asm(com_contents[i],asm_com))
			{
				baf_b = true;
				asm_command.push_back(asm_com);
			}
			if(!baf_b && base_com::if_trans(com_contents[i],asm_com))
			{
				baf_b = true;
				asm_command.push_back(asm_com);
			}
			if(!baf_b && base_com::loop_begin(com_contents[i],asm_com))
			{
				baf_b = true;
				asm_command.push_back(asm_com);
			}
			if(!baf_b && base_com::loop_end(com_contents[i],asm_com))
			{
				baf_b = true;
				asm_command.push_back(asm_com);
			}

			ans = ans && baf_b;
		}
		asm_command.push_back(filter_a(com_name) + L"_end:");
		asm_command.push_back("ret");
		return ans;
	}

	bool fanc::asm_comp(std::wstring input,std::vector<std::wstring>& target)
	{
		using namespace std;
		wstring ans;
		wregex tar_o(com_low.c_str());
		if(regex_match(input,tar_o))
		{
			target.push_back(L"call " + filter_a(com_name));
		}
		return ans;
	}
	
	std::string filter_a(std::wstring wstr)
	{
		int wsize = sizeof(wchar_t);
		std::string ans;
		char baf;
		/*4bitで表せる最大値*/
		for(int i = 0;i < wstr.size();i++)
		{
			for(int j = 0;j < wsize * 2;j++)
			{
				/*4bitずつに区切る*/
				baf = (wstr[i] >> (j*4)) % (16) + 'a';
				/*ansに付け加える*/
				ans += baf;
			}
		}
		return ans;
	}

	std::wstring filter_str(std::wstring input)
	{
		return input;
	}

	bool bsae_com::inline_asm(std::wstring input,std::wstring& output)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"機械「(*)」")))
		{
			output =  out.str(1);
			return true;
		}
		return false;
	}

	bool base_com::if_trans(std::wstring input,std::wstring& output)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"もし(*)ならば(*。)")))
		{

			return true;
		}
		return false;
	}

}

