/*
 * 2013-9-10:build by ken <http://kenchowcn.com>
 *
 */

#pragma once

#define NONEED_SELECT     1


#define SMTP_PORT         25
#define SMTP_MTU          800
#define MAX_EMAIL_LEN     64
#define TEXT_BOUNDARY     "KkK170891tpbkKk__FV_KKKkkkjjwq"


int authEmail(const int socketFd, const char *mailAddr, const char *mailPasswd);
int sendEmail(const int socketFd, const char *fromMail, const char *toMail,
                 const char *textMail, const int textLen);
int mailText(char **mail, const char *fromMail, const char *toMail,
                 const char *mailSubject, const char *mailBody);
int mailAttachment(char **mail, const char *filePath);
int mailEnd(char **mail);
int connectSmtp(const char* smtpUrl, const unsigned short smtpPort);
