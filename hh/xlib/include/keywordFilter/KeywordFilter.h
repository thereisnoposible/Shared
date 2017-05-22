#pragma once

#include "../include/define/define.h"
#include "DFA.h"

#include <string>
#include <map>
#include <set>

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{

	class XDLL KeywordFilter
	{
	public:
		/**
		* @brief		constructor
		*/
		KeywordFilter();

		/**
		* @brief		destrcutor
		*/
		~KeywordFilter();

		bool IsRealAreaCode(int32 province, int32 city, int32 county);

	public:
		/**
		* @brief		是否是合法的账号、密码
		* @param[in]	str[std::string] : 待检测字符
		* @return		bool : true=合法 false=非法
		* @remarks		只能是字母开头的字符数字、'_'、'@'符合的组合
		*/
		bool IsLaywerAccount(const std::string& str);

		bool IsLayerPassword(const std::string& str);

		/**
		* @brief		是否包含过滤词语
		* @param[in]	str[std::string] : 待检测字符
		* @return		bool : true=包含 false=不包含
		* @remarks
		*/
		bool HasFilterWord(const std::string& str);

		/**
		* @brief		是否包含过滤词语 并且过滤掉他
		* @param[in]	str[std::string] : 待检测字符
		* @return		bool : true=包含 false=不包含
		* @remarks
		*/
		bool HasFilterWordAndReplace(std::string& str, const std::string& replace);

		bool LoopFilterWordAndReplace(std::string& str, const std::string& replace, int max_loop_count = 100);

		void UnicodePosToUTF8Pos(int32 uniStart, int32 uniLen, std::string utfStr, int32& utfStart, int32& utfLen);

		int32 GetUTF8StringSize(std::string utfStr);

		bool IsLaywerName(const std::string& str);

		bool IsLaywerIDCard(const std::string& idcard);

	protected:
		void init();

	private:
		DFANode*	m_pDFATable;			/**< 词典表 */

		std::map<int32, std::map<int32, std::set<int32>>> areacode; /**< 地区码 */
	};
}

#pragma warning (pop)