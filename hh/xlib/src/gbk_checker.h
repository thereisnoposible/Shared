#pragma once

class GBKChecker
{
public:
    static bool is_name_valid(const char* pName);
    static bool is_allow_gbk(unsigned char b0, unsigned char b1);
    static bool is_allow_ascii(unsigned char b);
public:
    static bool _is_gbk_character_left(unsigned char b);
    static bool _is_gbk_character_right(unsigned char b);

private:
    static bool gbk_allow_table[256][256];
    static bool ascii_allow_table[256];
};
