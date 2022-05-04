Object = Mls.o\
#makefile 文件的变量
 
Mls: $(Object)
	g++ -std=c++17  -D_FILE_OFFSET_BITS=64 -D_XOPEN_SOURCE -lcrypt -o  Mls $(Object)\

#下面如果有依赖文件需要手动添加
clean :
	rm Mls $(Object)
