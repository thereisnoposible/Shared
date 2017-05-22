#include "../include/keywordFilter/DFA.h"
#include "../include/helper/Helper.h"

#include <fstream>

using namespace std;
using namespace xlib;
int32 DFANode::m_nLen = 0;

//-------------------------------------------------------------------------
DFANode::DFANode(keytype key)
	:m_Key(key	),m_nStart(0)
{

}


//-------------------------------------------------------------------------
DFANode::~DFANode()
{
	DFANodeMap::iterator  it=m_ChildNode.begin();
	DFANodeMap::iterator  itend=m_ChildNode.end();

	for(;it!=itend;++it)
	{
		if(it->second!=NULL)
		{
			delete it->second;
		}
	}

	m_ChildNode.clear();
}


//-------------------------------------------------------------------------
bool  DFANode::addNode(const keytype* key,int len)
{
	if(key==NULL||len<0)
	{
		return false;
	}

 
	int i=0;
	DFANode* ptemNode=this;
	while(i<len)
	{
		ptemNode=ptemNode->addNode(key[i],(i==len-1)?true:false);
		++i;
	}
	
	return true;

}




//-------------------------------------------------------------------------
DFANode* DFANode::addNode(keytype key,bool end)
{
	DFANodeMap::iterator it=m_ChildNode.find(key);

	if(it==m_ChildNode.end(	))
	{
		DFANode* temnode=new DFANode(key);
		m_ChildNode.insert(std::make_pair(key,temnode));
		return temnode;
	}else
	{
		DFANode* pNode = it->second;
		if (end)
		{//如果是最后一个,清除掉所有的子节点
			pNode->clearChild();
		}

		return pNode;
	}
}

//-------------------------------------------------------------------------
void DFANode::clearChild()
{
	DFANodeMap::iterator it = m_ChildNode.begin();
	for (; it != m_ChildNode.end(); it++)
	{
		DFANode* pNode = it->second;
		if (pNode != NULL)
		{
			pNode->clearChild();

			delete pNode;
		}
	}
	m_ChildNode.clear();
}


//-------------------------------------------------------------------------
int  DFANode::getChildNodeCount()const
{
	return m_ChildNode.size();

}


//-------------------------------------------------------------------------
DFANode* DFANode::getNode(keytype key) const 
{
	DFANodeMap::const_iterator  it=m_ChildNode.find(key);
	if(it==m_ChildNode.end())
	{
		return NULL	;
	}else
	{
		return it->second;
	}

}



//-------------------------------------------------------------------------
bool   DFANode::hasKeyWord(const keytype* pdata,int len,bool isreturn) 
{
	if(isreturn)
		m_nLen = 0;

	int32 currentPos=0;

	while(currentPos<len)
	{
		keytype p=pdata[currentPos];
		DFANode* pnode=getNode(p);
		m_nLen++;
		if(pnode==NULL)///如果没有找到到。从下一个开始找
		{
			if(isreturn==false)
			{
				return false;
			}

			++currentPos;
			m_nLen = 0;
			continue;

		}else if (pnode->getChildNodeCount()==0) ///如果找到直接返回true
		{
			if(isreturn)
			{
				m_nStart = currentPos;
			}
			return  true;
		}


		int startpos=1+currentPos;
		if(startpos>=len)
		{
			return false;
		}

		bool b= pnode->hasKeyWord(pdata+1+currentPos,len-1-currentPos,false);

		if(b==true)
		{
			if(isreturn)
			{
				m_nStart = currentPos;
			}

			return true;
		}else	
		{
			if(isreturn==false)
			{

				return false;
			}
			++currentPos;

			m_nLen = 0;
			
			continue;
		}

	}


	return false;

}

//--------------------------------------------------------------------------------------------------------------------
bool  DFANode::loadFile(const std::string&  filename)
{
	std::ifstream file;

	file.open(filename.c_str());
	if (file.fail())
	{
		return false ;
	}

	char line[1024]={0};
	while(file.eof()==false&&file.fail()==false)
	{

		if(file.getline(line,1024))
		{
			std::wstring wline = Helper::UTF8ToUnicode(line);
			if (hasKeyWord(wline.c_str(),wline.size()))
			{
				continue;
			}
			addNode(wline.c_str(),wline.size());
		}

	}

	return true;
}
//--------------------------------------------------------------------------------------------------------
bool DFANode::isFileterWord(const std::string& word)
{
	std::wstring wpData;
	
	if (Helper::IsTextUTF8(word.c_str(),word.size()))
	{
		wpData = Helper::UTF8ToUnicode(word);
	}
	else
	{
		wpData = Helper::UTF8ToUnicode(Helper::Gb2312ToUTF8(word.c_str()));
	}

	int len = wpData.size();

	return hasKeyWord(wpData.c_str(),len);
}


//--------------------------------------------------------------------------------------------------------
bool DFANode::isFileterWord(const std::wstring& word)
{
	std::wstring wpData;
	int len = word.size();

	return hasKeyWord(word.c_str(),len);
}


//--------------------------------------------------------------------------------------------------------
void DFANode::getKeyPos(int32& nStart,int32& nLen)
{
	nStart = m_nStart;
	nLen = m_nLen;

	m_nStart = 0;
	m_nLen = 0;
}