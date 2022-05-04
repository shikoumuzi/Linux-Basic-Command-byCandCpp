#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<glob.h>
#include<string.h>
#include<string>
#include<vector>
#include<sys/types.h>
#include<sys/wait.h>
#define DELIMS " \t\n"


static void prompt(void)
{
	printf("[Mshell-1 ]$ ");
}

struct commend
{
public:
	commend()
	{

	}
	~commend()
	{
		globfree(&(this->globres));
	}
public:
	glob_t globres;

};//采用结构体的形式方便拓展

static void parse(char *line, struct commend& cmd)
{
	char* tok = nullptr;
	int i = 0;

	while(true)
	{
		tok = strsep(&line, DELIMS);
		//这里的strsep表示按照某个分割符号，每次分割一小块并返回；
		if(tok == nullptr)
		{
			break;
		}
		
		if(tok[0] == '\0')
		{
			continue;
		}

		//这里用按位乘的形式，将GLOB——APPEND同0相乘，使得第一次循环中是没有追加模式的存在
		glob(tok, GLOB_NOCHECK | (GLOB_APPEND * i), NULL, &(cmd.globres));
		i = 1;//而后设为永真，使得追加模式启动
	}
}	

int main()
{
	while(true)
	{
		prompt();//打印出shell前缀
		
		char *linebuf = NULL;
		size_t linesize = 0;
		//getline函数第一个参数接收一个一级空指针的地址作出参
		//第二个参数用linesize作出参，第三个参数指定文件流
		
		if(getline(&linebuf, &linesize, stdin) == -1);
		{
			if(errno != 0)
			{	
				printf("getline() is failed errno: %d, %s, %ld\n", errno, linebuf,linesize);
			
				perror("getline(): ");
				break;
			}
		}
		struct commend cmd;

		parse(linebuf, cmd);
		
		if(false)//如果是内部命令
		{
	
		}
		else
		{

			pid_t ps = fork();
			//fork 函数的作用就是将父进程复制一份变成子进程
			//并且进程的执行位置相同，
			//相当于在此处程序分开允许
			if( ps < 0 )
			{
				perror("fork return an number less than 0");
				exit(1);
			}
			
			if( ps == 0)
			{	
				//exec函数族的作用就是将复制过来的子进程替换成所指定位置的进程
				execvp(cmd.globres.gl_pathv[0], cmd.globres.gl_pathv);
				perror("execvp faild");
				exit(1);
			}

			if( ps > 0 )
			{
				wait(NULL);
				//父进程等待子进程的结束，
				//而后回收进程资源	
			}
		}
	}


	return 0;
}
