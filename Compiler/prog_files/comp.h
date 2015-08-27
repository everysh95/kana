#ifndef KANA_COMP_H
#define KANA_COMP_H

#include<string>
#include<stack>
#include<map>
#include<iostream>
#include<vector>
#include<regex>
#include<utility>
#include<algorithm>

namespace kana
{

	class type;

	/*********************
	* 変数オブジェクト   *
	*--目的--------------*
	* 変数の管理         *
	*********************/
	class variable_type
	{
		public:
		variable_type();
		variable_type(std::wstring,type*);
		virtual ~variable_type();
		long get_id();
		std::wstring get_name();
		type* get_type();
		static long size_sum();
		friend variable_type* is_variable(std::wstring,std::vector<variable_type*>);
		friend std::wstring convert_wstr(std::wstring,std::vector<variable_type*>);
		friend bool find_adress(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>&);
		protected:
		virtual std::wstring is_wstring();
		virtual bool is_adress(std::vector<std::wstring>&,std::vector<variable_type*>&);
		long id;
		static long ref_id;
		std::wstring v_name;
		type* ref_type;
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
		type(std::wstring,unsigned int size = 0);
		type(std::wstring,std::vector<std::wstring> castable_type,unsigned int size = 0);
		virtual ~type();

		/*----ゲッター----*/
		virtual std::wstring type_name();/*型の名前を取得*/
		virtual int get_size();
		virtual std::vector<std::wstring> define_type();

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
		unsigned int t_size;
		std::vector<type*> castable_types;
		std::vector<std::wstring*> consted;
		static std::vector<type*> type_target;
	};

	/*****************
	* コンパイル時   *
	* 設定           *
	* オブジェクト   *
	*--目的----------*
	* コンパイル時に *
	* 必要な情報を   *
	* 保持する。     *
	*****************/
	class comp_option
	{
		public:
		static void set_cpu(unsigned int);
		static unsigned int get_cpu();
		enum cpu:unsigned int
		{
			x86,
			x64
		};
		private:
		static unsigned int cpu_type;
	};
	//std::wstring filter_str(std::wstring);
	/*ワイド文字からASKIIへの変換*/
	//std::wstring filter_a(std::wstring);	

	/*****************
	* 配列           *
	* オブジェクト   *
	*--目的----------*
	* 配列を         *
	* 管理する。     *
	*****************/
	class vector_type
		:public variable_type
	{
		public:
			vector_type(std::wstring,unsigned int,type*);
			static std::vector<std::wstring> vector_output();
		protected:
			std::wstring is_wstring();
			static std::vector<std::wstring> vec_outs;
	};

	/***********************
	* 改行、タブフィルター *
	***********************/
	std::wstring del_nt(std::wstring);
	std::wstring del_comment(std::wstring);


}

#endif
