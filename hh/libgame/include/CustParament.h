
/********************************************************
自动定参数类型
*********************************************************/


#pragma  once



class  XClass	 CustParament
{
public:

	CustParament();

	virtual  ~CustParament();


	/**加入一个新的参数。参数名不能和以有的重名
	*/
	bool  addValue(const xstring& key,const xstring& val);


	/**获取一个参数类型
	*/
	bool  getValue(const xstring& key,xstring& val);



	/**是否有一个key
	*/
	bool hasKey(const xstring& key);






protected:

	typedef std::map<xstring,xstring> ParamentMap;

	ParamentMap     m_Paramnets;


};