#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<cstring>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<glob.h>

static int64_t  Mdu(const char *pathname)
{
	struct stat statres;
	glob_t globres;

	if(lstat(pathname, &statres) < 0)
	{
		perror("lstat() is woring");
		exit(1);
	}
	//path为非目录文件
	
	if(!S_ISDIR(statres.st_mode))
	{
		return statres.st_blocks/2;
	}
	//path为目录文件
	std::string nextpath(pathname);
	nextpath += "/*";//所有未隐藏的文件
	//glob 解析模式
	//可以替代
	//opendir()
	//closedir()
	//readdir(3)
	//seekdir()
	//telldir()
	//需要搭配globfree
	
	glob(nextpath.c_str(), 0, NULL, &globres);
	//这里需要写校验
	
	nextpath = pathname;
	nextpath += "/.*";//所有隐藏文件
	glob(nextpath.c_str(), GLOB_APPEND, NULL, &globres);
	//GLOB——APPEND追加
	
	int64_t  sum =0;
	for(int i = 0; i < globres.gl_pathc; ++i)//pathc类似于argc
		sum += Mdu(globres.gl_pathv[i]);//pathv类似于arg

	return sum/2;
	//block的值是大小的两倍
}

int main(int argc, char*argv[])
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage commnd....\n");
		exit(0);
	}
	for(int i = 1; i < argc; ++i)
	{
		std::cout<<argv[i] <<" is "<<Mdu(argv[i]) <<std:: endl;
	}
	exit(0);
}
