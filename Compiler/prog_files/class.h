#ifndef KANA_STRECT_TYPE
#define KANA_STRECT_TYPE

#include "comp.h"

namespace kana
{

	class struct_type;

	/***************
	* 構造体       *
	* オブジェクト *
	*--目的--------*
	* 構造体を     *
	* 管理する。   *
	***************/
	class struct_base
		:public type
	{
		public:
			struct_base(std::wstring name);
			void add_menber(std::wstring,type*);
			void add_fanc(std::wstring,fanc* = nullptr);
			int get_val(std::wstring name,struct_type*);
			int get_size();
		private:
			std::map<std::wstring,type*> menber_val;
			std::map<std::wstring,fanc*> menber_fanc;
			std::vector<std::wstring> in_fanc_names;
	};

}

#endif
