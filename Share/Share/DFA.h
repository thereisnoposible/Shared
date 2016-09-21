

#pragma  once


class DFANode;
typedef  wchar_t   keytype;
typedef xmap<wchar_t, DFANode*>   DFANodeMap;

class XClass DFANode
{

public:



	DFANode(keytype key);



	~DFANode();


	///�����˸��ӽڵ�
	DFANode* addNode(keytype key, bool end=false);


	bool  addNode(const keytype* key,int len);


	///��ȡ�ӽڵ������
	int getChildNodeCount()const;


	///��ȡ���� key���ӽڵ�
	DFANode* getNode(keytype key) const ;



	/**�ж��Ƿ����ָ����key
	*/
	bool  hasKeyWord(const keytype* pdata,int len,bool isreturn = true) ;


	///���عؼ����ļ�
	bool  loadFile(const std::string&  filename);

	///�ж��Ƿ�Ϊ�ؼ��� �ǵĻ�true
	bool isFileterWord(const std::string& word);

	/**
	* @brief		�ж��Ƿ�Ϊ�ؼ��֣��ǵĻ�true
	* @details		���������wstring
	* @param[in]	word[wstring]: �����Ŀ��ַ���
	* @return		bool : �����ַ���
	* @remarks
	*/
	bool isFileterWord(const std::wstring& word);


	/**
	* @brief		�õ����Ĺؼ���λ����Ϣ
	* @param[out]	nStart[int32] : ��ʼλ��
	* @param[out]	nLen[int32] : �ؼ��ֳ���
	* @return		void
	* @remarks
	*/
	void getKeyPos(int32& nStart,int32& nLen);

protected:
	void clearChild();

protected:


	keytype      m_Key;

	DFANodeMap   m_ChildNode;//�����ֽڵ�

	int32 m_nStart;
	static int32 m_nLen;

protected:


};
