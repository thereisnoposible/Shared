/*
 * 2013-9-10:build by ken <http://kenchowcn.com>
 *
 */

#pragma once
#include "../include/define/define.h"

namespace xlib
{
	/**
	* Decode a base64-encoded string.
	*
	* @param out      buffer for decoded data
	* @param in       null-terminated input string
	* @param out_size size in bytes of the out buffer, must be at
	*                 least 3/4 of the length of in
	* @return         number of bytes written, or a negative value in case of
	*                 invalid input
	*/
	XDLL int base64_decode(unsigned char *out, const char *in, int out_size);

	/**
	* Encode data to base64 and null-terminate.
	*
	* @param out      buffer for encoded data
	* @param out_size size in bytes of the output buffer, must be at
	*                 least BASE64_SIZE(in_size)
	* @param in_size  size in bytes of the 'in' buffer
	* @return         'out' or NULL in case of error
	*/
	XDLL char *base64_encode(char *out, int out_size, const char *in, int in_size);

	XDLL int authEmail(const int socketFd, const char *mailAddr, const char *mailPasswd);
	XDLL int sendEmail(const int socketFd, const char *fromMail, const char *toMail,
		const char *textMail, const int textLen);
	XDLL int mailText(char **mail, const char *fromMail, const char *toMail,
		const char *mailSubject, const char *mailBody);
	XDLL int mailAttachment(char **mail, const char *filePath);
	XDLL int mailEnd(char **mail);
	XDLL int connectSmtp(const char* smtpUrl, const unsigned short smtpPort);

	//example
	//const EMailSandEvent* pEvt = dynamic_cast<const EMailSandEvent*>(&eventx);
	//const EMailSandData& data = pEvt->GetData();
	//int ret = 0;
	//char* pMail = (char*)calloc(1, 1);

	//ret = ::mailText(&pMail, m_fromMail.c_str(), data.toMail.c_str(), data.titleMail.c_str(), data.textMail.c_str());
	//ret = ::mailEnd(&pMail);
	//m_fd = ::connectSmtp(m_smtpUrl.c_str(), SMTP_PORT);
	//ret = ::authEmail(m_fd, m_fromMail.c_str(), m_fromMailPwa.c_str());
	//ret = ::sendEmail(m_fd, m_fromMail.c_str(), data.toMail.c_str(), pMail, strlen((char*)pMail));
	//closesocket(m_fd);
	//free(pMail);
	//m_fd = 0;
}


