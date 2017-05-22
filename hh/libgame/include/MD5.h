#pragma once

/* CMD5 Class. */
class XClass CMD5
{
public:
	CMD5();
	~CMD5();
public:
	void MD5Update (const char *input, const uint32 inputLen);
	void MD5Final (char digest[16]);
	xstring MD5String(const xstring &inputString);

private:
	uint32 state[4];     /* state (ABCD) */
	uint32 count[2];     /* number of bits, modulo 2^64 (lsb first) */
	uint8 buffer[64];       /* input buffer */
	uint8 PADDING[64];

private:
	void MD5Init ();
	void MD5Transform (uint32 state[4], uint8 block[64]);
	void MD5_memcpy (uint8* output, uint8* input,uint32 len);
	void Encode (uint8 *output, uint32 *input,uint32 len);
	void Decode (uint32 *output, uint8 *input, uint32 len);
	void MD5_memset (uint8* output,int32 value,uint32 len);
};

