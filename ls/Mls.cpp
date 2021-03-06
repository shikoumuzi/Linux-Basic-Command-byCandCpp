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
#include<time.h>
#include<crypt.h>
#include<pwd.h>
#include<vector>
#include<algorithm>
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
		_exit(1);
		break;
	case GLOB_ABORTED:
		fprintf(stderr, "'GLOB_ABORTED' is touch, for a read error and........\n");
		throw std::string("GLOB_ABORTEDE");
		_exit(1);
		break;
	case GLOB_NOMATCH:
		fprintf(stderr, "'GLOB_NOMATH' is for no found matches\n");
		throw std::string("GLOB_NOMATH");
		_exit(1);
		break;
	default:
		fprintf(stderr, "unkown error\n");
		throw std::string("unkown error");
		_exit(1);
		break;
	}
}

glob_t getGlobdir(const char*path, bool getHideFile = true)
{
	glob_t globdir;
	//string的初始化需要const char*才能初始，char*则不行
	std::string pathname(path);

	//这里的 * 是linux的通配符
	//因为是需要在该文件夹下进行查找，所以必须加上 /
	pathname += "/*";
	GlobCheck(glob(pathname.c_str(), 0 , NULL, &globdir));
	//对 glob 返回值进行检验， 0表示正常返回
	
	if(getHideFile)
	{
		pathname = path;
		pathname += "/.*";
		//隐藏文件加上 .
		GlobCheck(glob(pathname.c_str(), GLOB_APPEND, NULL,&globdir));
	}
	
	return globdir;

}
static char*getmode(struct stat& file)
{
	static char mode[12];
	mode[11] = '\0';
	
	switch(file.st_mode & S_IFMT)
	{
	case S_IFBLK:  mode[0] = 'b';      break;
        case S_IFCHR:  mode[0] = 'c';      break;
        case S_IFDIR:  mode[0] = 'd';      break;
        case S_IFIFO:  mode[0] = 'i';      break;
        case S_IFLNK:  mode[0] = 'L';      break;
        case S_IFREG:  mode[0] = 'r';      break;
        case S_IFSOCK: mode[0] = 's';      break;
        default:       mode[0] = '-';      break;

	}
	
	struct stat& s_buff = file;
	mode[1] = (s_buff.st_mode & S_IRUSR)?'r':'-';
	mode[2] = (s_buff.st_mode & S_IWUSR)?'w':'-';
	mode[3] = (s_buff.st_mode & S_IXUSR)?'x':'-';
	mode[4] = (s_buff.st_mode & S_IRGRP)?'r':'-';
	mode[5] = (s_buff.st_mode & S_IWGRP)?'w':'-';
	mode[6] = (s_buff.st_mode & S_IXGRP)?'x':'-';
	mode[7] = (s_buff.st_mode & S_IROTH)?'r':'-';
	mode[8] = (s_buff.st_mode & S_IWOTH)?'w':'-';
	mode[9] = (s_buff.st_mode & S_IXOTH)?'x':'-';
	mode[10] = '\0';
	return mode;
}

void all(const char* path)
{
	std::cout<< path << std:: endl;
	
	glob_t globdir; 

	if(path != NULL)	
		globdir	= getGlobdir(path);
	else
		globdir = getGlobdir(".");


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

bool path_noloop(const std::string& pathname)
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

struct timestr
{
public:
	timestr(char* otherstr, time_t time)
	{
		this->str = otherstr;
		this->time = time;
	}
	~timestr()
	{
		delete(this->str);
		this->str = nullptr;
	}
public:
	char* str;
	time_t time;
};

bool timesortfun(timestr *a , timestr *b)//std::vector<timestr*>::iterator a, std::vector<timestr*>::iterator b)
{
	return a->time < b->time;
}

void list(const char* path, bool node = false, bool timesort = false)
{
	struct stat FileNoDir;
	
	glob_t globdir;
	if(path != NULL)
		 globdir = getGlobdir(path);
	else
		 globdir = getGlobdir(".");

	std::vector<struct timestr*> timeout;	
	

	char *p = nullptr;
	for(size_t i = 0; i < globdir.gl_pathc; ++i)
	{
		
	
		size_t len = strlen(globdir.gl_pathv[i]);
		p = (char*)malloc((len - 2) * sizeof(char));
		
		char *p_index = strrchr(globdir.gl_pathv[i], '/');		
		strncpy(p, p_index + 1 , strlen(p_index) - 1);
	
		if(lstat(globdir.gl_pathv[i],&FileNoDir) >= 0)
		{
			//这里是时间处理字段
			struct tm* atime, *ctime;
			atime = localtime(&(FileNoDir.st_atime));
			
			//必须要先声明足够容量的字符串倒入localtime才行
			char *atimestr = (char*)malloc(sizeof(char) * 50);
			//如果是-y的话 原生是从1900年开始计算的数字
			//-Y则是标准时间
			strftime(atimestr, 50, "%Y年%m月%d日%H时%M分%S秒", atime);

			char *ctimestr = (char*)malloc(sizeof(char) * 50);
			
			ctime = localtime(&(FileNoDir.st_ctime));
			strftime(ctimestr, 50 , "%Y年%m月%d日%H时%M分%S秒", ctime);
			
				//这里是用户信息处理字段
				struct passwd *user;
				user = getpwuid(FileNoDir.st_uid);
				//这里是通过uid来获取内容信息
				// struct passwd {
				//	       char   *pw_name;       /* username */
				//	       char   *pw_passwd;     /* user password */
				//	       uid_t   pw_uid;        /* user ID */
				//	       gid_t   pw_gid;        /* group ID */
				//	       char   *pw_gecos;      /* user information */
				//	       char   *pw_dir;        /* home directory */
				//	       char   *pw_shell;      /* shell program */
				//	   };

				//在这其中的passwd是加密过后的密码，
				//如果要判断密码是否正确需要用密码和私钥相或运算看是否与加密后的杂串相等
			char *mode = getmode(FileNoDir);				
			if(!timesort)
			{		
				std::cout 
					<< std::setw(14)
					<< std::setiosflags(std::ios::left)
					<< mode;
				
					
				if(node)	
				{
					std::cout  
						//设置输出左对齐
						<< std::setw(8)
						<< FileNoDir.st_gid << " " 
						<< std::setw(8)
						<< FileNoDir.st_uid << " ";
				}
				else
				{
					std::cout  
						//设置输出左对齐
						<< std::setw(15)
						<< user->pw_name << " " 
						<< std::setw(15)
						<< user->pw_name << " ";
				}
					
					std::cout
						<< std::setw(8)
						<< FileNoDir.st_size << " "
						<< std::setw(4)			
						<< FileNoDir.st_blocks << " "
						<< std::setw(30)
						<< atimestr  << " "
						<< std::setw(30)
						<< ctimestr << " "
						<< std::setw(20)
						<< p 
						<< std::endl;
					
			}
			else
			{
				char* time_str = new char[160];
				
				snprintf(time_str, 160, "%14s %15s %15s %8ld %30s %20s",
					       	mode, user->pw_name, user->pw_name, FileNoDir.st_size, ctimestr, p );
				
				timeout.push_back(new timestr(time_str, time_t(FileNoDir.st_ctime)));
				time_str = nullptr;			
			}	
			free(atimestr);
			atimestr = nullptr;
			free(ctimestr);
			ctimestr = nullptr;
			
		}
		else
		{
			std::cout<< globdir.gl_pathv[i] << " 打开失败" << std::endl;
		}
		
		
		free(p);
		p = nullptr;
	
		
	}
	std::sort(timeout.begin(),timeout.end(),timesortfun);
	for(size_t i = 0; i < timeout.size(); ++i)
	{
		std::cout<< timeout[i]->str << std::endl;
		delete timeout[i];
		timeout[i] = nullptr;
	}




	globfree(&globdir);
}

void R(const char*path)
{
	glob_t globres;
	std::string pathname;
	if(path == NULL)
	{	
		globres = getGlobdir(".");
		pathname = ".";
	}
	else
	{
		globres = getGlobdir(path,false);
		pathname = path;
	}

	struct stat statres;
	if(path != NULL)
	{	
		if(lstat(path, &statres) < 0)
		{
			perror("R() this path cannot open ");
			exit(1);
		}
		std::cout << pathname << std::endl;
	
		list(path);
	}
	else
	{
		if(lstat(".",&statres) < 0)
		{
			perror("R(): this path cannot open");
			exit(1);
		}
		std::cout << pathname << std::endl;
	
		list(".");
	}
	
	

	
	for(size_t i = 0; i < globres.gl_pathc; ++i)
	{
		if(lstat(globres.gl_pathv[i],&statres) < 0)
		{
			perror(" R() this path can not open");
			exit(1);
		}
		if(S_ISDIR(statres.st_mode) == true && path_noloop(globres.gl_pathv[i]) == true)
		{
			std::cout<<"\n" << std::endl;
			R(globres.gl_pathv[i]);

		}
	}
	globfree(&globres);
}

int main(int argc, char* argv[])
{
	while(1)
	{

		int opt = getopt(argc, argv, "-alrtAFRn");
		
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
			list(argv[optind],false,true);
			break;
		case 'A':
			break;
		case 'F':
			break;
		case 'R':
			R(argv[optind]);
			break;
		case 'n':
			list(argv[optind], true);
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
