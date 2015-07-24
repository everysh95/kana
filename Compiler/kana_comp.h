#ifndef KANA_COMP_H
#define KANA_COMP_H

#include<string>
#include<stack>
#include<iostream>
#include<vector>
#include<regex>
#include<utility>
#include<algorithm>

namespace kana
{

	class type;

	/**********************
	 * 変数オブジェクト   *
	 *--目的--------------*
	 * 変数の管理         *
	 *********************/
	class variable_type
	{
		public:
		variable_type();
		variable_type(std::wstring,type*);
		long get_id();
		std::wstring get_name();
		type* get_type();
		friend variable_type is_variable(std::wstring,std::vector<variable_type>);
		private:
		std::wstring v_name;
		type* ref_type;
		long id;
		static long ref_id;
	};

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
	/*----コントラクタとデトラクタ----*/
		type();
		type(std::wstring);
		type(std::wstring,std::vector<std::wstring> castable_type);
		virtual ~type();

		/*----ゲッター----*/
		std::wstring type_name();/*型の名前を取得*/
		virtual std::vector<std::wstring> define_type(){return std::vector<std::wstring>();}

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
	* asm_comp             *
	* kansから             *
	* アセンブラへの       *
	* コンパイル処理       *
	***********************/

	class fanc
	:public type
	{
		public:
		fanc(std::wstring);
		std::wstring name();
		bool add_com(std::wstring);
		std::vector<std::wstring> output_com();
		std::vector<std::wstring> output_data();
//		bool do_command();
		bool precompile();
		bool main_compile();
		virtual bool asm_comp(std::wstring,std::vector<std::wstring>&,std::vector<variable_type>&);
		static long find_id(std::wstring);
		friend bool filter_com(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type>& ref);
		protected:
		std::wstring com_name;
		long com_id;
		/*もともとのコード*/
		std::vector<std::wstring> com_contents;
		/*アセンブラに変換したもの*/
		std::vector<std::wstring> asm_command;
		std::vector<std::wstring> asm_data;
		/*適応規則*/
		std::wstring com_low;
		std::vector<type*> input_types;
		type* output_type;
		/*定型文かどうか*/
		bool const_flg;
		/*変換用*/
		static bool asm_create(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type>& ref);
		static std::vector<fanc*> fancs;
	};

	/***************
	* 基本構文     *
	* オブジェクト *
	*--目的--------*
	* 基本的な処理 *
	* の変換の     *
	* 実行。       *
	***************/
	class base_com
	{
		public:
		static long if_counter,loop_counter;
		static std::stack<std::wstring> terms_stack,nstack;

		static bool inline_asm(std::wstring,std::vector<std::wstring>&);
		static bool if_begin(std::wstring,std::vector<std::wstring>&,std::vector<variable_type>);
		static bool loop_begin(std::wstring,std::vector<std::wstring>&,std::vector<variable_type>);
		static bool terms_end(std::wstring,std::vector<std::wstring>&,std::vector<variable_type>);
		static std::wstring asm_num(std::wstring,bool&);
		static bool base_if(std::wstring,std::vector<std::wstring>&,std::vector<variable_type>);
	};


	//std::wstring filter_str(std::wstring);
	/*ワイド文字からASKIIへの変換*/
	//std::wstring filter_a(std::wstring);

}

#endif
