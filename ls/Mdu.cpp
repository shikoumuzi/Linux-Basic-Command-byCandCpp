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
#include<iterator>

//如果一个变量在递归点之前或者之后使用， 完全可以放在静态区使用， 以减少创建变量的过程和耗时

static bool path_noloop(const std::string& pathname)
{
	//用以检验文件夹路径/后面的. 
	//当为..时为父节点的内容，进而会造成循环递归
	size_t index = 0;
	if((index = pathname.rfind('/')) != pathname.npos)
	{
		std::string temp = pathname.substr(index);		
		if(temp == "/." || temp == "/..")
		{
			return false;
		}
		return true;
	}
	exit(1);
}


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
		return statres.st_blocks;
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
	//是要在前面已经创建过了的情况下才能追加
		
	int64_t  sum =0;
	//通过递归将叶子节点获取
	for(int i = 0; i < globres.gl_pathc; ++i)//pathc类似于argc
	{	if(path_noloop(globres.gl_pathv[i]))
			{
				sum += Mdu(globres.gl_pathv[i]);//pathv类似于arg
			}
	}

	sum += statres.st_blocks;
	//加上自身的blocks数目


	globfree(&globres);
	return sum;
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
		//block的值是大小的两倍
		std::cout<<argv[i] <<" is "<<Mdu(argv[i]) / 2 <<std:: endl;
	}
	exit(0);
}
