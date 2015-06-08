#ifndef KANA_COMP_H

#include<string>
#include<vector>
#include<regex>
#include<utility>
#include<algorithm>

namespace kana
{
	/*型クラス*/
	class type
	{
		public:
		typedef std::pair<std::wstring,std::wstring> const_type;
		typedef std::pair<std::wstring,std::wstring> variable_type;
		/*----コントラクタとデトラクタ----*/
		explicit type(std::wstring);
		type(std::wstring,std::vector<std::wstring> castable_type);
		virtual ~type();

		/*----ゲッター----*/
		std::wstring type_name();/*型の名前を取得*/

		/*----全操作----*/
		static bool type_match(type);/*型が存在するか判定*/
		bool consted_type(std::wstring);/*リテラルかどうか判定*/

		/*----演算子----*/
		bool operator==(const type& rhs)const;
		bool operator!=(const type& rhs)const
		{return !(*this == rhs);}
		operator type*(std::wstring);

		protected:
		std::wstring t_name;
		std::vector<type*> castable_types;
		std::vector<const_type*> consted;
		static std::vector<type*> type_target;
	}
	/***********************
	*関数オブジェクト      *
	*--目的----------------*
	***********************/

	class fanc
	:public type
	{
		public:
		fanc(std::wstring);
		std::wstring name();
		bool add_com(std::wsrting);
		bool do_com();
		bool in_comp();
		protected:
		std::wstring com_name;
		std::vector<std::wstring> com_cont;
		std::wstring com_low;
	};

	std::wstring fliter_str(std::wstring);
	std::string fliter_a(std::wstring);

}

#endif
