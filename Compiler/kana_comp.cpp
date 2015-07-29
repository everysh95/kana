#include "kana_comp.h"
#include <queue>

namespace kana
{
	std::vector<type*> type::type_target;

	type::type()
	{}

	type::type(std::wstring t_name,unsigned int size)
		:t_name(t_name),t_size(size)
	{
		type_target.push_back(this);
	}

	type::type(std::wstring t_name,std::vector<std::wstring> castable_type,unsigned int size)
		:t_name(t_name),t_size(size)
	{
		type_target.push_back(this);
		auto cte = castable_type.end();
		for(auto ctp = castable_type.begin();ctp != cte;ctp++)
		{
			type* rt_type = find_from_wstr(*ctp);
			if(rt_type != nullptr)
			{
				castable_types.push_back(find_from_wstr(*ctp));
			}
		}
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

	std::vector<std::wstring> type::define_type()
	{
		return std::vector<std::wstring>();
	}

	int type::get_size()
	{
		if(t_size > 0)
			return t_size;
		else
			return 1;
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



	/*変数オブジェクト*/

	long variable_type::ref_id = 0;

	variable_type::variable_type()
	{}

	variable_type::variable_type(std::wstring name,type* type)
		:v_name(name),ref_type(type)
	{
		id = ref_id;
		ref_id += type->get_size();
	}

	long variable_type::get_id()
	{return id;}

	std::wstring variable_type::get_name()
	{return v_name;}

	type* variable_type::get_type()
	{return ref_type;}

	variable_type is_variable(std::wstring tar,std::vector<variable_type> ref)
	{
		auto re = ref.end();
		for(auto rp = ref.begin();rp != re;rp++)
		{
			if((*rp).get_name() == tar)
				return (*rp);
		}
		return variable_type();
	}



	/*関数オブジェクト*/

	fanc::fanc(std::wstring name)
		:com_name(name),com_low(name),const_flg(false)
	{
		com_id = fancs.size();
		fancs.push_back(this);
	}

	std::wstring fanc::name()
	{return com_name;}

	bool fanc::add_com(std::wstring target)
	{
		std::wstring input_com = target;
		if(target.empty())
			return false;
		com_contents.push_back(input_com);
		return true;
	}

	long fanc::find_id(std::wstring ref)
	{
		auto fe = fancs.end();
		for(auto fp = fancs.begin();fp != fe;fp++)
		{
			if((*fp)->name() == ref)
			{
				return (*fp)->com_id;
			}
		}
		return -1;
	}

	std::vector<std::wstring> fanc::output_com()
	{return asm_command;}

	std::vector<std::wstring> fanc::output_data()
	{return asm_data;}

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
			/*リテラル定義かどうか判定*/
			if(regex_match(*i,result_mws,wregex(L"これは定型文である。")))
			{
				const_flg = true;
			}
			if(const_flg && regex_match(*i,result_mws,wregex(L"これは「(.*)」という構文である。")))
			{
				wstring tar = result_mws.str(1);
				wsregex_token_iterator first(begin(tar),end(tar),wregex(L"[(](.*):(.*)[)]"),{1,2});
				wsregex_token_iterator last;
				while(first != last)
				{
					wstring v_name = first->str();
					wstring t_name  = first->str();
					if(!t_name.empty())
					{
						type* rt = type::find_from_wstr(t_name);
						if(rt == nullptr)
						{
							return false;
						}
						ref_v.push_back(variable_type(v_name,rt));
					}
					else
						ref_v.push_back(variable_type(v_name,nullptr));
					++first;
				}
				com_low = regex_replace(tar,wregex(L"[(](.*):(.*)[)]"),L"(.*)");
			}
			/*引数の宣言であるかどうかの判定*/
			if(!const_flg && regex_match(*i,result_mws,wregex(L"これは(.*)である(.*)を受け取る。")))
			{
				/*--引数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				/*引数の登録*/
				ref_v.push_back(variable_type(result_mws.str(1),type::find_from_wstr(result_mws.str(2))));
				ref_count++;

			}
			/*返り値の判定*/
			if(regex_match(*i,result_mws,wregex(L"これは(.*)を返す。",std::regex_constants::basic)) && !regex_match(*i,wregex(L"ここで(.*)を返す。")))
			{
				/*返り値の型宣言だったときの処理*/
				out_type = type::find_from_wstr(result_mws.str(1));
				out_count++;
			}
			else if(regex_match(*i,result_mws,wregex(L"ここで(.*)を返す。")))
			{
				/*返り値だった場合の処理*/
				auto vend = variables.end();
				auto refend = ref_v.end();
				wstring tar = result_mws.str(1);
				bool out_l = false;
				/*返り値が変数かどうか*/
				for(auto j = variables.begin();j != vend;j++)
				{
					if((*j).get_name() == tar)
					{
						/*返り値が変数だった場合の処理*/
						if(out_type != nullptr && *((*j).get_type()) != *out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type == nullptr)
						{
							out_type =  (*j).get_type();
						}
						out_l = true;
					}
				}
				/*返り値が引数かどうか*/
				for(auto j = ref_v.begin();j != refend;j++)
				{
					if((*j).get_name() == tar)
					{
						/*引数だった場合の処理*/
						if(out_type != nullptr && *((*j).get_type()) != *out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type == nullptr)
						{
							out_type = (*j).get_type();
						}
						out_l = true;
					}		
				}
				/*定数かどうか*/
				if(out_type != nullptr)
				{
					if(!out_l && !out_type->consted_type(tar))
					{
						cerr << "返り値が不正です。" << endl;
						return false;
					}
				}
			}

			if(regex_match(*i,result_mws,wregex(L"(.*)は(.*)である。")))
			{
				/*--変数の宣言であった時の処理--*/
				/* 対象の型と名前の特定         */
				/* 変数の登録                   */
				variables.push_back(variable_type(result_mws.str(1),type::find_from_wstr(result_mws.str(2))));	
			}
		}

		auto rve = ref_v.end();
		for(auto rvp = ref_v.begin();rvp != rve;rvp++)
		{
			this->input_types.push_back((*rvp).get_type());
		}
		this->output_type = out_type;
		if(!const_flg)
		{
			com_low = L"(.*)" + com_name + L"(.*)";
		}

		return true;
	}

	std::vector<fanc*> fanc::fancs;

	bool fanc::main_compile()
	{
		using namespace std;
		/*--初期化--*/
		bool ans = true,baf_b = false,baf_b2 = true;
		auto fe = fancs.end();
		std::vector<variable_type> variables;/*変数判定用*/
		asm_command.clear();

		asm_command.push_back(L"f" + to_wstring(com_id) + L":");
		if(comp_option::get_cpu() == comp_option::cpu::x86)
		{
			asm_command.push_back(L"pushl \%ebp");
			asm_command.push_back(L"movl \%esp, \%ebp");
		}
		else if(comp_option::get_cpu() == comp_option::cpu::x64)
		{
			asm_command.push_back(L"pushq \%rbp");
			asm_command.push_back(L"movq \%rsp, \%rbp");
		}

		for(int i = 0;i < com_contents.size();i++)
		{
			baf_b = asm_create(com_contents[i],asm_command,variables);
			if(!baf_b)
			{
				wcerr << L"不正:「" << name() << L"」の" << i << L"行目" << endl;
				wcerr << com_contents[i] << endl;
			//	for(int j = 0;j < fancs.size();j++)
			//	{
			//		wcerr << fancs[j]->com_low << endl;
			//	}
			}
			ans = ans && baf_b;
		}
		asm_command.push_back(L"f" + to_wstring(com_id) + L"_end:");
		asm_command.push_back(L"leave");
		asm_command.push_back(L"ret");
		return ans;
	}

	bool fanc::asm_comp(std::wstring input,std::vector<std::wstring>& target,std::vector<variable_type>& ref)
	{
		using namespace std;
		wstring ans;
		auto it_begin = input_types.begin();
		auto it_end = input_types.end();
		if(regex_match(input,wregex(com_low)))
		{
			if(const_flg)
			{
				//定型文だった場合
				wsmatch input_m;
				regex_match(input,input_m,wregex(com_low));
				size_t its = input_types.size();
				queue<variable_type> variables;

				for(int i = 0;i < its;i++)
				{
					variable_type vt = is_variable(input_m.str(i + 1),ref);
					if(vt.get_type() == nullptr || *(vt.get_type()) != *(input_types[i]))
					{
						return false;
					}
					variables.push(vt);
				}
				for(int i = 0;i< its;i++)
				{
					variable_type vt = variables.front();
					if(comp_option::get_cpu() == comp_option::cpu::x86)
					{
						target.push_back(L"pushl " + to_wstring(-vt.get_id()) + L"(\%ebp)");
					}
					else if(comp_option::get_cpu() == comp_option::cpu::x64)
					{
						target.push_back(L"pushq " + to_wstring(-vt.get_id()) + L"(\%rbp)");
					}
					variables.pop();
				}

				target.insert(target.end(),asm_command.begin(),asm_command.end());
				for(int i = 0;i< its;i++)
				{
					if(comp_option::get_cpu() == comp_option::cpu::x86)
					{
						target.push_back(L"popl \%edx");
					}
					else if(comp_option::get_cpu() == comp_option::cpu::x64)
					{
						target.push_back(L"popq \%rdx");
					}
				}
			}
			else
			{
				wsregex_token_iterator vpt(begin(input),end(input),wregex(L"(.*)を"),{1});
				wsregex_token_iterator ver;
				queue<variable_type> variables;
				while(vpt != ver)
				{
					variable_type vt = is_variable(vpt->str(),ref);
					bool iot = false;
					for(auto it_ptr = it_begin;it_ptr != it_end;it_ptr++)
					{
						if(**it_ptr == *(vt.get_type()))
						{
							iot = true;
							break;
						}
					}
					if(!iot)
					{
						return false;
					}
					variables.push(vt);
					++vpt;
				}

				int ad = 0;
				while(!variables.empty())
				{
					if(ad >= input_types.size())
					{
						return false;
					}
					variable_type vt = variables.front();
					if(*(input_types[ad]) == *(vt.get_type()))
					{
						if(comp_option::get_cpu() == comp_option::cpu::x86)
						{
						target.push_back(L"pushl " + to_wstring(-vt.get_id()) + L"(\%ebp)");
						}
						else if(comp_option::get_cpu() == comp_option::cpu::x64)
						{
						target.push_back(L"pushq " + to_wstring(-vt.get_id()) + L"(\%rbp)");
						}
						variables.pop();
						ad++;
					}
					else
					{
						variables.pop();
						variables.push(vt);
					}
				}
				if(ad < input_types.size())
				{
					return false;
				}
				target.push_back(L"call f" + to_wstring(com_id));
				for(int i = 0;i< input_types.size();i++)
				{
					if(comp_option::get_cpu() == comp_option::cpu::x86)
					{
						target.push_back(L"popl \%edx");
					}
					else if(comp_option::get_cpu() == comp_option::cpu::x64)
					{
						target.push_back(L"popq \%rdx");
					}
				}
			}
			return true;
		}
		return false;
	}
	

	bool filter_com(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type>& ref)
	{
		return fanc::asm_create(wstr,target,ref);
	}
	bool fanc::asm_create(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type>& ref)
	{
		std::wstring ans;
		std::vector<std::wstring> vans;
		bool baf_b = false,baf_b2;
		auto fe = fanc::fancs.end();
		for(auto j = fanc::fancs.begin();j != fe;j++)
		{
			baf_b2 = (*j)->asm_comp(wstr,target,ref);
			baf_b = baf_b || baf_b2;
		}

		/*--基本的な構文の処理--*/
		if(!baf_b && base_com::inline_asm(wstr,target))
			baf_b = true;
		if(!baf_b && base_com::if_begin(wstr,target,ref))
			baf_b = true;
		if(!baf_b && base_com::loop_begin(wstr,target,ref))
			baf_b = true;
		if(!baf_b && base_com::terms_end(wstr,target,ref))
			baf_b = true;

		return baf_b;
	}

	/*--base_com--*/
	long base_com::if_counter = 0;
	long base_com::loop_counter = 0;
	std::stack<std::wstring> base_com::terms_stack;
	std::stack<std::wstring> base_com::nstack;

	bool base_com::inline_asm(std::wstring input,std::vector<std::wstring>& output)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"機械「(.*)」(.*)")))
		{
			output.push_back(out.str(1));
			return true;
		}
		return false;
	}

	bool base_com::if_begin(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"もし(.*)ならば「(.*)。")))
		{
			wstring if_x = out.str(1);
			filter_com(if_x,output,ref);
			output.push_back(L"jz i" + to_wstring(if_counter) + L"n");
			terms_stack.push(L"i" + to_wstring(if_counter));
			if_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"もし(.*)ならば(.*)そうでなければ(.*)。")))
		{
			wstring if_x = out.str(1);
			filter_com(if_x,output,ref);
			/*--if--*/
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"jz i" + to_wstring(if_counter) + L"n");
			filter_com(out.str(2),output,ref);
			output.push_back(L"jmp i" + to_wstring(if_counter));
			/*--else--*/
			output.push_back(L"i" + to_wstring(if_counter) + L"n:");
			filter_com(out.str(3),output,ref);
			output.push_back(L"i" + to_wstring(if_counter) + L":");
			if_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"もし(.*)ならば(.*)。")))
		{
			wstring if_x = out.str(1),if_y;
			filter_com(if_x,output,ref);
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"je i" + to_wstring(if_counter));
			filter_com(out.str(2),output,ref);
			output.push_back(L"i" + to_wstring(if_counter) + L":");
			if_counter++;
			return true;
		}
		return false;
	}
	
	bool base_com::loop_begin(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"(.*)かぎり「(.*)。")))
		{
			wstring if_x = out.str(1),if_y;
			output.push_back(L"l" + to_wstring(loop_counter) + L":");
			filter_com(if_x,output,ref);
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"je l" + to_wstring(loop_counter) + L"e:");
			filter_com(out.str(2),output,ref);
			terms_stack.push(L"l" + to_wstring(loop_counter));
			loop_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"(.*)かぎり(.*)。")))
		{
			wstring if_x = out.str(1),if_y;
			output.push_back(L"l" + to_wstring(loop_counter) + L":");
			filter_com(if_x,output,ref);
			output.push_back(L"cmpl $0 \%eax");
			output.push_back(L"je l" + to_wstring(loop_counter) + L"e:");
			filter_com(out.str(2),output,ref);
			output.push_back(L"jmp l" + to_wstring(loop_counter) + L":");
			output.push_back(L"l" + to_wstring(loop_counter) + L"e:");
			loop_counter++;
			return true;
		}
		return false;
	}

	bool base_com::terms_end(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type> ref)
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
		else if(regex_match(input,out,wregex(L"」そうでなければ「(.*)。")))
		{
			if(terms_stack.top()[0] == L'i')
			{
				output.push_back(terms_stack.top() + L"n:");
				filter_com(out.str(1),output,ref);
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

	bool base_com::base_if(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type> ref)
	{
		return filter_com(input,output,ref);
	}

	unsigned int comp_option::cpu_type = comp_option::cpu::x86;

	void comp_option::set_cpu(unsigned int cpu)
	{cpu_type = cpu;}

	unsigned int comp_option::get_cpu()
	{
		return cpu_type;
	}

}

