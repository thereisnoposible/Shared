
/********************************************************
自动定参数类型
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

		/**加入一个新的参数。参数名不能和以有的重名
		*/
		bool  addValue(const std::string& key, const std::string& val);

		/**获取一个参数类型
		*/
		bool  getValue(const std::string& key, std::string& val);

		/**是否有一个key
		*/
		bool hasKey(const std::string& key);
	protected:
		typedef std::map<std::string, std::string> ParamentMap;
		ParamentMap     m_Paramnets;
	};
}

#pragma warning (pop)