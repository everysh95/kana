#ifndef KANA_FANC
#define KANA_FANC

#include <queue>
#include "comp.h"
//#include "class.h"

namespace kana
{


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
	:public variable_type
	{
		public:
		fanc(std::wstring);
		~fanc();
		std::wstring name();
		bool add_com(std::wstring);
		std::vector<std::wstring> output_com();
		std::vector<std::wstring> output_data();
//		bool do_command();
		bool precompile();
		bool main_compile();
		virtual bool asm_comp(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>&);
		static long find_id(std::wstring);
		friend bool filter_com(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type*>& ref);
		friend fanc* find_fanc(std::wstring);
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
		/*コンパイル後かどうか*/
		bool is_after_comp;
		std::vector<variable_type*> variables;/*変数判定用*/
		std::vector<variable_type*> ref_variables;/*引数判定用*/
		static bool asm_create(std::wstring wstr,std::vector<std::wstring>& target,std::vector<variable_type*>& ref);
		static std::vector<fanc*> fancs;
		static fanc* now_main;
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

		static bool inline_asm(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>);
		static bool if_begin(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>);
		static bool loop_begin(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>);
		static bool terms_end(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>);
		static std::wstring asm_num(std::wstring,bool&);
		static bool base_if(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>);
	};


	/********************
	* 基本関数          *
	* オブジェクト      *
	*--目的-------------*
	* 基本的な関数を    *
	* 定義する。        *
	********************/
	class predefine
	{
		public:
		static bool asm_comp(std::wstring,std::vector<std::wstring>&,std::vector<variable_type*>&);
		private:
		static std::vector<variable_type*> skips;
	};


}

#endif
