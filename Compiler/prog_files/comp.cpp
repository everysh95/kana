#include "comp.h"
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
		//std::wcout << L"生成(" << name << L"," << id << L")" << std::endl;
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


	
	std::vector<std::wstring> vector_type::vec_outs;

	vector_type::vector_type(std::wstring name,unsigned int size,type* ref_type)
		:variable_type()
	{
		v_name = name;
		id = vec_outs.size();
		this->ref_type = ref_type;
		//std::wcout << L"生成(" << v_name << L"," << id <<  L")" << std::endl;
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

