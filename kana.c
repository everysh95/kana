#include<stdio.h>
#include<stdlib.h>

/*************************************
* kanaヴァーチャルマシンソースコード *
* 数値対応表                         *
* 0:プログラム終了                   *
* 1:セーブ                           *
* 2:ロード                           *
* 3:入力                             *
* 4:出力                             *
* 5:命令追加                         *
*************************************/
enum 
{
	EXIT = 0,
	SAVE = 1,
	LOAD = 2,
	INPT = 3,
	OUTP = 4,
	ININ = 5
};

/************************
* 参照ファイルポインタ  *
* indef:基本プログラム  *
* memory:メインメモリー *
* inf:命令用メモリー    *
* ink:補助用ポインタ    *
************************/

FILE *indef,*memory,*inf,*ink;
	
void Load(char*,FILE*);
void Save(char*,FILE*);
void cmd(char,char,char*);

int main(int agru,char** agrv)
{
	/*******************
	* com:コマンド     *
	* inu:第一引数     *
	* inv:第二引数     *
	* ins:文字列用引数 *
	*******************/
	char com,in;
	char* ins;

	/*----ここからファイルポインタの初期化----*/
	indef = fopen(agrv[1],"rb");
	memory = fopen(".MainMemory.memo","r+b");
	if(memory == NULL)
	{
		memory = fopen(".MainMemory.memo","w+b");
	}
	inf = fopen(".commander.memo","w+b");
	/*----ここまでファイルポインタの初期化----*/

	Load(&com,indef);
	while(com != EOF)
	{
		Save(&com,inf);
		Load(&com,indef);
	}
	fclose(indef);

	Load(&com,inf);
	while(com != EOF && in != EOF)
	{
		}
	
	return 0;
}

void Load(char* ans,FILE* in)
{
	fread(ans,sizeof(char),1,in);
}

void Save(char* ans,FILE* in)
{
	fwrite(ans,sizeof(char),1,in);
}

void cmd(char com,char in,char* ins)
{
	switch(com)
	{
		case EXIT:
		fclose(memory);
		fclose(inf);
		exit(0);
		break;
		case SAVE:
	}
}
