#include<kana_comp.h>

namespace kana
{
	std::vector<type*> type::type_target;

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

	~type()
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
			if(t_type == *i)
				return true;
		}
		return false;
	}

	bool operator==(const type& rhs)const
	{
		/*型の判定*/
		if(rhs.type_name() == type_name())
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
			for(auto ti = castable_type.begin();ti != tend;ti++)
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
		std::wstring input_com = fliter_str(target);
		com_cont.push_back(input_com);
		return true;
	}

	bool fanc::precompile()
	{
		using namespace std;
		com_low = "";

		wregex 
		input_l(L"*である*を受け取る。",std::regex_contant::basic),
		output_l(L"*を返す。",std::regex_content::basic),
		output_nl(L"ここで*を返す。",std::regex_content::basic),
		variable_l(L"*は*である。",std::regex_content::basic);

		wsmatch result_mws;
		vector<variable_type> 
		variables,/*変数判定用*/
		ref_v;/*引数判定用*/
		type out_type(L"");

		int ref_conunt = 0,out_conunt = 0;
		for(auto i = com_contents.begin();i != com_contents.end();i++)
		{
			/*引数の宣言であるかどうかの判定*/
			if(regex_match(*i,inputl))
			{
				/*--引数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				auto point_f = (*i).begin();
				std::wstring::size_type point_tr = point_f + (*i).rfind(L"である");
				std::wstring::size_type point_nl = point_tr + 3;
				std::wstring::size_type point_nr = point_f + (*i).rfind(L"を受け取る。");
				/*引数の登録*/
				ref_v.push_back(make_pair(wstring(point_f,point_tr),wstring(point_nl,point_nr));
				ref_count++;
			}
			/*返り値の判定*/
			if(regex_match(*i,output_l) && !regex_match(*i,output_nl))
			{
				/*返り値の型宣言だったときの処理*/
				auto point_l = (*i).begin();
				auto point_r = point_l + (*i).rfind(L"を返す。")
				type x(wstring(point_l,point_r));
				out_type = x;
				out_count++;
			}
			else if(regex_match(*i,output_l))
			{
				/*返り値だった場合の処理*/
				auto point_l = (*i).rfind(L"ここで") + 3;
				auto point_r = point_l + (*i).rfind(L"を返す。")
				auto vend = variable.end(),refend = ref_v.end();
				wstring tar(pintl,point_r);
				bool out_l = false;
				/*返り値が変数かどうか*/
				for(auto i = variables.begin();i != vend;i++)
				{
					if((*i).first = tar)
					{
						/*返り値が変数だった場合の処理*/
						if(out_type.type_name() != L"" && (*i).second != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type == L"")
						{
							out_type =  (*i).second;
						}
						out_l = true;
					}
				}
				/*返り値が引数かどうか*/
				for(auto i = ref_v.begin();i != refend;i++)
				{
					if((*i).first = tar)
					{
						/*引数だった場合の処理*/
						if(out_type.type_name() != L"" && (*i).second != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type == L"")
						{
							out_type =  (*i).second;
						}
						out_l = true;
					}		
				}
				/*定数かどうか*/
				if(out_type.type_name() != L"")
				{
					if(!out_l && !out_type.constred_type(tar))
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
				std::wstring::size_type point_tr = point_f + (*i).rfind(L"は");
				std::wstring::size_type point_nl = point_tr + 1;
				std::wstring::size_type point_nr = point_f + (*i).rfind(L"である。");
				/*変数の登録*/
				variables.push_back(make_pair(wstring(point_f,point_tr),wstring(point_nl,point_nr));	
			}
		}

		return true;
	}

	std::vector<fanc*> fanc::fancs;

	bool fanc::main_compile()
	{
		bool ans = true,baf_b = true;
		for(int i = 0;i < com_contents.size();i++)
		{
			cpp_contents.push_back(cpp_comp(com_contents[i],baf_b));
			ans = ans && baf_b;
		}
		return ans;
	}

	std::wstring cpp_comp(std::wstring input,bool& succeeded)
	{
		using namespace std;
		wstring ans;
		auto fe = fancs.end();
		for(auto i = fancs.begin();i != fe;i++)
		{
			if(regex_match(input,(*i)->com_low))
			{
				auto output_r = input.find("に");
				auto output_l = input.find("を")
				while(output_r != wstring::npos && baf_l < output_r)
				{
					output_l =
				}
			}
		}

		cerr << "条件に適合した動詞が見つかりませんでした。"
		succeeded = succeeded && false;
	}

}
