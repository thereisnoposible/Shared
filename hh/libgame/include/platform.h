#pragma  once



#ifdef WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4661)
#pragma warning(disable:4275)
#pragma warning(disable:4251)
#pragma warning(disable:4819)
#endif


//#include <boost/interprocess/managed_shared_memory.hpp>
//#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/windows_shared_memory.hpp>

#include <boost/asio/error.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/bind.hpp>
//#include <boost/asio/basic_signal_set.hpp>
#include <boost/asio.hpp>
#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>


#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ctime>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>
#include <list>
#include <stdlib.h>


#include <stdio.h>
#ifdef _WIN32
#include <tchar.h>
#endif
#include "targetver.h"
#include "../lsge/lsge/utility/pfmholder.h"

#define		xset			std::set
#define		xlist			std::list
#define		xvector			std::vector
#define		xmap			std::map

#define		xhash_map		boost::unordered_map
#define		xhash_set		boost::unordered_set
typedef boost::thread			xthread;
#ifdef _WIN32		//for windows

#ifdef LIBGAME_EXPORTS
#define XClass _declspec(dllexport)
#else
#define XClass _declspec(dllimport)
#endif

#include <Windows.h>
#include <WinBase.h>
#include <hash_map>
#include <assert.h>
#include <io.h>
#include <tchar.h>
#include <stdio.h>

#include <direct.h>

#define __typename

typedef __int64					int64;
typedef unsigned long long		uint64;
typedef char					int8;
typedef unsigned char			uint8;
typedef short					int16;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef int						int32;
typedef std::stringstream		xstringstream;
typedef std::string				xstring;
typedef xvector<xstring>		StringArray;

///32位下为32位。64位下为64位一般用来在保存指针
typedef		unsigned long		ulong;

typedef     boost::interprocess::windows_shared_memory  shared_memory_object;
typedef		boost::uuids::uuid	UUID;
#else		// for linux

#include <hash_map>
#include <assert.h>
#include <stdio.h>
#include <ext/hash_map>
#include <unistd.h>
#include <time.h>
#include<iconv.h>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef void *PVOID;
typedef unsigned char UCHAR;
typedef unsigned long long		    uint64;
typedef		long long				int64;
typedef		char					int8;
typedef		unsigned char			uint8;
typedef		short					int16;
typedef		unsigned short			uint16;
typedef		unsigned int			uint32;
typedef		int						int32;
typedef std::stringstream xstringstream;
typedef		std::string				xstring;
typedef xvector<xstring> StringArray;

#define __typename typename
typedef void* HINSTANCE;

#define Sleep(a)   usleep(a*1000)
#define XClass

//typedef 	boost::interprocess::shared_memory_object  shared_memory_object;

#endif


#ifdef _WIN32
#ifndef snprintf
#	define snprintf		_snprintf
#endif
#include "WindowAPP.h"
#define  xApplication WindowAPP
#else

#include "LinuxApp.h"
#define sscanf_s sscanf
#define sprintf_s snprintf
#define  xApplication LinuxApp
#define closesocket close
#define	localtime_s(pTm, pSec)  localtime_r(pSec, pTm)
#endif



#define interface struct

