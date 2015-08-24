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
			if(*i == nullptr)
			{
				std::wcerr << L"不正な型が存在します。" << std::endl;
			}
			else
			{
				//std::wcout << L"sr(" << target << L"," << (*i)->type_name() << L")"<< std::flush;
			}
			if(*i != nullptr && target == (*i)->type_name())
			{
				//std::wcout << L"sr(" << (*i)->type_name() << L")"<< std::flush;
				return *i;
			}
		}
		std::wcerr << L"型が定義されていません(" << target << L")" << std::endl;
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
		std::wcout << L"生成(" << name << L"," << id << L")" << std::endl;
		if(type != nullptr)
			ref_id += type->get_size();
		else if(comp_option::get_cpu() == comp_option::cpu::x64)
			ref_id += 8;
		else if(comp_option::get_cpu() == comp_option::cpu::x86)
			ref_id += 4;
	}

	variable_type::~variable_type()
	{}

	long variable_type::get_id()
	{return id;}

	std::wstring variable_type::get_name()
	{return v_name;}

	type* variable_type::get_type()
	{return ref_type;}

	variable_type* is_variable(std::wstring tar,std::vector<variable_type*> ref)
	{
		auto re = ref.end();
		for(auto rp = ref.begin();rp != re;rp++)
		{
			if((*rp) == nullptr)
				std::wcerr << L"不正変数が存在します。" << std::endl;
			else if((*rp)->get_name() == tar)
				return (*rp);
		}
		//std::wcerr << L"変数(" << tar << L")が存在しません。" << std::endl;
		return nullptr;
	}

	std::wstring convert_wstr(std::wstring tar,std::vector<variable_type*> ref)
	{
		auto re = ref.end();
		for(auto rp = ref.begin();rp != re;rp++)
		{
			if((*rp) == nullptr)
				std::wcerr << L"不正変数が存在します。" << std::endl;
			else if((*rp)->get_name() == tar)
				return (*rp)->is_wstring();
		}
		//std::wcerr << L"変数が存在しません。" << std::endl;
		return tar;
	}


	bool find_adress(std::wstring target_name,std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
	{
		variable_type* tar = is_variable(target_name,ref);
		if(tar != nullptr)
			return tar->is_adress(output,ref);
		else
		{
			std::wcerr << L"変数(" << target_name << L")は存在しません。" << std::endl;
			return false;
		}
	}

	std::wstring variable_type::is_wstring()
	{
		if(comp_option::get_cpu() == comp_option::cpu::x64)
		{
			return std::to_wstring(get_id()) + L"(\%rcx)";
		}
		if(comp_option::get_cpu() == comp_option::cpu::x86)
		{
			return std::to_wstring(get_id()) + L"(\%ecx)";
		}

		return L"";
	}


	bool variable_type::is_adress(std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
	{
		if(comp_option::get_cpu() == comp_option::cpu::x64)
		{
			output.push_back(L"movq $mm , \%rax");
			output.push_back(L"addq $" + std::to_wstring(id) + L" , \%rax");
		}
		if(comp_option::get_cpu() == comp_option::cpu::x86)
		{
			output.push_back(L"movl $mm , \%eax");
			output.push_back(L"addl $" + std::to_wstring(id) + L" , \%eax");
		}
		return true;
	}

	long variable_type::size_sum()
	{
		return ref_id;
	}



	/*関数オブジェクト*/

	fanc::fanc(std::wstring name)
		:com_name(name),com_low(name),const_flg(false),is_after_comp(false)
	{
		com_id = fancs.size();
		fancs.push_back(this);
	}

	fanc::~fanc()
	{
		auto ve = variables.end();
		for(auto p = variables.begin();p != ve;p++)
		{
			delete *p;
		}
	}

	std::wstring fanc::name()
	{return com_name;}

	bool fanc::add_com(std::wstring target)
	{
		if(target.empty())
			return false;
		else
			com_contents.push_back(target);
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
		vector<variable_type*> 
		variables,/*変数判定用*/
		ref_v;/*引数判定用*/
		type* out_type = nullptr;

		int ref_count = 0,out_count = 0;
		int line_count = 1;
		for(auto i = com_contents.begin();i != com_contents.end();i++)
		{
			/*リテラル定義かどうか判定*/
			if(regex_match(*i,result_mws,wregex(L"これは定型文である。")))
			{
				//wcout << L"dt" << flush;
				const_flg = true;
			}
			else if(const_flg && regex_match(*i,result_mws,wregex(L"これは「(.*)」という構文である。")))
			{
				//wcout << L"td" << flush;
				wstring tar = result_mws.str(1);
				wregex ref_regex(L"[(]([^(]*):([^(]*)[)]");
				wsregex_token_iterator first(begin(tar),end(tar),ref_regex,{1,2});
				wsregex_token_iterator last;
				while(first != last)
				{
					wstring v_name = first->str();
					++first;
					wstring t_name  = first->str();
					if(!t_name.empty())
					{
						type* rt = type::find_from_wstr(t_name);
						if(rt == nullptr)
						{
							return false;
						}
						variable_type* nvt = new variable_type(v_name,rt);
						this->ref_variables.push_back(nvt);
						variables.push_back(nvt);
						input_types.push_back(rt);
					}
					else
					{
						variable_type* nvt = new variable_type(v_name,nullptr);
						this->ref_variables.push_back(nvt);
						variables.push_back(nvt);
						input_types.push_back(nullptr);
					}
					++first;
				}
				//wcout << L"td(end1)" << flush;
				com_low = regex_replace(tar,wregex(L"[(](.*):(.*)[)]"),L"(.*)");
				com_low += L"(。*)";
				//wcout << L"td(end)" << flush;
			}
			else if(regex_match(*i,result_mws,wregex(L"(.*)は(.*)である。")))
			{
				/*--変数の宣言であった時の処理--*/
				/* 対象の型と名前の特定         */
				/* 変数の登録                   */
				//デバック用
				//wcout << L"ti" << flush;
				type* rt = type::find_from_wstr(result_mws.str(2));
				if(rt == nullptr)
				{
					return false;
				}
				variables.push_back(new variable_type(result_mws.str(1),rt));	
			}
			/*引数の宣言であるかどうかの判定*/
			if(!const_flg && regex_match(*i,result_mws,wregex(L"これは(.*)である(.*)を受け取る。")))
			{
				/*--引数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				/*引数の登録*/
				//デバック用
				//wcout << L"td" << flush;
				type* rt = type::find_from_wstr(result_mws.str(1));
				if(rt == nullptr)
				{
					return false;
				}
				this->input_types.push_back(rt);
				variable_type* nvt = new variable_type(result_mws.str(2),rt);
				this->ref_variables.push_back(nvt);
				variables.push_back(nvt);	
				ref_count++;

			}
			/*返り値の判定*/
			if(regex_match(*i,result_mws,wregex(L"これは(.*)を返す。",std::regex_constants::basic)) && !regex_match(*i,wregex(L"ここで(.*)を返す。")))
			{
				/*返り値の型宣言だったときの処理*/
				//デバック用
				//wcout << L"rt" << flush;
				out_type = type::find_from_wstr(result_mws.str(1));
				out_count++;
			}
			else if(regex_match(*i,result_mws,wregex(L"ここで(.*)を返す。")))
			{
				/*返り値だった場合の処理*/
				//デバック用
				//wcout << L"ri" << flush;
				auto vend = variables.end();
				auto refend = ref_v.end();
				wstring tar = result_mws.str(1);
				bool out_l = false;
				/*返り値が変数かどうか*/
				variable_type* rvt = is_variable(tar,variables);
				if(rvt != nullptr)
				{
					if(out_type != nullptr && *(rvt->get_type()) != *out_type)
					{
						cerr << "返り値の型が不正です。" << endl;
						return false;
					}
					else if(out_type == nullptr)
					{
						out_type =  rvt->get_type();
					}
					out_l = true;
				}
				/*返り値が引数かどうか*/
				rvt = is_variable(tar,ref_v);
				if(rvt != nullptr)
				{
					if(out_type != nullptr && *(rvt->get_type()) != *out_type)
					{
						cerr << "返り値の型が不正です。" << endl;
						return false;
					}
					else if(out_type == nullptr)
					{
						out_type =  rvt->get_type();
					}
					out_l = true;
				}
				/*返り値が関数かどうか*/
				vector<wstring> baf_com;
				if(asm_create(tar,baf_com,variables))
				{
					out_l = true;
				}
			}

		}

		//デバック用
		//wcout << L"be" << flush;

		//auto rve = ref_v.end();
		//for(auto rvp = ref_v.begin();rvp != rve;rvp++)
		//{
		//	this->input_types.push_back((*rvp)->get_type());
		//}
		this->output_type = out_type;
		if(!const_flg)
		{
			com_low = L"(.*)" + com_name + L"(。*)";
		}

		//for(int i = 0;i < this->ref_variables.size();i++)
		//{
		//	delete this->ref_variables[i];
		//}
		//this->ref_variables.clear();
		//this->ref_variables.insert(this->ref_variables.begin(),ref_v.begin(),ref_v.end());

		for(int i = 0;i < this->variables.size();i++)
		{
			delete this->variables[i];
		}
		this->variables.clear();
		this->variables.insert(this->variables.begin(),variables.begin(),variables.end());
		this->variables.insert(this->variables.begin(),ref_v.begin(),ref_v.end());

		return true;
	}

	std::vector<fanc*> fanc::fancs;
	fanc* fanc::now_main;

	bool fanc::main_compile()
	{
		using namespace std;
		/*--初期化--*/
		bool ans = true,baf_b = false,baf_b2 = true;
		auto fe = fancs.end();
		//std::vector<variable_type*> variables;/*変数判定用*/
		if(!const_flg)
			is_after_comp = true;
		asm_command.clear();
		now_main = this;

		asm_command.push_back(L"f" + to_wstring(com_id) + L":");
		if(comp_option::get_cpu() == comp_option::cpu::x86)
		{
			asm_command.push_back(L"pushl \%ebp");
			asm_command.push_back(L"movl \%esp, \%ebp");
			asm_command.push_back(L"movl $mm, \%ecx");
		}
		else if(comp_option::get_cpu() == comp_option::cpu::x64)
		{
			asm_command.push_back(L"pushq \%rbp");
			asm_command.push_back(L"movq \%rsp, \%rbp");
			asm_command.push_back(L"movq $mm, \%rcx");
		}

		//wcout << L"cb" << endl;

		for(int i = 0;i < com_contents.size();i++)
		{
			baf_b = asm_create(com_contents[i],asm_command,this->variables);
			if(!baf_b)
			{
				wcerr << L"不正:「" << name() << L"手順」の" << (i + 1) << L"行目" << endl;
				wcerr << com_contents[i] << endl;
				for(int j = 0;j < fancs.size();j++)
				{
					wcerr << fancs[j]->com_low << endl;
				}
			}
			else
			{
				//wcout << L"[" << i << L"]" << flush;
			}
			ans = ans && baf_b;
		}

		//wcout << endl;
		//wcout << L"ce" << flush;

		asm_command.push_back(L"f" + to_wstring(com_id) + L"_end:");
		asm_command.push_back(L"leave");
		asm_command.push_back(L"ret");
		
		is_after_comp = true;

		return ans;
	}

	bool fanc::asm_comp(std::wstring input,std::vector<std::wstring>& target,std::vector<variable_type*>& ref)
	{
		using namespace std;

		if(!is_after_comp)
			return false;

		//	wcout << "(" << this->name() << ")" << endl;

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
				queue<variable_type*> variables;

				for(int i = 0;i < its;i++)
				{
					variable_type* vt = is_variable(input_m.str(i + 1),ref),*vta;
					if(input_types[i] == nullptr)
					{
						//wcout << com_low << endl;
						ref.push_back(new vector_type(input_m.str(i + 1),input_m.str(i + 1).size() + 1,find_from_wstr(L"文字")));
					}
					else if(vt == nullptr)
					{
						return false;
					}
					else if(vt->get_type() != nullptr && *(vt->get_type()) != *(input_types[i]))
					{
						return false;
					}
					else
					{
						vta = vt;
					}
					variables.push(vta);
				}
				for(int i = 0;i< its;i++)
				{
					variable_type* vt = variables.front();
					if(vt->get_type() != nullptr && input_types[i] != nullptr)
					{
						//通常の変数だった場合
						if(comp_option::get_cpu() == comp_option::cpu::x86)
						{
							target.push_back(L"movl " + convert_wstr(vt->get_name(),ref) + L" , \%ebx" );
							target.push_back(L"movl \%ebx, " + convert_wstr(ref_variables[i]->get_name(),ref_variables));
						}
						else if(comp_option::get_cpu() == comp_option::cpu::x64)
						{
							target.push_back(L"movq " + convert_wstr(vt->get_name(),ref) + L" , \%rbx");
							target.push_back(L"movq \%rbx, " + convert_wstr(ref_variables[i]->get_name(),ref_variables));
						}
					}
					else if(input_types[i] == nullptr)
					{
						wstring t = vt->get_name();
						int s = t.size();
						type* letter = find_from_wstr(L"文字");
						if(comp_option::get_cpu() == comp_option::cpu::x64)
						{
							target.push_back(L"movq " + convert_wstr(input_m.str(i + 1),ref) + L" , \%rdx");
							for(int j = 0;j < s;j++)
							{
								//wstring wc_buf;
								//wc_buf += t[j];
								//target.push_back(L"movl $\'" + wc_buf + L"\' , " + to_wstring(j * letter->get_size()) + L"(\%rdx)");
								target.push_back(L"movl $" + to_wstring(t[j]) + L" , " + to_wstring(j * letter->get_size()) + L"(\%rdx)");
							}
							target.push_back(L"movl $0 , " + to_wstring(s * letter->get_size()) + L"(\%rdx)");
							target.push_back(L"movq \%rdx," + convert_wstr(ref_variables[i]->get_name() , ref_variables));
						}
					}
					variables.pop();
				}
				target.push_back(L"call f" + to_wstring(com_id));
			}
			else
			{
				//定型文でなかった場合。
				wstring ii = input;
				ii = regex_replace(ii,wregex(L"[^を].*に"),L"");
				ii = regex_replace(ii,wregex(L".*に"),L"");
				ii = regex_replace(ii,wregex(L"を.*"),L"と");
				ii = ii;

				//wcout << ii << endl;

				//wregex ref_regex(L"(.*)を");
				wregex ref_regex(L"([^と]*)と");
				wsregex_token_iterator vpt(begin(ii),end(ii),ref_regex,{1});
				wsregex_token_iterator ver;
				//wsregex_iterator vpt(begin(ii),end(ii),and_regex);
				//wsregex_iterator ver;
				queue<variable_type*> variables;
				int count_it = 0;
				int max_it = this->input_types.size();
				while(vpt != ver)
				{
					//wcout << vpt->str() << endl;
					variable_type* vt = is_variable(vpt->str(),ref);
					bool iot = false;
					for(auto it_ptr = it_begin;vt != nullptr &&it_ptr != it_end;it_ptr++)
					{
						if(*it_ptr == nullptr || vt->get_type() == nullptr)
						{
							iot = false;
							break;
						}
						if(**it_ptr == *(vt->get_type()))
						{
							iot = true;
							break;
						}
					}
					if(!iot)
						return false;
					variables.push(vt);
					count_it++;
					if(count_it >= max_it)
					{
						break;
					}
					++vpt;
				}

				int ad = 0;
				while(!variables.empty())
				{
					if(ad >= input_types.size())
					{
						return false;
					}
					variable_type* vt = variables.front();
					if(ref_variables[ad]->get_type() == nullptr)
					{
						return false;
					}
					else if(vt->get_type() == nullptr)
					{}
					else if( *(ref_variables[ad]->get_type()) == *(vt->get_type()) )
					{
						if(comp_option::get_cpu() == comp_option::cpu::x86)
						{
							target.push_back(L"movl " + convert_wstr(vt->get_name(),ref) + L" , \%ebx");
							target.push_back(L"movl \%ebx, " + convert_wstr(ref_variables[ad]->get_name(),ref_variables));
						}
						else if(comp_option::get_cpu() == comp_option::cpu::x64)
						{
							target.push_back(L"movq " + convert_wstr(vt->get_name(),ref) + L" , \%rbx");
							target.push_back(L"movq \%rbx, " + convert_wstr(ref_variables[ad]->get_name(),ref_variables));
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
				wsmatch output_m;
				if(regex_match(input,output_m,wregex(L"(.*)に(.*)")))
				{
					if(comp_option::get_cpu() == comp_option::cpu::x64)
					{
						target.push_back(L"movq $mm,\%rcx");
						target.push_back(L"movq \%rax , " + convert_wstr(output_m.str(1),ref));
					}
					if(comp_option::get_cpu() == comp_option::cpu::x86)
					{
						target.push_back(L"movl $mm,\%ecx");
						target.push_back(L"movl \%eax , " + convert_wstr(output_m.str(1),ref));
					}
				}
			}
			return true;
		}
		return false;
	}
	

	bool filter_com(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type*>& ref)
	{
		return fanc::asm_create(wstr,target,ref);
	}
	bool fanc::asm_create(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type*>& ref)
	{
		using namespace std;
		std::wstring ans;
		std::vector<std::wstring> vans;
		bool baf_b = false,baf_b2;
		auto fe = fanc::fancs.end();
		for(auto j = fanc::fancs.begin();j != fe;j++)
		{
			baf_b = baf_b || (*j)->asm_comp(wstr,target,ref);
		}

		/*--基本的な構文の処理--*/
		if(!baf_b && base_com::inline_asm(wstr,target,ref))
			baf_b = true;
		if(!baf_b && base_com::if_begin(wstr,target,ref))
			baf_b = true;
		if(!baf_b && base_com::loop_begin(wstr,target,ref))
			baf_b = true;
		if(!baf_b && base_com::terms_end(wstr,target,ref))
			baf_b = true;
		if(!baf_b && predefine::asm_comp(wstr,target,ref))
			baf_b = true;

		wsmatch result_mws;
		if(regex_match(wstr,wregex(L"これは定型文である。")))
			baf_b = true;
		if(regex_match(wstr,wregex(L"これは「(.*)」という構文である。")))
			baf_b = true;
		if(regex_match(wstr,wregex(L"これは(.*)である(.*)を受け取る。")))
			baf_b = true;
		else if(regex_match(wstr,result_mws,wregex(L"これは(.*)を返す。")))
			baf_b = true;
		else if(regex_match(wstr,result_mws,wregex(L"ここで(.*)を返す。")))
		{		
			/*返り値だった場合の処理*/
			//デバック用
			//wcout << L"ri" << flush;
			auto refend = ref.end();
			wstring tar = result_mws.str(1);
			vector<wstring> baf_com;
			/*返り値が変数かどうか*/
			variable_type* rvt = is_variable(tar,ref);
			if(rvt != nullptr)
			{
				target.push_back(L"movq " + convert_wstr(tar,ref) + L",\%rax");
				target.push_back(L"jmp f" + to_wstring(now_main->com_id) + L"_end");
				baf_b = true;
			}
			/*返り値が関数かどうか*/
			else if(asm_create(tar,target,ref))
			{
				target.push_back(L"jmp f" + to_wstring(now_main->com_id) + L"_end");
				baf_b = true;
			}
		}
		if(regex_match(wstr,wregex(L"(.*)は(.*)である。")))
			baf_b = true;

		return baf_b;
	}

	/*--base_com--*/
	long base_com::if_counter = 0;
	long base_com::loop_counter = 0;
	std::stack<std::wstring> base_com::terms_stack;
	std::stack<std::wstring> base_com::nstack;

	bool base_com::inline_asm(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"機械「(.*)」(.*)「(.*)」(.*)")))
		{
			wstring ans = out.str(1);
			variable_type* rt = is_variable(out.str(2),ref);
			if(rt != nullptr)
			{
				ans += convert_wstr(out.str(2),ref);
			}
			ans += out.str(3);
			rt = is_variable(out.str(4),ref);
			if(rt != nullptr)
			{
				ans += convert_wstr(out.str(4),ref);
			}
			output.push_back(ans);
			return true;
		}
		else if(regex_match(input,out,wregex(L"機械「(.*)」(.*)")))
		{
			output.push_back(out.str(1));
			return true;
		}
		return false;
	}

	bool base_com::if_begin(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"もし(.*)ならば「(.*)。")))
		{
			wstring if_x = out.str(1),if_y = out.str(2);
			filter_com(if_x,output,ref);
			if(comp_option::get_cpu() == comp_option::cpu::x86)
				output.push_back(L"cmpl $0 , \%eax");
			if(comp_option::get_cpu() == comp_option::cpu::x64)
				output.push_back(L"cmpq $0 , \%rax");
			output.push_back(L"je i" + to_wstring(if_counter) + L"n");
			terms_stack.push(L"i" + to_wstring(if_counter));
			filter_com(if_y,output,ref);
			if_counter++;
			return true;
		}
		else if(regex_match(input,out,wregex(L"もし(.*)ならば(.*)そうでなければ(.*)。")))
		{
			wstring if_x = out.str(1);
			filter_com(if_x,output,ref);
			/*--if--*/
			if(comp_option::get_cpu() == comp_option::cpu::x86)
				output.push_back(L"cmpl $0 , \%eax");
			if(comp_option::get_cpu() == comp_option::cpu::x64)
				output.push_back(L"cmpq $0 , \%rax");
			output.push_back(L"je i" + to_wstring(if_counter) + L"n");
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
			if(comp_option::get_cpu() == comp_option::cpu::x86)
				output.push_back(L"cmpl $0 , \%eax");
			if(comp_option::get_cpu() == comp_option::cpu::x64)
				output.push_back(L"cmpq $0 , \%rax");
			output.push_back(L"je i" + to_wstring(if_counter));
			filter_com(out.str(2),output,ref);
			output.push_back(L"i" + to_wstring(if_counter) + L":");
			if_counter++;
			return true;
		}
		return false;
	}
	
	bool base_com::loop_begin(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,out,wregex(L"(.*)かぎり「(.*)。")))
		{
			wstring if_x = out.str(1),if_y;
			output.push_back(L"l" + to_wstring(loop_counter) + L":");
			filter_com(if_x,output,ref);
			if(comp_option::get_cpu() == comp_option::cpu::x86)
				output.push_back(L"cmpl $0 , \%eax");
			if(comp_option::get_cpu() == comp_option::cpu::x64)
				output.push_back(L"cmpq $0 , \%rax");
			output.push_back(L"je l" + to_wstring(loop_counter) + L"e");
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
			if(comp_option::get_cpu() == comp_option::cpu::x86)
				output.push_back(L"cmpl $0 , \%eax");
			if(comp_option::get_cpu() == comp_option::cpu::x64)
				output.push_back(L"cmpq $0 , \%rax");
			output.push_back(L"je l" + to_wstring(loop_counter) + L"e");
			filter_com(out.str(2),output,ref);
			output.push_back(L"jmp l" + to_wstring(loop_counter));
			output.push_back(L"l" + to_wstring(loop_counter) + L"e:");
			loop_counter++;
			return true;
		}
		return false;
	}

	bool base_com::terms_end(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*> ref)
	{
		using namespace std;
		wsmatch out;
		if(regex_match(input,wregex(L"」。")) && !terms_stack.empty())
		{
			wstring top = terms_stack.top();
			if(top[0] == L'i')
			{
				if(terms_stack.top() != nstack.top())
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
			else if(top[0] == L'l')
			{
				output.push_back(L"jmp " + terms_stack.top());
				output.push_back(terms_stack.top() + L"e:");
				terms_stack.pop();
				return true;
			}
		}
		else if(regex_match(input,out,wregex(L"」そうでなければ「(.*)。")))
		{
			wstring top = terms_stack.top();
			if(top[0] == L'i')
			{
				output.push_back(L"jmp " + terms_stack.top() + L"e");
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

	bool base_com::base_if(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*> ref)
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

	namespace
	{
		//基本定型文
		bool type_set(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
		{
			using namespace std;

			wsmatch out;
			if(regex_match(input,out,wregex(L"文字「(.)」(.*)")))
			{
				wchar_t baf = (out.str(1))[0];
				if(comp_option::get_cpu() == comp_option::cpu::x64)
					output.push_back(L"movq $" + to_wstring(baf) + L",\%rax");
				if(comp_option::get_cpu() == comp_option::cpu::x86)
					output.push_back(L"movl $" + to_wstring(baf) + L",\%eax");
				return true;
			}
			else if(regex_match(input,out,wregex(L"数値「(.*)」(.*)")))
			{
				wstring baf = out.str(1);
				if(comp_option::get_cpu() == comp_option::cpu::x64)
					output.push_back(L"movq $" + baf + L",\%rax");
				if(comp_option::get_cpu() == comp_option::cpu::x86)
					output.push_back(L"movl $" + baf + L",\%eax");
				return true;
			}
			else if(regex_match(input,out,wregex(L"(.*)の場所(.*)")))
			{
				//wcout << L"PDfind" << out.str(1) << endl;
				if(find_adress(out.str(1),output,ref))
				{
					//wcout << L"PDfindT" << endl;
					return true;
				}
				else
				{
					//wcout << L"PDfindF" << endl;
					false;
				}
			}
			return false;
		}

		//基本代入手順
		bool type_mv(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
		{
			using namespace std;
			wsmatch out;
			//wcout << L"PDmv?" << endl;
			if(regex_match(input,out,wregex(L"(.*)に(.*)を代入する。")))
			{
				//wcout << L"PDmvT" << endl;
				if(filter_com(out.str(2),output,ref))
				{
					if(comp_option::get_cpu() == comp_option::cpu::x64)
						output.push_back(L"movq \%rax, \%rbx");
					if(comp_option::get_cpu() == comp_option::cpu::x86)
						output.push_back(L"movl \%eax, \%ebx");
				}
				else
				{
					if(comp_option::get_cpu() == comp_option::cpu::x64)
						output.push_back(L"movq " + convert_wstr(out.str(2),ref) + L" , \%rbx");
					if(comp_option::get_cpu() == comp_option::cpu::x86)
						output.push_back(L"movl " + convert_wstr(out.str(2),ref) + L" , \%ebx");
				}

				//wcout << L"PDmvT2nd" << endl;
				if(comp_option::get_cpu() == comp_option::cpu::x64)
					output.push_back(L"movq \%rbx , " + convert_wstr(out.str(1),ref) );
				if(comp_option::get_cpu() == comp_option::cpu::x86)
					output.push_back(L"movl \%ebx , " + convert_wstr(out.str(1),ref) );

				return true;
			}
			//wcout << L"PDmvF" << endl;

			return false;
		}

		bool ac_vector(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
		{
			using namespace std;
			wsmatch out;
			if(regex_match(input,out,wregex(L"(.*)[(.*)]")))
			{
				
				if(comp_option::get_cpu() == comp_option::cpu::x64)
				{
					output.push_back(L"movq " + convert_wstr(out.str(1),ref) + L" , \%rdx");
					output.push_back(L"addq " + convert_wstr(out.str(2),ref) + L" , \%rdx");
					output.push_back(L"movq (\%rdx) , \%rax");
				}
				if(comp_option::get_cpu() == comp_option::cpu::x86)
				{
					output.push_back(L"movl " + convert_wstr(out.str(1),ref) + L" , \%edx");
					output.push_back(L"addl "  + convert_wstr(out.str(2),ref) + L" , \%edx");
					output.push_back(L"movl (\%edx) , \%eax");
				}
				return true;
			}
			return false;
		}

		bool set_vector(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
		{
			using namespace std;
			wsmatch out;

			if(regex_match(input,out,wregex(L"(.*)は大きさ(.*)の(.*)の配列である。")))
			{
				unsigned int size = std::stoi(out.str(2));
				type* ref_type = type::find_from_wstr(out.str(3));
				ref.push_back(new vector_type(out.str(1),size,ref_type));
			}
		}

	}


	bool predefine::asm_comp(std::wstring input,std::vector<std::wstring>& output,std::vector<variable_type*>& ref)
	{
		//std::wcout << L"PDon" << std::endl;
		if(type_mv(input,output,ref))
		{
		//std::wcout << L"PDmv" << std::endl;
			return true;
		}
		else if(type_set(input,output,ref))
		{
		//std::wcout << L"PDset" << std::endl;
			return true;
		}
		else if(ac_vector(input,output,ref))
		{
			return true;
		}
		return false;
	}
	
	std::vector<std::wstring> vector_type::vec_outs;

	vector_type::vector_type(std::wstring name,unsigned int size,type* ref_type)
		:variable_type()
	{
		v_name = name;
		id = vec_outs.size();
		this->ref_type = ref_type;
		std::wcout << L"生成(" << v_name << L"," << id <<  L")" << std::endl;
		vec_outs.push_back(L"m" + std::to_wstring(id) + L": .skip " + std::to_wstring(size * ref_type->get_size()));
	}

	std::wstring vector_type::is_wstring()
	{
		return L"$m" + std::to_wstring(id);
	}

	std::vector<std::wstring> vector_type::vector_output()
	{
		return vec_outs;
	}

	std::wstring del_nt(std::wstring target)
	{
		std::wstring ans = std::regex_replace(target,std::wregex(L"\n"),L"");
		ans = std::regex_replace(ans,std::wregex(L"\t"),L"");
		//ans = std::regex_replace(ans,std::wregex(L" "),L"");
		ans = std::regex_replace(ans,std::wregex(L"　"),L" ");
		return ans;
	}

	std::wstring del_comment(std::wstring target)
	{
		std::wstring ans = std::regex_replace(target,std::wregex(L"//(.*)\n"),L"");
		ans = std::regex_replace(ans,std::wregex(L"[/][*](.*)[*][/]"),L"");
		return ans;
	}
}

