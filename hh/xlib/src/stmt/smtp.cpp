/*
 * 2013-9-10:build by ken <http://kenchowcn.com>

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>

#include <time.h>
#include <sys/types.h>
#ifdef _WIN32

#include <ws2tcpip.h>
#include <mswsock.h>
#include <iphlpapi.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>

#else

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#endif
#include <time.h>
#include <errno.h>
#include "../include/stmt/smtp.h"
#pragma comment(lib,"ws2_32.lib")

#ifdef _WIN32
#ifndef snprintf
#	define snprintf		_snprintf
#endif
#endif


#define NONEED_SELECT     1


#define SMTP_PORT         25
#define SMTP_MTU          800
#define MAX_EMAIL_LEN     64
#define TEXT_BOUNDARY     "KkK170891tpbkKk__FV_KKKkkkjjwq"

#define BASE64_SIZE(x)  (((x)+2) / 3 * 4 + 1)

#define SIZEOF_ARRAY(arr)        (sizeof(arr) / sizeof(arr[0]))

namespace xlib
{

	/* ---------------- private code */
	static const unsigned char map2[] =
	{
		0x3e, 0xff, 0xff, 0xff, 0x3f, 0x34, 0x35, 0x36,
		0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01,
		0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
		0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0x1b,
		0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
		0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
		0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33
	};

	int base64_decode(unsigned char *out, const char *in, int out_size)
	{
		int i, v;
		unsigned char *dst = out;

		v = 0;
		for (i = 0; in[i] && in[i] != '='; i++) {
			unsigned int index = in[i] - 43;
			if (index >= SIZEOF_ARRAY(map2) || map2[index] == 0xff)
				return -1;
			v = (v << 6) + map2[index];
			if (i & 3) {
				if (dst - out < out_size) {
					*dst++ = v >> (6 - 2 * (i & 3));
				}
			}
		}

		return dst - out;
	}

	/*****************************************************************************
	* b64_encode: Stolen from VLC's http.c.
	* Simplified by Michael.
	* Fixed edge cases and made it work from data (vs. strings) by Ryan.
	*****************************************************************************/

	char *base64_encode(char *out, int out_size, const char *in, int in_size)
	{
		static const char b64[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		char *ret, *dst;
		unsigned i_bits = 0;
		int i_shift = 0;
		int bytes_remaining = in_size;

		if (out_size < BASE64_SIZE(in_size))
			return NULL;

		ret = dst = out;
		while (bytes_remaining) {
			i_bits = (i_bits << 8) + *in++;
			bytes_remaining--;
			i_shift += 8;

			do {
				*dst++ = b64[(i_bits << 6 >> i_shift) & 0x3f];
				i_shift -= 6;
			} while (i_shift > 6 || (bytes_remaining == 0 && i_shift > 0));
		}
		while ((dst - ret) & 3)
			*dst++ = '=';
		*dst = '\0';

		return ret;
	}



	int stringCut(const char *pcSrc, const char *start, const char *end, char *pcDest)
	{
		const char *posStart = NULL, *posEnd = NULL;
		int len;

		if (0 == *pcSrc || NULL == pcDest)
		{
			return -1;
		}

		if (NULL == start)
		{
			posStart = pcSrc;
		}
		else
		{
			posStart = strstr(pcSrc, start);
			if (NULL == posStart)
			{
				return -1;
			}
			/* ignore header */
			posStart++;
		}

		if (NULL == end)
		{
			posEnd = posStart + strlen(posStart);
		}
		else
		{
			posEnd = strstr(pcSrc, end);
			if (NULL == posEnd)
			{
				return -1;
			}
		}

		len = posEnd - posStart;

		strncpy(pcDest, posStart, len);

		return len;
	}
	int connectSmtp(const char* smtpUrl, const unsigned short smtpPort)
	{
		int socketFd = -1;
		struct sockaddr_in smtpAddr = { 0 };
		struct hostent *host = NULL;

		if (NULL == (host = gethostbyname(smtpUrl)))
		{
			perror("gethostbyname...\n");
			exit(-1);
		}

		memset(&smtpAddr, 0, sizeof(smtpAddr));
		smtpAddr.sin_family = AF_INET;
		smtpAddr.sin_port = htons(smtpPort);
		smtpAddr.sin_addr = *((struct in_addr *)host->h_addr);

		socketFd = socket(PF_INET, SOCK_STREAM, 0);
		if (0 > socketFd)
		{
			perror("socket...\n");
			exit(-1);
		}

		if (0 > connect(socketFd, (struct sockaddr *)&smtpAddr, sizeof(struct sockaddr)))
		{
			closesocket(socketFd);
			perror("connect...\n");
			exit(-1);
		}

		return socketFd;
	}

	int safeRead(int socketFd, char *readData, int readLen)
	{
#ifdef NONEED_SELECT
		int recvLen = 0;
		do{
			recvLen = recv(socketFd, readData, readLen, 0);
		} while (recvLen == 0);

		return recvLen;
#else
		fd_set readFds;
		struct timeval tv;
		int ret, len = 0;

		if ((0 >= readLen) || (NULL == readData))
		{
			return -1;
		}

		FD_ZERO(&readFds);
		FD_SET(socketFd, &readFds);

		tv.tv_sec = 5;

		while(1)
		{
			ret = select(socketFd+1, &readFds, NULL, NULL, &tv);
			if (0 > ret)
			{
				perror("select...\n");
				/* reconnect or ... */
			}
			else if (0 == ret)
			{
				continue;
				/* reconnect or continue wait */
			}
			else
			{
				/* got it */
				if(FD_ISSET(socketFd, &readFds))
				{
					/* don't do read twice */
					len = recv(socketFd, readData, readLen, 0);
					/* take data,and leave */
					break;
				}
			}
		}

		return len;
#endif
	}

	int safeWrite(int socketFd, const char *writeData, int writeLen)
	{
#ifdef NONEED_SELECT
		return send(socketFd, writeData, writeLen, 0);
#else
		fd_set writeFds;
		struct timeval tv;
		int ret, len = 0;

		if ((0 >= writeLen) || (NULL == writeData))
		{
			return -1;
		}

		FD_ZERO(&writeFds);
		FD_SET(socketFd, &writeFds);

		tv.tv_sec = 5;

		while(1)
		{
			ret = select(socketFd+1, NULL, &writeFds, NULL, &tv);
			if (0 > ret)
			{
				perror("select...\n");
				/* reconnect or ... */
			}
			else if (0 == ret)
			{
				continue;
				/* reconnect or continue wait */
			}
			else
			{
				if(FD_ISSET(socketFd, &writeFds))
				{
					len = send(socketFd, writeData, writeLen, 0);
					break;
				}
			}
		}

		return len;
#endif
	}

	/*
	 * You can find more detail in here.
	 * http://www.ietf.org/rfc/rfc821.txt
	 */
	int recvStatus(const char *recvString)
	{
		char statusStr[4] = { 0 };

		memset(statusStr, 0, sizeof(statusStr));
		strncpy(statusStr, recvString, 3);

		switch (atoi(statusStr))
		{
		case 250:
		{
			break;
		}
		case 235:
		{
			break;
		}
		case 354:
		{
			break;
		}
		case 334:
		{
			break;
		}
		case 221:
		{
			break;
		}
		default:
		{
			printf("\r\n# You could be got exception status !\r\n");
			return -1;
		}
		}

		return 0;
	}

	int authEmail(const int socketFd, const char *mailAddr, const char *mailPasswd)
	{
		int outSize = 0;
		char readData[SMTP_MTU] = { 0 };
		char writeData[SMTP_MTU] = { 0 };
		char userName[MAX_EMAIL_LEN] = { 0 };
		char userPasswd[MAX_EMAIL_LEN] = { 0 };

		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		/* Send: EHLO */
		safeWrite(socketFd, "EHLO smtp.163.com\r\n", strlen("EHLO smtp.163.com\r\n"));

		/* Recv: EHLO */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: AUTH LOGIN */
		safeWrite(socketFd, "AUTH LOGIN\r\n", strlen("AUTH LOGIN\r\n"));

		/* Recv: AUTH LOGIN */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: username */
		memset(&userName, 0, MAX_EMAIL_LEN);
		memset(&writeData, 0, SMTP_MTU);
		//stringCut(mailAddr, NULL, "@", userName);
		strcpy(userName, mailAddr);

		outSize = BASE64_SIZE(strlen(userName));
		base64_encode(writeData, outSize, userName, strlen(userName));
		strcat(writeData, "\r\n");
		safeWrite(socketFd, writeData, strlen(writeData));

		/* Recv: username */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: passwd */
		memset(&userPasswd, 0, MAX_EMAIL_LEN);
		strcpy(userPasswd, mailPasswd);
		memset(&writeData, 0, SMTP_MTU);
		outSize = BASE64_SIZE(strlen(userPasswd));
		base64_encode(writeData, outSize, userPasswd, strlen(userPasswd));
		strcat(writeData, "\r\n");
		safeWrite(socketFd, writeData, strlen(writeData));

		/* Recv: passwd */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		return 0;
	}

	int sendEmail(const int socketFd, const char *fromMail, const char *toMail,
		const char *textMail, const int textLen)
	{
		char readData[SMTP_MTU] = { 0 };
		char writeData[SMTP_MTU] = { 0 };

		/* Send: MAIL FROM */
		memset(&writeData, 0, SMTP_MTU);
		sprintf(writeData, "MAIL FROM:<%s>\r\n", fromMail);
		safeWrite(socketFd, writeData, strlen(writeData));

		/* Recv: MAIL FROM */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: RCPT TO */
		memset(&writeData, 0, SMTP_MTU);
		sprintf(writeData, "RCPT TO:<%s>\r\n", toMail);
		safeWrite(socketFd, writeData, strlen(writeData));

		/* Recv: RCPT TO */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: DATA */
		memset(&writeData, 0, SMTP_MTU);
		safeWrite(socketFd, "DATA\r\n", strlen("DATA\r\n"));

		/* Recv: DATA */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: MAIL TEXT */
		safeWrite(socketFd, textMail, textLen);

		/* Recv: MAIL TEXT */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		/* Send: QUIT */
		memset(&writeData, 0, SMTP_MTU);
		safeWrite(socketFd, "QUIT\r\n", strlen("QUIT\r\n"));

		/* Recv: QUIT */
		memset(&readData, 0, SMTP_MTU);
		safeRead(socketFd, readData, SMTP_MTU);

		recvStatus(readData);

		return 0;
	}

	/* subject and body could be null. */
	int mailText(char **mail, const char *fromMail, const char *toMail,
		const char *mailSubject, const char *mailBody)
	{
		char *mailText = NULL;
		int mailTextLen = 0;
		char fromName[MAX_EMAIL_LEN] = { 0 };
		char toName[MAX_EMAIL_LEN] = { 0 };

		if ((NULL == *mail) || (NULL == fromMail) || (NULL == toMail))
		{
			printf("[%s][%d] \r\n", __FILE__, __LINE__);
			return -1;
		}

		mailTextLen = strlen(fromMail) + strlen(toMail) + strlen(mailSubject) + strlen(mailBody) + 500;

		mailText = (char *)calloc(mailTextLen, 1);
		if (NULL == mailText)
		{
			perror("malloc...\n");
			return -1;
		}

		memset(&fromName, 0, MAX_EMAIL_LEN);
		stringCut(fromMail, NULL, "@", fromName);

		memset(&toName, 0, MAX_EMAIL_LEN);
		stringCut(toMail, NULL, "@", toName);

		//_snprintf(mailText, mailTextLen, "From: \"%s\"<%s>\r\nTo: \"%s\"<%s>\r\nSubject: %s\r\nMIME-Version:1.0\r\nContent-Type:multipart/mixed;boundary=\"%s\"\r\n\r\n--%s\r\nContent-Type: text/plain; charset=\"gb2312\"\r\n\r\n%s\r\n\r\n--%s\r\n", fromName, fromMail, toName, toMail, mailSubject, TEXT_BOUNDARY, TEXT_BOUNDARY, mailBody, TEXT_BOUNDARY);
		snprintf(mailText, mailTextLen, "From: \"%s\"<%s>\r\nTo: \"%s\"<%s>\r\nSubject: %s\r\nMIME-Version:1.0\r\nContent-Type:multipart/alternative;boundary=\"%s\"\r\n\r\n--%s\r\nContent-Type: text/plain; charset=\"gb2312\"\r\n\r\n%s\r\n\r\n--%s\r\n", fromName, fromMail, toName, toMail, mailSubject, TEXT_BOUNDARY, TEXT_BOUNDARY, mailBody, TEXT_BOUNDARY);
		*mail = (char*)realloc(*mail, strlen(*mail) + strlen(mailText) + 1);
		if (NULL == *mail)
		{
			perror("realloc...\n");
			/* what should I do? */
			return -1;
		}

		strcat(*mail, mailText);

		free(mailText);

		/* If I am in danger, let me konw, please */
		return (mailTextLen - strlen(*mail));
	}

	/* static attachmemt size */
	int mailAttachment(char **mail, const char *filePath)
	{
		FILE *fp = NULL;
		int fileSize, base64Size, headerSize, len;
		char *attach = NULL, *base64Attach = NULL, *attachHeader = NULL;
		char fileName[MAX_EMAIL_LEN] = { 0 };
		const char *contentType = "Content-Type: application/octet-stream";
		const char *contentEncode = "Content-Transfer-Encoding: base64";
		const char *contentDes = "Content-Disposition: attachment";

		fp = fopen(filePath, "rb");
		if (NULL == fp)
		{
			perror("open...");
			return -1;
		}

		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		if (0 > fileSize)
		{
			perror("ftell...\n");
			return -1;
		}

		rewind(fp);

		attach = (char*)calloc(fileSize, 1);
		if (NULL == attach)
		{
			perror("malloc...");
			return -1;
		}

		headerSize = strlen(contentType) + strlen(contentEncode) + strlen(contentDes) + 200;
		attachHeader = (char*)calloc(headerSize, 1);
		if (NULL == attach)
		{
			perror("malloc...");
			return -1;
		}

		/* attachment header */
		stringCut(filePath, "/", NULL, fileName);

		sprintf(attachHeader, "%s;name=\"%s\"\r\n%s\r\n%s;filename=\"%s\"\r\n\r\n", contentType, fileName, contentEncode, contentDes, fileName);

		base64Size = BASE64_SIZE(fileSize);
		base64Attach = (char*)calloc(base64Size, 1);
		if (NULL == base64Attach)
		{
			perror("malloc...");
			return -1;
		}

		len = fread(attach, sizeof(char), fileSize, fp);

		/* attachment transform to base64 */
		base64_encode(base64Attach, base64Size, attach, fileSize);

		free(attach);

		*mail = (char*)realloc(*mail, strlen(*mail) + headerSize + base64Size + 1);
		if (NULL == *mail)
		{
			perror("realloc...\n");
			/* what should I do? */
			return -1;
		}

		strcat(*mail, attachHeader);
		strcat(*mail, base64Attach);

		free(attachHeader);
		free(base64Attach);

		return fileSize;
	}

	int mailEnd(char **mail)
	{
		char bodyEnd[200] = { 0 };
		int len;

		memset(bodyEnd, 0, sizeof(bodyEnd));
		sprintf(bodyEnd, "\r\n--%s--\r\n\r\n.\r\n", TEXT_BOUNDARY);

		len = strlen(bodyEnd);

		*mail = (char*)realloc(*mail, strlen(*mail) + len + 1);
		if (NULL == *mail)
		{
			perror("realloc...\n");
			/* what should I do? */
			return -1;
		}

		strcat(*mail, bodyEnd);

		return 0;
	}

}