

#pragma  once


class DFANode;
typedef  wchar_t   keytype;
typedef xmap<wchar_t, DFANode*>   DFANodeMap;

class XClass DFANode
{

public:



	DFANode(keytype key);



	~DFANode();


	///加入了个子节点
	DFANode* addNode(keytype key, bool end=false);


	bool  addNode(const keytype* key,int len);


	///获取子节点的数量
	int getChildNodeCount()const;


	///获取包含 key的子节点
	DFANode* getNode(keytype key) const ;



	/**判断是否包含指定的key
	*/
	bool  hasKeyWord(const keytype* pdata,int len,bool isreturn = true) ;


	///加载关键字文件
	bool  loadFile(const std::string&  filename);

	///判断是否为关键字 是的话true
	bool isFileterWord(const std::string& word);

	/**
	* @brief		判断是否为关键字，是的话true
	* @details		输入必须是wstring
	* @param[in]	word[wstring]: 待检测的宽字符串
	* @return		bool : 包含字符串
	* @remarks
	*/
	bool isFileterWord(const std::wstring& word);


	/**
	* @brief		得到最后的关键词位置信息
	* @param[out]	nStart[int32] : 起始位置
	* @param[out]	nLen[int32] : 关键字长度
	* @return		void
	* @remarks
	*/
	void getKeyPos(int32& nStart,int32& nLen);

protected:
	void clearChild();

protected:


	keytype      m_Key;

	DFANodeMap   m_ChildNode;//所有字节点

	int32 m_nStart;
	static int32 m_nLen;

protected:


};
