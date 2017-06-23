
/********************************************************
�Զ�����������
*********************************************************/

#pragma  once
#include "../include/define/define.h"
#include <map>

#pragma warning (push)

#pragma warning (disable: 4251)

namespace xlib
{
	class  XDLL	 CustParament
	{
	public:
		CustParament();
		virtual  ~CustParament();

		/**����һ���µĲ��������������ܺ����е�����
		*/
		bool  addValue(const std::string& key, const std::string& val);

		/**��ȡһ����������
		*/
		bool  getValue(const std::string& key, std::string& val);

		/**�Ƿ���һ��key
		*/
		bool hasKey(const std::string& key);
	protected:
		typedef std::map<std::string, std::string> ParamentMap;
		ParamentMap     m_Paramnets;
	};
}

#pragma warning (pop)