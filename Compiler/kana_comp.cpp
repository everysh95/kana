#include "kana_comp.h"

namespace kana
{
	std::vector<type*> type::type_target;

	type::type()
	{}

	type::type(std::wstring t_name)
		:t_name(t_name)
	{
		id = type_target.size();
		type_target.push_back(this);
	}


	type::type(std::wstring t_name,std::vector<std::wstring> castable_type)
		:t_name(t_name)
	{
		id = type_target.size();
		type_target.push_back(this);
		castable_types = castable_type;
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

		wsmatch result_mws,result;
		vector<variable_type> 
		variables,/*変数判定用*/
		ref_v;/*引数判定用*/
		type* out_type = nullptr;

		int ref_count = 0,out_count = 0;
		for(auto i = com_contents.begin();i != com_contents.end();i++)
		{
			/*引数の宣言であるかどうかの判定*/
			if(regex_match(*i,result_mws,wregex(L"これは(*)である(*)を受け取る。",std::regex_constants::basic)))
			{
				/*--引数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				/*引数の登録*/
				ref_v.push_back(make_pair(result_mws.str(1),type::find_from_wstr(result_mws.str(2))));
				ref_count++;
			}
			/*返り値の判定*/
			if(regex_match(*i,result_mws,wregex(L"これは(*)を返す。",std::regex_constants::basic)) && !regex_match(*i,wregex(L"ここで(*)を返す。",std::regex_constants::basic)))
			{
				/*返り値の型宣言だったときの処理*/
				out_type = type::find_from_wstr(result_mws.str(1));
				out_count++;
			}
			else if(regex_match(*i,result_mws,wregex(L"ここで(*)を返す。",std::regex_constants::basic)))
			{
				/*返り値だった場合の処理*/
				auto vend = variables.end();
				auto refend = ref_v.end();
				wstring tar = result_mws.str(1);
				bool out_l = false;
				/*返り値が変数かどうか*/
				for(auto j = variables.begin();j != vend;j++)
				{
					if((*j).first == tar)
					{
						/*返り値が変数だった場合の処理*/
						if(out_type.type_name() != nullptr && *((*j).second) != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type.type_name() == nullptr)
						{
							out_type =  (*j).second;
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
						if(out_type != nullptr && *((*j).second) != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type == nullptr)
						{
							out_type = (*j).second;
						}
						out_l = true;
					}		
				}
				/*定数かどうか*/
				if(out_type != nullptr)
				{
					if(!out_l && !out_type.consted_type(tar))
					{
						cerr << "返り値が不正です。" << endl;
						return false;
					}
				}
			}

			if(regex_match(*i,result_mws,wregex(L"(*)は(*)である。",std::regex_constants::basic)))
			{
				/*--変数の宣言であった時の処理--*/
				/* 対象の型と名前の特定         */
				/* 変数の登録                   */

				variables.push_back(make_pair(result_mws.str(1),type::find_from_wstr(result_mws.str(2))));	
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

		asm_command.push_back(L"f" + id + L":");

		for(int i = 0;i < com_contents.size();i++)
		{
			baf_b = false;
			for(auto j = fancs.begin();j != fe;j++)
			{
				baf_b2 = (*j)->asm_comp(com_contents[i],variables,asm_command);
				baf_b = baf_b || baf_b2;
			}

			/*--基本的な構文の処理--*/
			if(!baf_b && base_com::inline_asm(com_contents[i],asm_command))
				baf_b = true;
			if(!baf_b && base_com::if_trans(com_contents[i],asm_command))
				baf_b = true;
			if(!baf_b && base_com::loop_begin(com_contents[i],asm_command))
				baf_b = true;
			if(!baf_b && base_com::loop_end(com_contents[i],asm_command))
				baf_b = true;

			ans = ans && baf_b;
		}
		asm_command.push_back(L"f" + id + L"_end:");
		asm_command.push_back(L"ret");
		return ans;
	}

	bool fanc::asm_comp(std::wstring input,std::vector<std::wstring>& target)
	{
		using namespace std;
		wstring ans;
		wregex tar_o(com_low.c_str());
		if(regex_match(input,tar_o))
		{
			target.push_back(L"call f" + id);
		}
		return ans;
	}
	
	std::wstring filter_a(std::wstring wstr)
	{
		return L"f" + func::find_id(wstr);
	}

	std::wstring filter_str(std::wstring input)
	{
		return input;
	}

	/*--base_com--*/
	long base_com::if_counter = 0;
	long base_com::loop_counter = 0;
	std::vector<std::wstring> base_com::terms_stack;
	std::vector<std::wstring> base_com::loopin_counter;

	bool bsae_com::inline_asm(std::wstring input,std::vector<std::wstring>& output)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"機械「(*)」。")))
		{
			output.push_back(out.str(1));
			return true;
		}
		return false;
	}

	bool base_com::if_begin(std::wstring input,std::vector<std::wstring>& output,std::vector<type::variable_type> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"もし(*)ならば「(*)。")))
		{
			wstring if_x = out.str(1),if_y;
			if(base_if(if_x,if_y,ref))
			{
				output.push_back(L"call " + if_y);
			}
			else
			{
				output.push_back(L"call " + to_wstring(fanc::find_id(if_x)));
			}
			output.push_back(L"jz i" + to_wstring(if_counter) + L"n");
			terms_stack.push(L"i" + to_wstring(if_counter));
			if_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"もし(*)ならば(*)そうでなければ(*)。")))
		{
			wstring if_x = out.str(1),if_y;
			if(base_if(if_x,if_y,ref))
			{
				output.push_back(L"call " + if_y);
			}
			else
			{
				output.push_back(L"call " + to_wstring(fanc::find_id(if_x)));
			}
			/*--if--*/
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"jz i" + to_wstring(if_counter) + L"n");
			output.push_back(L"call " + to_wstring(fanc::find_id(out.str(2))));
			output.push_back(L"jmp i" + to_wstring(if_counter));
			/*--else--*/
			output.push_back(L"i" + to_wstring(if_counter) + L"n:");
			output.push_back(L"call " + to_wstring(fanc::find_id(out.str(3))));
			output.push_back(L"i" + to_wstring(if_counter) + L":");
			if_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"もし(*)ならば(*)。")))
		{
			wstring if_x = out.str(1),if_y;
			if(base_if(if_x,if_y,ref))
			{
				output.push_back(L"call " + if_y);
			}
			else
			{
				output.push_back(L"call f" + to_wstring(fanc::find_id(if_x)));
			}
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"je i" + to_wstring(if_counter));
			output.push_back(L"call f" + to_wstring(fanc::find_id(out.str(2))));
			output.push_back(L"i" + to_wstring(if_counter) + L":");
			if_counter++;
			return true;
		}
		return false;
	}
	
	bool base_com::loop_begin(std::wstring input,std::vector<std::wstring>& output,std::vector<type::variable_type> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"(*)かぎり「(*)。")))
		{
			wstring if_x = out.str(1),if_y;
			output.push_back(L"l" + to_wstring(loop_counter) + L":");
			if(base_if(if_x,if_y,ref))
			{
				output.push_back(L"call f" + if_y);
			}
			else
			{
				output.push_back(L"call f" + to_wstring(fanc::find_id(if_x)));
			}
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"je l" + to_wstring(loop_counter) + L"e:");
			output.push_back(L"call f" + to_wstring(fanc::find_id(out.str(2))));
			terms_stack.push(L"l" + to_wstring(loop_counter));
			loop_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"(*)かぎり(*)。")))
		{
			wstring if_x = out.str(1),if_y;
			output.push_back(L"l" + to_wstring(loop_counter) + L":");
			if(base_if(if_x,if_y,ref))
			{
				output.push_back(L"call " + if_y);
			}
			else
			{
				output.push_back(L"call f" + to_wstring(fanc::find_id(if_x)));
			}
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"je l" + to_wstring(loop_counter) + L"e:");
			output.push_back(L"call f" + to_wstring(fanc::find_id(out.str(2))));
			output.push_back(L"jmp l" + to_wstring(loop_counter) + L":");
			output.push_back(L"l" + to_wstring(loop_counter) + L"e:");
			loop_counter++;
			return true;
		}
		return false;
	}

	bool base_com::terms_end(std::wstring input,std::vector<std::wstring>& output)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,wregex(L"」。")))
		{
			if(terms_stack.top()[0] == L'i')
			{
				if(terms_stack.top() == nstack.top())
				{
					output.push_back(terms_stack.top() + L"n:");
					terms_stack.pop();
				}
				else
				{
					output.push_back(terms_stack.top() + L"e:");
					terms_stack.pop();
					nstack.pop();
				}
				return true;
			}
			else if(terms_stack.top()[0] == L'l')
			{
				output.push_back(L"jmp " + terms_stack.top());
				output.push_back(terms_stack.top() + L"e:");
				terms_stack.pop();
				return true;
			}
		}
		else if(regex_match(input,out,wregex(L"」そうでなければ「(*)。")))
		{
			if(terms_stack.top()[0] == L'i')
			{
				output.push_back(terms_stack.top() + L"n:");
				output.push_back(L"call f" + to_wstring(fanc::find_id(out.str(1))));
				nstack.push(terms_stack.top());
				return true;
			}
			else
			{
				wcerr << L"構文が間違っています。" << endl;
				return false;
			}
		}

		return false;
	}

	bool base_com::base_if(std::wstring input,std::wstring& output,std::vector<type::variable_type> ref)
	{
		using namespace std;
		wsmatch out;
		type *rht,*lht;
		if(regex_match(input,out,wregex(L"(*)が(*)以下である")))
		{
			auto rend = ref.end();
			for(auto i = ref.begin();i != rend;i++)
			{
				if((*i).first == out.str(1))
					rht = (*i).second;
				if((*i).first == out.str(2))
					lht = (*i).second;
			}	
			output = rht->type_name() + L"_" + lht->type_name();
			return true;
		}
		return false;
	}

	operator_fanc::operator_fanc(type& rigth,type& left,int mode)
		:right_type(rigth),left_type(left),out_type(nullptr)

}

