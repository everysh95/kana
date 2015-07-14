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
		std::vector<std::wstring> output_com();
		std::vector<std::wstring> output_data();
//		bool do_command();
		bool precompile();
		bool main_compile();
		bool asm_comp(std::wstring,std::vector<std::wstring>&);
		static long find_id(std::wstring);
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
		static std::vector<fanc*> fancs;
	};

	/***********************
	* 演算子オブジェクト   *
	*--目的----------------*
	* 演算子を定義する。   *
	***********************/
//	class operator_fanc
//		:public fanc
//	{
//		public:
//		operator_fanc(int mode,type* output,type* right = nullptr,type* left = nullptr);
//		~operator_fanc();
//		static long find_from_type(type&,type&,int);
//		static long find_from_type(type&,type&,type&,int);
//		private:
//		type *right_type,*left_type,*output_type;
//		int mode;
//		static std::vector<operator_fanc*> operators;
//	};

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
		static bool if_begin(std::wstring,std::vector<std::wstring>&,std::vector<type::variable_type>);
		static bool loop_begin(std::wstring,std::vector<std::wstring>&,std::vector<type::variable_type>);
		static bool terms_end(std::wstring,std::vector<std::wstring>&);
		static std::wstring asm_num(std::wstring,bool&);
		static bool base_if(std::wstring,std::wstring&,std::vector<type::variable_type>);
	};


	//std::wstring filter_str(std::wstring);
	/*ワイド文字からASKIIへの変換*/
	//std::wstring filter_a(std::wstring);

}

#endif
