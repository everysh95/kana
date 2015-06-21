#ifndef KANA_COMP_H
#define KANA_COMP_H

#include<string>
#include<iostream>
#include<vector>
#include<regex>
#include<utility>
#include<algorithm>

namespace kana
{
	/*********************
	* 型クラス           *
	*--目的--------------*
	* 型を表現する。     *
	* 型系統のクラスは、 *
	* このクラスを       *
	* 継承する。         *
	*********************/
	class type
	{
		public:
		typedef std::pair<std::u32string,type*> variable_type;
		/*----コントラクタとデトラクタ----*/
		type();
		type(std::u32string);
		type(std::u32string,std::vector<std::u32string> castable_type);
		virtual ~type();

		/*----ゲッター----*/
		std::u32string type_name();/*型の名前を取得*/

		/*----全操作----*/
		static bool type_match(type);/*型が存在するか判定*/
		static type* find_from_wstr(std::u32string ref);/*ある名前の型を探す*/
		bool consted_type(std::u32string);/*リテラルかどうか判定*/

		/*----演算子----*/
		bool operator==(const type& rhs)const;
		bool operator!=(const type& rhs)const
		{return !(*this == rhs);}

		protected:
		std::u32string t_name;
		std::vector<type*> castable_types;
		std::vector<std::u32string*> consted;
		std::vector<variable_type> variables;/*変数判定用*/
		static std::vector<type*> type_target;
	};
	/***********************
	* 関数オブジェクト     *
	*--目的----------------*
	* 関数の管理           *
	*--メンバ--------------*
	* do_command           *
	* インタプリタで実行   *
	*----------------------*
	* precompile           *
	* プレコンパイル時に   *
	* 実行。               *
	*----------------------*
	* miain_compile        *
	* コンパイルの処理     *
	*----------------------*
	* cpp_comp             *
	* kans->C のコンパイル *
	* 処理                 *
	***********************/

	class fanc
	:public type
	{
		public:
		fanc(std::u32string);
		std::u32string name();
		bool add_com(std::u32string);
//		bool do_command();
		bool precompile();
		bool main_compile();
		std::u32string cpp_comp(std::u32string,std::vector<variable_type>&,bool&);
		protected:
		std::u32string com_name;
		/*もともとのコード*/
		std::vector<std::u32string> com_contents;
		/*c++に変換したもの*/
		std::vector<std::u32string> cpp_contents;
		std::u32string cpp_io;
		type* comp_o;
		std::vector<std::u32string> comp_i;
		/*適応規則*/
		std::u32string com_low;
		static std::vector<fanc*> fancs;
	};


	std::u32string filter_str(std::u32string);
	/*ワイド文字からASKIIへの変換*/
	std::string filter_a(std::u32string);

}

#endif
