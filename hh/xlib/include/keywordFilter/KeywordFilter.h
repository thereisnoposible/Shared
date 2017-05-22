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
		* @brief		�Ƿ��ǺϷ����˺š�����
		* @param[in]	str[std::string] : ������ַ�
		* @return		bool : true=�Ϸ� false=�Ƿ�
		* @remarks		ֻ������ĸ��ͷ���ַ����֡�'_'��'@'���ϵ����
		*/
		bool IsLaywerAccount(const std::string& str);

		bool IsLayerPassword(const std::string& str);

		/**
		* @brief		�Ƿ�������˴���
		* @param[in]	str[std::string] : ������ַ�
		* @return		bool : true=���� false=������
		* @remarks
		*/
		bool HasFilterWord(const std::string& str);

		/**
		* @brief		�Ƿ�������˴��� ���ҹ��˵���
		* @param[in]	str[std::string] : ������ַ�
		* @return		bool : true=���� false=������
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
		DFANode*	m_pDFATable;			/**< �ʵ�� */

		std::map<int32, std::map<int32, std::set<int32>>> areacode; /**< ������ */
	};
}

#pragma warning (pop)