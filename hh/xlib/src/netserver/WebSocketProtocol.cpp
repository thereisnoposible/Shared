#include "../include/netserver/WebSocketProtocol.h"
#include <string>
#include <vector>
#include "../include/helper/Helper.h"
#include <unordered_map>
#include <boost/format.hpp>
#include <boost/uuid/sha1.hpp>  
#include "base64.h"
#include "../include/json/json.h"
#include "../include/netserver/NetPack.h"

namespace xlib
{
	//-------------------------------------------------------------------------------------
	bool WebSocketProtocol::isWebSocketProtocol(const char* s, int rpos)
	{
		std::string data;

		data.assign(s, rpos);

		size_t fi = data.find_first_of("Sec-WebSocket-Key");
		if (fi == std::string::npos)
		{
			return false;
		}

		fi = data.find_first_of("GET");
		if (fi == std::string::npos)
		{
			return false;
		}

		std::vector<std::string> header_and_data;
		Helper::SplitStringHasEmpty(data, "\r\n\r\n", header_and_data);

		if (header_and_data.size() != 2)
		{
			return false;
		}
		return true;
	}

	//-------------------------------------------------------------------------------------
	bool WebSocketProtocol::handshake(std::shared_ptr<boost::asio::ip::tcp::socket>& pChannel, const char* s, int rpos)
	{
		std::string data;

		data.assign(s, rpos);

		std::vector<std::string> header_and_data;
		Helper::SplitStringHasEmpty(data, "\r\n\r\n", header_and_data);

		if (header_and_data.size() != 2)
		{
			return false;
		}

		std::unordered_map<std::string, std::string> headers;
		std::vector<std::string> values;

		Helper::SplitStringHasEmpty(header_and_data[0], "\r\n", values);
		std::vector<std::string>::iterator iter = values.begin();

		for (; iter != values.end(); ++iter)
		{
			header_and_data.clear();
			Helper::SplitStringHasEmpty((*iter), ": ", header_and_data);

			if (header_and_data.size() == 2)
				headers[header_and_data[0]] = header_and_data[1];
		}

		std::string szKey, szOrigin, szHost;

		std::unordered_map<std::string, std::string>::iterator findIter = headers.find("Sec-WebSocket-Origin");
		if (findIter == headers.end())
		{
			findIter = headers.find("Origin");
			if (findIter == headers.end())
			{
				//��Щapp���ͻ��˿���û������ֶ�
				//s->rpos(rpos);
				//s->wpos(wpos);
				//return false;
			}
		}

		if (findIter != headers.end())
			szOrigin = (boost::format("WebSocket-Origin: %1%\r\n") % findIter->second).str();

		findIter = headers.find("Sec-WebSocket-Key");
		if (findIter == headers.end())
		{
			return false;
		}

		szKey = findIter->second;

		findIter = headers.find("Host");
		if (findIter == headers.end())
		{
			return false;
		}

		szHost = findIter->second;


		std::string server_key = szKey;

		//RFC6544_MAGIC_KEY
		server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		boost::uuids::detail::sha1 sha;
		unsigned int message_digest[5];

		sha.reset();
		sha.process_bytes(server_key.c_str(), server_key.size());
		sha.get_digest(message_digest);

		for (int i = 0; i < 5; ++i)
			message_digest[i] = htonl(message_digest[i]);

		server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);

		std::string ackHandshake = (boost::format("HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %1%\r\n"
			"%2%"
			"WebSocket-Location: ws://%3%/WebManagerSocket\r\n"
			"WebSocket-Protocol: WebManagerSocket\r\n\r\n")
			% server_key %szOrigin %szHost).str();

		pChannel->send(boost::asio::buffer(ackHandshake));
		return true;
	}

	//-------------------------------------------------------------------------------------
	int WebSocketProtocol::makeFrame(NetPack* pInPacket, NetPack * pOutPacket, FrameType frame)
	{
		unsigned long long size = pInPacket->m_pBuff.size();

		// д��frame����
		pOutPacket->m_pBuff += frame;
		//char* p = const_cast<char*>(pOutPacket->m_pBuff.c_str());


		if (size <= 125)
		{
			pOutPacket->m_pBuff += ((unsigned char)size);
		}
		else if (size <= 65535)
		{
			unsigned char bytelength = 126;
			pOutPacket->m_pBuff += bytelength;

			pOutPacket->m_pBuff += ((unsigned char)((size >> 8) & 0xff));
			pOutPacket->m_pBuff += ((unsigned char)((size)& 0xff));
		}
		else
		{
			unsigned char bytelength = 127;
			pOutPacket->m_pBuff += bytelength;

			Helper::apply<unsigned long long>(&size);
			pOutPacket->m_pBuff.append((char*)&size, sizeof(size));
		}

		//pOutPacket->m_pBuff += base64_encode(reinterpret_cast<const unsigned char*>(pInPacket->m_pBuff.c_str()), size);
		pOutPacket->m_pBuff += pInPacket->m_pBuff;
		return pOutPacket->m_pBuff.size();
	}
	//
	//-------------------------------------------------------------------------------------
	//int WebSocketProtocol::getFrame(const char* pPacket, unsigned char& msg_opcode, unsigned char& msg_fin, unsigned char& msg_masked, 
	//	unsigned int& msg_mask, unsigned int& msg_length_field, unsigned long long& msg_payload_length, FrameType& frameType)
	//{
	//	/*
	//	0                   1                   2                   3
	//	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//	+-+-+-+-+-------+-+-------------+-------------------------------+
	//	|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
	//	|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
	//	|N|V|V|V|       |S|             |   (if payload len==126/127)   |
	//	| |1|2|3|       |K|             |                               |
	//	+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
	//	|     Extended payload length continued, if payload len == 127  |
	//	+ - - - - - - - - - - - - - - - +-------------------------------+
	//	|                               |Masking-key, if MASK set to 1  |
	//	+-------------------------------+-------------------------------+
	//	| Masking-key (continued)       |          Payload Data         |
	//	+-------------------------------- - - - - - - - - - - - - - - - +
	//	:                     Payload Data continued ...                :
	//	+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
	//	|                     Payload Data continued ...                |
	//	+---------------------------------------------------------------+
	//	*/
	//
	//	// ��һ���ֽ�, ���λ����������Ϣ�Ƿ����, ���4λ����������Ϣ����
	//	unsigned char bytedata;
	//	bytedata = *(pPacket);
	//
	//	msg_opcode = bytedata & 0x0F;
	//	msg_fin = (bytedata >> 7) & 0x01;
	//
	//	// �ڶ����ֽ�, ��Ϣ�ĵڶ����ֽ���Ҫ���������������Ϣ����, ���λ��0��1�������Ƿ������봦��
	//	(*pPacket) >> bytedata;
	//	msg_masked = (bytedata >> 7) & 0x01;
	//
	//	// ��Ϣ����
	//	msg_length_field = bytedata & (~0x80);
	//
	//	// ʣ�µĺ���7λ����������Ϣ����, ����7λ���ֻ������127�������ֵ������������
	//	// һ������Ϣ��������126�洢��Ϣ����, �����Ϣ��������UINT16�������ֵΪ126
	//	// ����Ϣ���ȴ���UINT16������´�ֵΪ127;
	//	// �������������Ϣ���ȴ洢����������byte[], �ֱ���UINT16(2λbyte)��UINT64(4λbyte)
	//	if (msg_length_field <= 125)
	//	{
	//		msg_payload_length = msg_length_field;
	//	}
	//	else if (msg_length_field == 126)
	//	{
	//		unsigned char bytedata1, bytedata2;
	//		(*pPacket) >> bytedata1 >> bytedata2;
	//		msg_payload_length = (bytedata1 << 8) | bytedata2;
	//	}
	//	else if (msg_length_field == 127)
	//	{
	//		msg_payload_length = ((uint64)(pPacket->data() + pPacket->rpos() + 0) << 56) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 1) << 48) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 2) << 40) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 3) << 32) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 4) << 24) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 5) << 16) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 6) << 8) |
	//			((uint64)(pPacket->data() + pPacket->rpos() + 7));
	//
	//		pPacket->read_skip(8);
	//	}
	//
	//	// ����ɶ����Ȳ���
	//	/* ���ﲻ����飬ֻ����Э��ͷ
	//	if(pPacket->length() < (size_t)msg_payload_length) {
	//	frameType = INCOMPLETE_FRAME;
	//	return (size_t)msg_payload_length - pPacket->length();
	//	}
	//	*/
	//
	//	// ����������������»�ȡ4�ֽ�����ֵ
	//	if (msg_masked)
	//	{
	//		(*pPacket) >> msg_mask;
	//	}
	//
	//	if (NETWORK_MESSAGE_MAX_SIZE < msg_payload_length)
	//	{
	//		WARNING_MSG(boost::format("WebSocketProtocol::getFrame: msglen exceeds the limit! msglen=({}), maxlen={}.\n",
	//			msg_payload_length, NETWORK_MESSAGE_MAX_SIZE));
	//
	//		frameType = ERROR_FRAME;
	//		return 0;
	//	}
	//
	//	if (msg_opcode == 0x0) frameType = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME; // continuation frame ?
	//	else if (msg_opcode == 0x1) frameType = (msg_fin) ? TEXT_FRAME : INCOMPLETE_TEXT_FRAME;
	//	else if (msg_opcode == 0x2) frameType = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME;
	//	else if (msg_opcode == 0x8) frameType = CLOSE_FRAME;
	//	else if (msg_opcode == 0x9) frameType = PING_FRAME;
	//	else if (msg_opcode == 0xA) frameType = PONG_FRAME;
	//	else frameType = ERROR_FRAME;
	//
	//	return 0;
	//}

	//-------------------------------------------------------------------------------------
	bool WebSocketProtocol::decodingDatas(const char* data, int len, NetPack* pPacket, unsigned char msg_masked, unsigned int msg_mask)
	{
		// ��������
		//if (len < sizeof(pPacket->m_Head))
		//	return false;
		std::string temp;
		temp.assign(data, len);
		if (!msg_masked)
			return false;

		std::string str;
		for (int i = 0; i < sizeof(pPacket->m_Head); ++i)
		{
			*((char*)(&pPacket->m_Head) + i) = *(data + i) ^ ((unsigned char*)(&msg_mask))[i % 4];
			str += *(data + i) ^ ((unsigned char*)(&msg_mask))[i % 4];
		}

		if (pPacket->m_Head.begflag != tagPackHead::PACK_HFLAG || pPacket->m_Head.endflag != tagPackHead::PACK_EFLAG)
		{
			return false;
		}

		for (int i = sizeof(pPacket->m_Head); i < len; ++i)
		{
			pPacket->m_pBuff += *(data + i) ^ ((unsigned char*)(&msg_mask))[i % 4];
		}
		

		return true;
	}

	//-------------------------------------------------------------------------------------
	bool WebSocketProtocol::decodingDatas(const char* data, int len, char* o_data, unsigned char msg_masked, unsigned int msg_mask)
	{
		if (msg_masked)
		{
			for (int i = 0; i < len; ++i)
			{
				*(o_data + i) = *(data + i) ^ ((unsigned char*)(&msg_mask))[i % 4];
			}
		}
		else
		{
			for (int i = 0; i < len; ++i)
			{
				*(o_data + i) = *(data + i);
			}
		}
		return true;
	}
}
