#include "kana_comp.h"

namespace kana
{
	std::vector<type*> type::type_target;

	type::type()
	{
		type_target.push_back(this);
	}

	type::type(std::u32string t_name)
		:t_name(t_name)
	{
		type_target.push_back(this);
	}


	type::type(std::u32string t_name,std::vector<std::u32string> castable_type)
		:t_name(t_name)
	{
		type_target.push_back(this);
	}

	type::~type()
	{
		auto target_end = std::remove(type_target.begin(),type_target.end(),this);
		type_target.erase(target_end,type_target.end());
	}

	std::u32string type::type_name()
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

	bool type::consted_type(std::u32string target)
	{
		bool ans = false;
		auto cend = consted.end();
		for(auto i = consted.begin();i != cend;i++)
		{
			std::basic_regex<char32_t> ref((*i)->c_str());
			ans = ans || regex_match(target,ref);
		}
		return ans;
	}

	type* type::find_from_wstr(std::u32string target)
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

	fanc::fanc(std::u32string name)
		:com_name(name)
	{}

	bool fanc::add_com(std::u32string target)
	{
		std::u32string input_com = filter_str(target);
		com_contents.push_back(input_com);
		return true;
	}

	bool fanc::precompile()
	{
		using namespace std;
		com_low = U"";

		basic_regex<char32_t> 
		input_l(U"*である*を受け取る。",std::regex_constants::basic),
		output_l(U"*を返す。",std::regex_constants::basic),
		output_nl(U"ここで*を返す。",std::regex_constants::basic),
		variable_l(U"*は*である。",std::regex_constants::basic);

		wsmatch result_mws;
		vector<variable_type> 
		variables,/*変数判定用*/
		ref_v;/*引数判定用*/
		type out_type(U"");

		int ref_count = 0,out_count = 0;
		for(auto i = com_contents.begin();i != com_contents.end();i++)
		{
			/*引数の宣言であるかどうかの判定*/
			if(regex_match(*i,input_l))
			{
				/*--引数の宣言であった時の処理--*/
				/*対象の型と名前の特定*/
				auto point_f = (*i).begin();
				auto point_tr = point_f + (*i).rfind(U"である");
				auto point_nl = point_tr + 3;
				auto point_nr = point_f + (*i).rfind(U"を受け取る。");
				/*引数の登録*/
				ref_v.push_back(make_pair(u32string(point_f,point_tr),type::find_from_wstr(u32string(point_nl,point_nr))));
				ref_count++;
			}
			/*返り値の判定*/
			if(regex_match(*i,output_l) && !regex_match(*i,output_nl))
			{
				/*返り値の型宣言だったときの処理*/
				auto point_l = (*i).begin();
				auto point_r = point_l + (*i).rfind(U"を返す。");
				type x(u32string(point_l,point_r));
				out_type = x;
				out_count++;
			}
			else if(regex_match(*i,output_l))
			{
				/*返り値だった場合の処理*/
				auto point_l = (*i).rfind(U"ここで") + 3;
				auto point_r = point_l + (*i).rfind(U"を返す。");
				auto vend = variables.end();
				auto refend = ref_v.end();
				u32string tar(point_l,point_r);
				bool out_l = false;
				/*返り値が変数かどうか*/
				for(auto j = variables.begin();j != vend;j++)
				{
					if((*j).first == tar)
					{
						/*返り値が変数だった場合の処理*/
						if(out_type.type_name() != U"" && *((*j).second) != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type.type_name() == U"")
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
						if(out_type.type_name() != U"" && *((*j).second) != out_type)
						{
							cerr << "返り値の型が不正です。" << endl;
							return false;
						}
						else if(out_type.type_name() == U"")
						{
							out_type = *((*j).second);
						}
						out_l = true;
					}		
				}
				/*定数かどうか*/
				if(out_type.type_name() != U"")
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
				auto point_tr = point_f + (*i).rfind(U"は");
				auto point_nl = point_tr + 1;
				auto point_nr = point_f + (*i).rfind(U"である。");
				/*変数の登録*/
				variables.push_back(make_pair(u32string(point_f,point_tr),type::find_from_wstr(u32string(point_nl,point_nr))));	
			}
		}

		return true;
	}

	std::vector<fanc*> fanc::fancs;

	bool fanc::main_compile()
	{
		bool ans = true,baf_b = false,baf_b2 = true;
		auto fe = fancs.end();
		std::vector<variable_type> variables;/*変数判定用*/
		cpp_contents.clear();
		for(int i = 0;i < com_contents.size();i++)
		{
			baf_b = false;
			for(auto j = fancs.begin();j != fe;j++)
			{
				cpp_contents.push_back((*j)->cpp_comp(com_contents[i],variables,baf_b2));
				baf_b = baf_b || baf_b2;
			}
			ans = ans && baf_b;
		}
		return ans;
	}

	std::u32string fanc::cpp_comp(std::u32string input,std::vector<variable_type>& variables,bool& succeeded)
	{
		using namespace std;
		u32string ans;
		basic_regex<char32_t> tar_o(com_low.c_str());
		if(regex_match(input,tar_o))
		{
			/*返り値判定*/
			auto output_l = input.begin();
			auto output_r = input.begin() + input.find(U"に");
			u32string baf(output_l,output_r);
			auto ve = variables.end();
			bool v_flg = false;
			for(auto vi = variables.begin();vi != ve;vi++)
			{
				if((*vi).first == baf && (*vi).second == comp_o)
				{
					v_flg = true;
					ans = /*filter_a*/(baf) + U" = ";
				}
			}
			if(v_flg)
			{
				/*引数判定*/
				ans += cpp_io + U"(";
				basic_regex<char32_t> in_t(U"([^を])を");
				basic_regex_token_iterator<u32string> obj_o(begin(input),end(input),in_t,{1});
				basic_regex_token_iterator<u32string> last;
				while(obj_o != last)
				{
					ans += /*filter_a*/(obj_o->str());
					++obj_o;
					if(obj_o != last)
					ans += U",";
				}
				ans += U");";
			}
		}

		if(ans.empty())
		{
			succeeded = succeeded && false;
		}

		return ans;
	}
	
	std::string filter_a(std::u32string wstr)
	{
		int wsize = sizeof(wstr[0]);
		std::string ans;
		char baf;
		/*4bitで表せる最大値*/
		for(int i = 0;i < wstr.size();i++)
		{
			for(int j = 0;j < wsize * 2;j++)
			{
				/*4bitずつに区切る*/
				baf = wstr[i] & ( 0xF << (j*4)) + 'a';
				/*ansに付け加える*/
				ans += baf;
			}
		}
		return ans;
	}

	std::u32string filter_str(std::u32string input)
	{
		return input;
	}

}

