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
		typedef std::pair<std::wstring,type*> variable_type;
		/*----コントラクタとデトラクタ----*/
		type();
		type(std::wstring);
		type(std::wstring,std::vector<std::wstring> castable_type);
		virtual ~type();

		/*----ゲッター----*/
		std::wstring type_name();/*型の名前を取得*/

		/*----全操作----*/
		static bool type_match(type);/*型が存在するか判定*/
		static type* find_from_wstr(std::wstring ref);/*ある名前の型を探す*/
		bool consted_type(std::wstring);/*リテラルかどうか判定*/

		/*----演算子----*/
		bool operator==(const type& rhs)const;
		bool operator!=(const type& rhs)const
		{return !(*this == rhs);}

		protected:
		std::wstring t_name;
		std::vector<type*> castable_types;
		std::vector<std::wstring*> consted;
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
		fanc(std::wstring);
		std::wstring name();
		bool add_com(std::wstring);
//		bool do_command();
		bool precompile();
		bool main_compile();
		std::wstring cpp_comp(std::wstring,std::vector<variable_type>&,bool&);
		protected:
		std::wstring com_name;
		/*もともとのコード*/
		std::vector<std::wstring> com_contents;
		/*c++に変換したもの*/
		std::vector<std::wstring> cpp_contents;
		std::wstring cpp_io;
		type* comp_o;
		std::vector<std::wstring> comp_i;
		/*適応規則*/
		std::wstring com_low;
		static std::vector<fanc*> fancs;
	};


	std::wstring filter_str(std::wstring);
	/*ワイド文字からASKIIへの変換*/
	std::string filter_a(std::wstring);

}

#endif
