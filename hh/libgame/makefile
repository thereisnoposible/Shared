#EXECUTABLE 可执行文件
#PROJECTDIR 项目目录
#LIBDIR 链接库目录 用空格隔开
#LIBS 链接库 lib 和 .os 不用写
#INCLUDES include文件路径
#SRCDIR 源码路径
#CFLAGS 编译选项
#LFLAGS 链接选项
EXECUTABLE = libgame.so
PROJECTDIR = ./libgame
LIBS = pthread dl
CFLAGS = -fPIC -fpermissive -fexceptions -shared
LFLAGS = -shared
SRCDIR = $(PROJECTDIR) ./libgame/include/lua ./depend/json ./libgame/source
include makefile.main
