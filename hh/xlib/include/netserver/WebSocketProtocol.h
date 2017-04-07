#pragma once
#include <string>
#include <memory>
#include <boost/asio.hpp>

namespace xlib
{
	class NetPack;
	class WebSocketProtocol
	{
	public:
		enum FrameType
		{
			// 下一帧与结束
			NEXT_FRAME = 0x0,
			END_FRAME = 0x80,

			ERROR_FRAME = 0xFF00,
			INCOMPLETE_FRAME = 0xFE00,

			OPENING_FRAME = 0x3300,
			CLOSING_FRAME = 0x3400,

			// 未完成的帧
			INCOMPLETE_TEXT_FRAME = 0x01,
			INCOMPLETE_BINARY_FRAME = 0x02,

			// 文本帧与二进制帧
			TEXT_FRAME = 0x81,
			BINARY_FRAME = 0x82,

			PING_FRAME = 0x19,
			PONG_FRAME = 0x1A,

			// 关闭连接
			CLOSE_FRAME = 0x08
		};

		struct WebSocketHead
		{
			//#ifdef BIG_ENDIAN
			//unsigned char fin : 1;				//FIN
			//unsigned char RSV1: 1;				//
			//unsigned char RSV2 : 1;				//
			//unsigned char RSV3 : 1;				//
			//unsigned char opcode : 4;			//opcode

			//unsigned char mask : 1;				//MASK	
			//unsigned char payloadlen : 7;		//payload len


			//#else
			unsigned char opcode : 4;			//opcode
			unsigned char RSV3 : 1;				//
			unsigned char RSV2 : 1;				//
			unsigned char RSV1 : 1;				//
			unsigned char fin : 1;				//FIN

			unsigned char payloadlen : 7;		//payload len
			unsigned char mask : 1;				//MASK	

			//#endif

		};

		struct WebSocketPack
		{
			WebSocketPack()
			{
				datalen = 0;
			}
			WebSocketHead head;
			union
			{
				unsigned long long datalen;
				struct DataLen
				{
#ifdef BIG_ENDIAN
					unsigned int long_high;
					unsigned short high;
					unsigned short low;
#else
					unsigned short low;
					unsigned short high;
					unsigned int long_high;
#endif
				} high_low;
			};
			unsigned int mask_key;
			std::string data;
		};

		/**
		是否是websocket协议
		*/
		static bool isWebSocketProtocol(const char* s, int rpos);

		/**
		websocket协议握手
		*/
		static bool handshake(std::shared_ptr<boost::asio::ip::tcp::socket>& pChannel, const char* s, int rpos);

		///**
		//帧解析相关
		//*/
		static int makeFrame(NetPack* pInPacket, NetPack * pOutPacket, FrameType frame);
		/*static int getFrame(const char* pPacket, WebSocketHead& head, FrameType& frameType);*/

		static bool decodingDatas(const char* data, int len, NetPack* pPacket, unsigned char msg_masked, unsigned int msg_mask);
		static bool decodingDatas(const char* data, int len, char* o_data, unsigned char msg_masked, unsigned int msg_mask);

	};

}

