#include "class.h"

namespace kana
{
	struct_base::strect_base(std::wstring name)
		:type()
	{
		t_name = name;
		size = 0;
	}

	void add_menber(std::wstring name,type* r_ref_type)
	{
		menber_val[name] = r_ref_type;
	}

	void add_fanc(std::wstring name,fanc* rfanc)
	{
		if(rfanc == nullptr)
		{
			in_fanc_names.push_back(name);
		}
		else
		{
			menber_fanc[name] = rfanc;
		}
	}

	int get_size()
	{
		if(comp_option::get_cpu() == comp_option::cpu::x64)
			return menber_val.size() * 8;
		else if(comp_option::get_cpu() == comp_option::cpu::x86)
			return menber_val.size() * 4;
		else
			return -1;
	}

}
