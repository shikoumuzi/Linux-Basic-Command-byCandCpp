#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<cstring>
#include<string.h>
#include<fstream>
#include<sys/types.h>
#include<sys/stat.h>
#include<glob.h>
#include<fstream>
#include<iomanip>

void Hook(void)
{
	//钩子函数 用atexit钩上 会在程序结束时被调用，在这里相当于c++的析构函数
}

int GlobErrFunc(const char*epath, int errno)
{
	std::cout<< epath <<" is worring, error number is "<< errno << std::endl;
	_exit(errno);
}

void GlobCheck(int checknumber)
{
	switch(checknumber)
	{
	case 0:
		break;
	case GLOB_NOSPACE:
		fprintf(stderr, "'GLOB_NOSPACE' is touch, for running out of memory\n");
		throw std::string("GLOB_NOSPACE");
		break;
	case GLOB_ABORTED:
		fprintf(stderr, "'GLOB_ABORTED' is touch, for a read error and........\n");
		throw std::string("GLOB_ABORTEDE");
		break;
	case GLOB_NOMATCH:
		fprintf(stderr, "'GLOB_NOMATH' is for no found matches\n");
		throw std::string("GLOB_NOMATH");
		break;
	default:
		fprintf(stderr, "unkown error\n");
		throw std::string("unkown error");
		break;
	}
}

void all(const char* path)
{
	std::cout<< path << std:: endl;
	
	//string的初始化需要const char*才能初始，char*则不行
	std::string pathname(path);
	pathname += "/*";
	//这里的 * 是linux的通配符
	//因为是需要在该文件夹下进行查找，所以必须加上 /
	
	glob_t globdir;
	int globcheck = glob(pathname.c_str(), 0, NULL, &globdir);
	//对 glob 返回值进行检验， 0表示正常返回
	
	GlobCheck(globcheck);
	
	pathname = pathname.substr(0, pathname.size()-3);
	pathname += "/.";
	
	globcheck = glob(pathname.c_str(),GLOB_APPEND,NULL,&globdir);
	GlobCheck(globcheck);

	char* p = NULL;

	for(size_t i = 0; i< globdir.gl_pathc; ++i)
	{	
		size_t len = strlen(globdir.gl_pathv[i]);
		p = (char*)malloc((len - 2) * sizeof(char));

		strncpy(p, globdir.gl_pathv[i] + 2, len - 2);

		std::cout<< p  <<" ";
		free(p);
		p = NULL;
	}

	std::cout<<std::endl;
	globfree(&globdir);
}

void list(const char* path, bool node = false)
{
	std::string pathname(path);
	pathname += "/*";

	struct stat FileNoDir;
	glob_t globdir;
	int globcheck = glob(pathname.c_str(), 0, NULL, &globdir);
	GlobCheck(globcheck);

	char *p = nullptr;
	for(size_t i = 0; i < globdir.gl_pathc; ++i)
	{
		if(node == false)
		{
			size_t len = strlen(globdir.gl_pathv[i]);
			p = (char*)malloc((len - 2) * sizeof(char));
									
			strncpy(p, globdir.gl_pathv[i] + 2 , len - 2);
		
			if(lstat(globdir.gl_pathv[i],&FileNoDir) >= 0)
			{
				
				std::cout << std::setiosflags(std::ios::left) 
					//设置输出左对齐
					<< std::setw(8)
				       	<< FileNoDir.st_gid << " " 
					<< std::setw(8)
					<< FileNoDir.st_uid << " "
					<< std::setw(8)
					<< FileNoDir.st_size << " "
					<< std::setw(8)			
					<< FileNoDir.st_blocks << " "
					<< std::setw(20)
					<< p 
					<< std::endl;
			}
			else
			{
				std::cout<< globdir.gl_pathv[i] << " 打开失败" << std::endl;
			}
			free(p);
			p = nullptr;

		}
		
	}
	


	globfree(&globdir);
}

int main(int argc, char* argv[])
{
	while(1)
	{

		int opt = getopt(argc, argv, "-alrtAFR");
		
		//当读取完全部的命令时，getopt()会返回-1，这里用<0来表示循环终点
		if(opt < 0)
		{		
			break;
		}
		switch(opt)
		{
		case 'a':
			all(argv[optind]);		
			break;
		case 'l':
			list(argv[optind]);	
			//optind记录的是下一个检索的位置，将其减一得到当前的位置
			break;
		case 'r':
			break;
		case 't':
			break;
		case 'A':
			break;
		case 'F':
			break;
		case 'R':
			break;
		default:
			//当查找不到时，getopt会返回‘?’
			fprintf(stderr,"%s :it has no option in commend \n",__FUNCTION__);
			//__FUNCTION__是gcc所提供的查看当前运行函数的宏
			
	
			break;
		}
	
	
		opt = getopt(argc, argv, "-alrtAFR");
		
		//当读取完全部的命令时，getopt()会返回-1，这里用<0来表示循环终点
		if(opt < 0)
		{
			break;
		}
		switch(opt)
		{
		case 'a':
			break;
		case 'l':

			break;
		case 'r':
			break;
		case 't':
			break;
		case 'A':
			break;
		case 'F':
			break;
		case 'R':
			break;
		default:
			//当查找不到时，getopt会返回‘?’
		
			fprintf(stderr,"%s :it has no option in commend",__FUNCTION__);
			//__FUNCTION__是gcc所提供的查看当前运行函数的宏
			break;
		}
	
	}
	
	

	


	atexit(Hook);//挂上钩子， 在正常结束程序时被调用


	exit(0);//这里的状态值会返回给父进程
}
