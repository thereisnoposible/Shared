
/********************************************************
�Զ�����������
*********************************************************/


#pragma  once



class  XClass	 CustParament
{
public:

	CustParament();

	virtual  ~CustParament();


	/**����һ���µĲ��������������ܺ����е�����
	*/
	bool  addValue(const xstring& key,const xstring& val);


	/**��ȡһ����������
	*/
	bool  getValue(const xstring& key,xstring& val);



	/**�Ƿ���һ��key
	*/
	bool hasKey(const xstring& key);






protected:

	typedef std::map<xstring,xstring> ParamentMap;

	ParamentMap     m_Paramnets;


};