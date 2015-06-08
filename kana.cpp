#include <string>
#include <iostream>
#include <fstream>
#include <bitset>

enum flg
{
	str,
	cpflg,
	uflg,
	vflg,
	
	flg_nam
};

enum com
{
	Exit = 0,
	input,
	output,
	load,
	save,
	addv,
	data,
	mvu,
	mvv
};

int main(int argu,char** argv){

	std::ifstream inf(argv[1],std::ios_base::in|std::ios_base::binary);
	std::fstream memory(argv[2]);
	std::bitset<flg_nam> flgs;
	std::string ins;
	char in,inu,inv;
	while(inf)
	{
		inf.read(&in,sizeof(in));

		if(inf.eof()){break;}
		if(flgs[str] && in == '\0'){flgs.reset(str);}else
		if(flgs[str]){ins += in;}else
		if(flgs[uflg])
		{
			flgs.reset(uflg);
			flgs.reset(cpflg);
			inu = in;
		}else
		if(flgs[vflg])
		{
			flgs.reset(vflg);
			flgs.reset(cpflg);
			inv = in;
		}else
		if(flgs[cpflg] && in == 0){flgs.set(uflg);}else
		if(flgs[cpflg] && in == 1){flgs.set(vflg);}else
		if(in == Exit){return 0;}else
		if(in == cp){flgs.set(cpflg);}else
		if(in == add){inu += inv;}else
		if(in == output){std::cout << ins << std::flush;}else
		if(in == input){std::cin >> ins;}else
		if(in == data){flgs.set(str);}else
		if(in == load){memory >> ins;}else
		if(in == save){memory << ins << std::endl;}
	}
	
	return 0;
}
