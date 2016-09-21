#include "NetService.h"

#include "google/protobuf/stubs/common.h"

#ifdef _DEBUG
#pragma comment(lib,"../lib/libgame_d.lib")
#pragma comment(lib,"../lib/libprotobuf_d.lib")
#else
#pragma comment(lib,"../lib/libgame.lib")
#pragma comment(lib,"../lib/libprotobuf.lib")
#endif // DEBUG

#include "../new/proto/protobuf/hello.pb.h"

class A
{
public:
	void regist()
	{
		NetService::getInstance().RegisterMessage(808464432, boost::bind(&A::hello, this, _1));
	}
	void hello(PackPtr nPack){
		std::cout << "hello"; 
		pm_hello hell;
		hell.set_dbid(23333);
		NetService::getInstance().SendPack(1234, hell, "123", 0);
	}
protected:
private:
};

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	NetService* p = new NetService(10084,4);
	A a;
	a.regist();
	NetService::getInstance().run();

	delete p;
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}