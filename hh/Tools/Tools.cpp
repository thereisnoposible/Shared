// Tools.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <iosfwd>
#include <set>
#include <map>
#include "Helper.h"
#include "Helper.cpp"
#include "proto_data.h"



using namespace std;
using namespace boost::property_tree;
void loadtype();
void loaddb();

int _tmain(int argc, _TCHAR* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	//loadtype();
	//loaddb();
	cout << "--type  默认文件后缀*.type 批量生成加载配置表代码\n";
	cout << "--db    默认文件后缀*.db	批量生成数据库代码\n";

	string cmd;
	cin >> cmd;
	while (cmd != "exit")
	{
		if (cmd == "type")
		{
			loadtype();
			std::cout << "loadtype finished\n";
		}

		if (cmd == "db")
		{
			loaddb();
			std::cout << "loaddb finished\n";
		}

		cin >> cmd;
	}

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}

struct ClassParam
{
	bool operator<(const ClassParam& right)const
	{
		return keyindex < right.keyindex;
	}

	string type;
	string name;
	int key;
	int keyindex;
	int istimedate;
};

struct ExClassParam
{
	string type;
	string subtype;
	vector<ClassParam> params;

	string name;
	string src;
	string split;
};

struct ClassItem
{
	string name;

	vector<ClassParam> params;
	vector<ExClassParam> exparams;

	map<int, set<ClassParam>> keygroup;
	map<int, vector<string>> keygroupname;
};

void loadclasses(ptree classes,vector<ClassItem>& clss)
{
	for (ptree::iterator subit = classes.begin(); subit != classes.end(); ++subit)
	{
		std::cout << subit->first;
		std::cout << "\n";

		if (subit->first != "class")
			continue;
		
		ptree clas = subit->second;

		ClassItem item;
		item.name = clas.get<string>("<xmlattr>.table_name");

		ptree fileds = clas.get_child("fileds", ptree());
		ptree fileds_child = fileds.get_child("");

		for (ptree::iterator filedsit = fileds_child.begin(); filedsit != fileds_child.end(); ++filedsit)
		{
			if (filedsit->first == "<xmlattr>")
				continue;
			if (filedsit->first == "<xmlcomment>")
				continue;

			if (filedsit->first == "field")
			{
				ClassParam temp;
				temp.type = filedsit->second.get<string>("<xmlattr>.type", "");
				if (temp.type == "string" || temp.type == "std::string")
					temp.type = "xstring";
				temp.name = filedsit->second.get<string>("<xmlattr>.name", "");
				temp.key = Helper::StringToInt32(filedsit->second.get<string>("<xmlattr>.key", "0"));
				temp.keyindex = Helper::StringToInt32(filedsit->second.get<string>("<xmlattr>.keyindex", "0"));
				temp.istimedate = Helper::StringToInt32(filedsit->second.get<string>("<xmlattr>.istimedate", "0"));
				if (temp.type.empty())
				{
					std::cout << "has empty type\n";
					return;
				}

				if (temp.name.empty())
				{
					std::cout << "has empty name\n";
					return;
				}

				item.params.push_back(temp);

				if (temp.key > 0)
				{
					set<ClassParam>& keyindex = item.keygroup[temp.key];
					auto res = keyindex.insert(temp);
					if (res.second == false)
					{
						std::cout << "has same keyindex\n";
						return;
					}
					vector<string>& keyname = item.keygroupname[temp.key];
					keyname.clear();
					
					for (auto iiit = keyindex.rbegin(); iiit != keyindex.rend(); ++iiit)
					{
						string str = "std::map<";
						str += iiit->type;
						str += ", ";
						str += keyname.size() > 0 ? keyname[keyname.size() - 1] : (item.name + "*");
						str += ">";

						keyname.push_back(str);
					}
				}
			}
			
		}

		ptree split = clas.get_child("split", ptree());
		ptree split_child = split.get_child("");
		for (ptree::iterator splitsit = split_child.begin(); splitsit != split_child.end(); ++splitsit)
		{
			if (splitsit->first == "<xmlattr>")
				continue;
			if (splitsit->first == "<xmlcomment>")
				continue;

			if (splitsit->first != "member")
				continue;
			
			ExClassParam exparam;
			exparam.type = splitsit->second.get<string>("<xmlattr>.type", "");
			exparam.subtype = splitsit->second.get<string>("<xmlattr>.subtype", "");
			if (exparam.subtype == "string" || exparam.subtype == "std::string")
				exparam.subtype = "xstring";
			exparam.name = splitsit->second.get<string>("<xmlattr>.name", "");
			exparam.src = splitsit->second.get<string>("<xmlattr>.src", "");
			exparam.split = splitsit->second.get<string>("<xmlattr>.split", "");

			ptree memb = splitsit->second.get_child("", ptree());


			for (auto membit = memb.begin(); membit != memb.end(); ++membit)
			{
				if (membit->first == "filed")
				{
					ClassParam temp;

					string temp1 = membit->first;

					temp.type = membit->second.get<string>("<xmlattr>.type", "");
					if (temp.type == "string" || temp.type == "std::string")
						temp.type = "xstring";
					temp.name = membit->second.get<string>("<xmlattr>.name", "");

					if (temp.type.empty())
					{
						std::cout << "member_type empty\n";
						return;
					}

					if (temp.name.empty())
					{
						std::cout << "member_name empty\n";
						return;
					}

					exparam.params.push_back(temp);
				}

			}

			if (exparam.type.empty())
			{
				std::cout << "split_type empty\n";
				return;
			}

			if (exparam.name.empty())
			{
				std::cout << "split_name empty\n";
				return;
			}

			if (exparam.src.empty())
			{
				std::cout << "split_src empty\n";
				return;
			}

			if (exparam.type == "vector")
			{
				if (exparam.subtype.empty())
				{
					std::cout << "split_subtype empty\n";
					return;
				}

				if (exparam.split.size() < 1)
				{
					std::cout << "split < 1\n";
					return;
				}
			}
			else
			{
				if (!exparam.subtype.empty())
				{
					std::cout << "split_subtype not empty\n";
					return;
				}
			}

			item.exparams.push_back(exparam);
		}

		clss.push_back(item);
	}
}

void writeclass(std::stringstream& ss, ClassItem&item)
{
	ss << "class " << item.name << "\n";
	ss << "{\n";
	ss << "public:\n";
	for (size_t i = 0; i < item.params.size(); i++)
	{
		ss << "\t" << item.params[i].type << "\t\t" << item.params[i].name << ";\n";
	}

	ss << "\n\n//exparam\n";
	for (size_t i = 0; i < item.exparams.size(); i++)
	{
		if (item.exparams[i].type == "vector")
			ss << "\n\t" << "xvector";
		else
			ss << "\n\t" << item.exparams[i].type;
		if (!item.exparams[i].subtype.empty())
		{
			ss << "<" << item.exparams[i].subtype << ">";
		}

		ss << "\t\t" << item.exparams[i].name << ";";
	}
	ss << "\n};\n\n";
}

void writetable_destructor(std::stringstream& ss, ClassItem&item)
{
	ss << "\t////////////////////////////////////////////////////////////////////////////\n";
	ss << "\t~" << item.name << "Table()\n";
	ss << "\t{\n";
	ss << "\t\tthis->Release();\n";
	ss << "\t}\n";
}

void writetable_release(std::stringstream& ss, ClassItem&item)
{
	ss << "\t////////////////////////////////////////////////////////////////////////////\n";
	ss << "\tvoid Release()\n";
	ss << "\t{\n";
	ss << "\t\tfor(size_t i = 0; i < data_.size(); i++)\n";
	ss << "\t\t{\n";
	ss << "\t\t\tdelete data_[i];\n";
	ss << "\t\t}\n\n";
	ss << "\t\tdata_.clear();\n";
	for (size_t i = 0; i < item.keygroupname.size(); i++)
	{
		ss << "\t\tmappings_" << i + 1 << ".clear();\n";
	}
	ss << "\t}\n";
}

void writetable_load(std::stringstream& ss, ClassItem&item)
{
	ss << "\tbool Load()\n";
	ss << "\t{\n";
	ss << "\t\tstd::string sql = \"select * from " << item.name << "\";\n";
	ss << "\t\tStmtSyncResult result;\n";
	ss << "\t\tsStmt.SyncExecuteQueryVariable(sql, nullptr, result);\n";
	ss << "\t\twhile (!result.pResult.eof())\n";
	ss << "\t\t{\n";
	ss << "\t\t\t" << item.name << "* temp = new " << item.name << ";\n";
	for (size_t i = 0; i < item.params.size(); i++)
	{
		if (item.params[i].type == "int32")
			ss << "\t\t\ttemp->" << item.params[i].name << " = result.pResult.GetInt32(\"" << item.params[i].name << "\");\n";
		if (item.params[i].type == "xstring")
			ss << "\t\t\ttemp->" << item.params[i].name << " = result.pResult.GetString(\"" << item.params[i].name << "\");\n";
		if (item.params[i].type == "int64")
		{
			if (item.params[i].istimedate == 0)
			{
				ss << "\t\t\ttemp->" << item.params[i].name << " = result.pResult.GetInt64(\"" << item.params[i].name << "\");\n";
			}
			else
			{
				ss << "\t\t\ttemp->" << item.params[i].name << " = result.pResult.GetDateTime(\"" << item.params[i].name << "\");\n";
			}
		}
	}

	for (size_t i = 0; i < item.exparams.size(); i++)
	{
		ss << "\t\t\t{\n";
		ss << "\t\t\t\tstd::vector<xstring> ssplit;\n";
		ss << "\t\t\t\tHelper::SplitString(temp->" << item.exparams[i].src << ", \"" << item.exparams[i].split[0] << "\", ssplit);\n";
		ss << "\t\t\t\tfor(size_t i = 0;i < ssplit.size();i++)\n";
		ss << "\t\t\t\t{\n";

		if (item.exparams[i].type == "vector")
		{
			if (item.exparams[i].subtype == "int32")
				ss << "\t\t\t\t\ttemp->" << item.exparams[i].name << ".push_back(Helper::StringToInt32(ssplit[i]));\n";
			else if (item.exparams[i].subtype == "int64")
				ss << "\t\t\t\t\ttemp->" << item.exparams[i].name << ".push_back(Helper::StringToInt64(ssplit[i]));\n";
			else if (item.exparams[i].subtype == "xstring")
				ss << "\t\t\t\t\ttemp->" << item.exparams[i].name << ".push_back(ssplit[i]);\n";
			else
			{
				ss << "\t\t\t\t\tstd::vector<string> sub_ssplit;\n";
				ss << "\t\t\t\t\tHelper::SplitString(ssplit[i], \"" << item.exparams[i].split[1] << "\", sub_ssplit);\n";
				ss << "\t\t\t\t\tfor(size_t j = 0;j < sub_ssplit.size();j++)\n";
				ss << "\t\t\t\t\t{\n";
				ss << "\t\t\t\t\t\t" << item.exparams[i].subtype << " sub_temp;\n";
				for (size_t j = 0; j < item.exparams[i].params.size(); j++)
				{
					if (item.exparams[i].params[j].type == "int32")
						ss << "\t\t\t\t\t\t" << "sub_temp." << item.exparams[i].params[j].name << ".push_back(Helper::StringToInt32(sub_ssplit[j]));\n";
					if (item.exparams[i].params[j].type == "int64")
						ss << "\t\t\t\t\t\t" << "sub_temp." << item.exparams[i].params[j].name << ".push_back(Helper::StringToInt64(sub_ssplit[j]));\n";
					if (item.exparams[i].params[j].type == "string" || item.exparams[i].params[j].type == "std::string")
						ss << "\t\t\t\t\t\t" << "sub_temp." << item.exparams[i].params[j].name << ".push_back(sub_ssplit[j]);\n";
				}
				ss << "\t\t\t\t\t\t" << "temp->" << item.exparams[i].name << ".push_back(sub_temp);\n";
				ss << "\t\t\t\t\t}\n";
			}

		}
		else
		{
			for (size_t j = 0; j < item.exparams[i].params.size(); j++)
			{
				if (item.exparams[i].params[j].type == "int32")
					ss << "\t\t\t\t\ttemp->" << item.exparams[i].name << "." << item.exparams[i].params[j].name << ".push_back(Helper::StringToInt32(ssplit[i]));\n";
				else if (item.exparams[i].params[j].type == "int64")
					ss << "\t\t\t\t\ttemp->" << item.exparams[i].name << "." << item.exparams[i].params[j].name << ".push_back(Helper::StringToInt64(ssplit[i]));\n";
				else if (item.exparams[i].params[j].type == "xstring")
					ss << "\t\t\t\t\ttemp->" << item.exparams[i].name << "." << item.exparams[i].params[j].name << ".push_back(ssplit[i]);\n";
			}
		}

		ss << "\t\t\t\t}\n";
		ss << "\t\t\t}\n\n";
	}
	ss << "\t\t\t" << "data_.push_back(temp);\n\n";

	int tttt = 1;
	for (auto it = item.keygroup.begin(); it != item.keygroup.end(); ++it)
	{
		if (it->second.size() == 0)
			continue;
		if (it->second.size() == 1)
		{
			ss << "\t\t\t" << "mappings_" << tttt << "[temp->" << it->second.begin()->name << "] = temp;\n";
			continue;
		}
		auto temp = item.keygroupname[it->first];
		int count = temp.size() - 1;

		ss << "\t\t\t" << "auto map_tempit" << tttt << count << " = mappings_" << tttt << ".find(temp->" << it->second.begin()->name << ");\n";
		ss << "\t\t\t" << "if (map_tempit" << tttt << count << " == mappings_" << tttt << ".end())\n";
		ss << "\t\t\t" << "{\n";
		ss << "\t\t\t\t" << "auto temm = mappings_" << tttt << "[temp->" << it->second.begin()->name << "];\n";
		ss << "\t\t\t\t" << "map_tempit" << tttt << count << " = mappings_" << tttt << ".find(temp->" << it->second.begin()->name << ");\n";
		ss << "\t\t\t" << "}\n";
		auto iit = it->second.begin();
		iit++;

		for (; iit != it->second.end(); ++iit)
		{
			count--;
			auto nextiit = iit;
			nextiit++;

			string preitname = "map_tempit";
			preitname += Helper::Int32ToString(tttt);
			preitname += Helper::Int32ToString(count + 1);
			string itname = "map_tempit";
			itname += Helper::Int32ToString(tttt);
			itname += Helper::Int32ToString(count);
			if (nextiit == it->second.end())
			{
				ss << "\t\t\t" << preitname << "->second[temp->" << iit->name << "] = temp;\n";
				break;
			}

			ss << "\t\t\t" << "auto " << itname << " = " << preitname << "->second.find(temp->" << iit->name << ");\n";
			ss << "\t\t\t" << "if (" << itname << " == " << preitname << "->second.end())\n";
			ss << "\t\t\t" << "{\n";
			ss << "\t\t\t\t" << "auto temm = " << preitname << "->second[temp->" << iit->name << "];\n";
			ss << "\t\t\t\t" << itname << " = " << preitname << "->second.find(temp->" << iit->name << ");\n";
			ss << "\t\t\t" << "}\n";
		}
		ss << "\n";
		tttt++;
	}
	ss << "\t\t\t" << "result.pResult.nextRow();\n";
	ss << "\t\t}\n";
	ss << "\t\t" << "return result.pResult.GetResult();\n";
	ss << "\t}\n";
}

void writetable_get(std::stringstream& ss, ClassItem&item)
{
	int ccc = 1;
	for (auto it = item.keygroup.begin(); it != item.keygroup.end(); ++it)
	{
		auto temp = item.keygroupname[it->first];
		int count = 1;
		vector<string> param_type;
		vector<string> param_name;
		string param;
		for (auto iit = it->second.begin(); iit != it->second.end(); ++iit)
		{
			vector<string> pre_param_type = param_type;
			vector<string> pre_param_name = param_name;
			if (count != 1)
				param += ",";
			string type = iit->type;
			if (type != "int64" && type != "int32" && type != "char")
			{
				type = "const ";
				type += iit->type;
				type += "&";
			}
			param += type;
			param_type.push_back(type);
			param += " ";
			param += iit->name;
			param_name.push_back(iit->name);
			string ret = temp.size() - count > 0 ? temp[temp.size() - count - 1] : item.name;
			ss << "\t////////////////////////////////////////////////////////////////////////////\n";
			ss << "\t" << ret << "* Get" << ccc << "(" << param << ")\n";
			ss << "\t{\n";
			if (param_type.size() == 1)
			{
				ss << "\t\tauto pData = &mappings_" << ccc << ";\n";
			}
			else
			{
				ss << "\t\tauto pData = Get" << ccc << "(";
				for (size_t xx = 0; xx < param_name.size() - 1; xx++)
				{
					if (xx != 0)
						ss << ", ";
					ss << param_name[xx];
				}
				ss << ");\n";
				ss << "\t\tif(pData == nullptr)\n";
				ss << "\t\t\t return nullptr;\n\n";
			}
			ss << "\t\tauto it = pData->find(" << param_name[param_name.size() - 1] << ");\n";
			ss << "\t\tif(it == pData->end())\n";
			ss << "\t\t\treturn nullptr;\n\n";
			if (param_name.size() != it->second.size())
				ss << "\t\treturn &it->second;\n";
			else
				ss << "\t\treturn it->second;\n";

			ss << "\t}\n";

			count++;
		}
		ccc++;
	}
}

void writetable_getmap(std::stringstream& ss, ClassItem&item)
{
	int zzz = 1;
	for (auto it = item.keygroupname.begin(); it != item.keygroupname.end(); ++it)
	{
		ss << "\t//////////////////////////////////////////////////////////////////////////\n";
		ss << "\t" << (it->second)[it->second.size() - 1] << " &GetMappings" << zzz << "(void)\n";
		ss << "\t{\n";
		ss << "\t\treturn mappings_" << zzz << ";\n";
		ss << "\t}\n";
		zzz++;
	}
}

void writetable_param(std::stringstream& ss, ClassItem&item)
{
	ss << "\tstd::vector<" << item.name << "*>" << "\t\t data_;\n";
	int countt = 1;
	for (auto it = item.keygroupname.begin(); it != item.keygroupname.end(); ++it)
	{
		ss << "\t" << (it->second)[it->second.size() - 1] << "\t\t" << "mappings_" << countt << ";\n";
		countt++;
	}
}

void writetable(std::stringstream& ss, ClassItem&item)
{
	ss << "class " << item.name << "Table : public Singleton<"<<item.name<<"Table>\n";
	ss << "{\n";
	ss << "public:\n";

	writetable_release(ss, item);
	writetable_destructor(ss, item);

	//加载数据
	writetable_load(ss, item);
	writetable_get(ss, item);

	ss << "\t//////////////////////////////////////////////////////////////////////////\n";
	ss << "\t" << item.name << "* At(int index)\n";
	ss << "\t{\n";
	ss << "\t\treturn data_[index];\n";
	ss << "\t}\n";
	ss << "\t//////////////////////////////////////////////////////////////////////////\n";
	ss << "\tint GetSize(void)\n";
	ss << "\t{\n";
	ss << "\t\treturn data_.size();\n";
	ss << "\t}\n";
	ss << "\t//////////////////////////////////////////////////////////////////////////\n";
	ss << "\tstd::vector<" << item.name << "*> &GetData(void)\n";
	ss << "\t{\n";
	ss << "\t\treturn data_;\n";
	ss << "\t}\n";

	writetable_getmap(ss, item);

	ss << "private:\n";
	writetable_param(ss, item);
	ss << "};\n";
}

void writetype(ClassItem& item)
{
	std::stringstream ss;
	ss << "#pragma once\n\n";
	ss << "#include \"Application.h\"\n";
	ss << "namespace TABLE{\n";
	writeclass(ss, item);
	writetable(ss, item);

	ss << "}\n";

	ss << "TABLE::" << item.name << "Table* Singleton<TABLE::" << item.name << "Table>::single = nullptr;\n";
	ss << "#define s_" << item.name << "Table TABLE::" << item.name << "Table::getInstance()\n";

	string filename = "table/";
	filename += item.name;
	filename += ".h";
#ifdef _WIN32
	FILE * m_hFile = _fsopen(filename.c_str(), "w+", _SH_DENYWR);
#else
	m_hFile = fopen(filename.c_str(), "a+");
#endif

	fwrite(ss.str().c_str(), ss.str().length(), 1, m_hFile);
	fflush(m_hFile);
	fclose(m_hFile);
}

void loadtype()
{
	xvector<xstring> file;
	proto::auto_load_proto("", "*.type", file);

	for (size_t i = 0; i < file.size(); i++)
	{
		ptree pt;
		string str = file[i];

		try
		{
			read_xml(str, pt);
		}
		catch (std::exception& e)
		{
			cout << e.what();
		}

		//ptree classes = pt.get<ptree>("config");
		vector<ClassItem> clss;
		for (ptree::iterator it = pt.begin(); it != pt.end(); ++it)
		{
			if (it->first == "classes")
			{
				loadclasses(it->second, clss);
			}
		}


		for (size_t i = 0; i < clss.size(); i++)
		{
			writetype(clss[i]);
		}
	}


}

struct DBItem
{
	string table_name;
	string class_name;
	string proto_name;
	string primary_key;

	map<string, string> dbtype;

	string insert_id;
	string update_id;
	string delete_id;
	string request_id;
	string response_id;
	string request_proto;
	string response_proto;
	string request_key;
};

void loaddbs(ptree classes, vector<DBItem>& clss)
{
	for (ptree::iterator subit = classes.begin(); subit != classes.end(); ++subit)
	{
		std::cout << subit->first;
		std::cout << "\n";

		if (subit->first != "db")
			continue;

		ptree clas = subit->second;

		DBItem item;
		item.table_name = clas.get<string>("<xmlattr>.table_name", "");
		if (item.table_name.empty())
		{
			std::cout << "db table_name empty\n";
			return;
		}

		item.class_name = clas.get<string>("<xmlattr>.class_name", "");
		if (item.class_name.empty())
		{
			std::cout << "db class_name empty\n";
			return;
		}

		item.proto_name = clas.get<string>("<xmlattr>.proto_name", "");
		if (item.proto_name.empty())
		{
			std::cout << "db proto_name empty\n";
			return;
		}
		item.primary_key = clas.get<string>("<xmlattr>.primary_key", "");
		if (item.primary_key.empty())
		{
			std::cout << "db table_name empty\n";
			return;
		}

		ptree members = clas.get_child("members", ptree());
		ptree members_child = members.get_child("");

		for (ptree::iterator members_childit = members_child.begin(); members_childit != members_child.end(); ++members_childit)
		{
			if (members_childit->first == "<xmlattr>")
				continue;
			if (members_childit->first == "<xmlcomment>")
				continue;

			if (members_childit->first == "member")
			{
				item.dbtype[members_childit->second.get<string>("<xmlattr>.name", "")] = 
					members_childit->second.get<string>("<xmlattr>.dbtype", "");
			}

		}

		ptree protos = clas.get_child("protos", ptree());
		ptree protos_child = protos.get_child("");
		for (ptree::iterator protosit = protos_child.begin(); protosit != protos_child.end(); ++protosit)
		{
			if (protosit->first == "<xmlattr>")
				continue;
			if (protosit->first == "<xmlcomment>")
				continue;

			if (protosit->first != "proto")
				continue;

			string name = protosit->second.get<string>("<xmlattr>.name", "");
			if (name == "insert")
			{
				item.insert_id = protosit->second.get<string>("<xmlattr>.req_id", "");
				if (item.insert_id.empty())
				{
					std::cout << "db insert req_id empty\n";
					return;
				}
			}

			if (name == "update")
			{
				item.update_id = protosit->second.get<string>("<xmlattr>.req_id", "");
				if (item.update_id.empty())
				{
					std::cout << "db update req_id empty\n";
					return;
				}
			}

			if (name == "delete")
			{
				item.delete_id = protosit->second.get<string>("<xmlattr>.req_id", "");
				if (item.delete_id.empty())
				{
					std::cout << "db delete req_id empty\n";
					return;
				}
			}

			if (name == "query")
			{
				item.request_id = protosit->second.get<string>("<xmlattr>.req_id", "");
				if (item.request_id.empty())
				{
					std::cout << "db query req_id empty\n";
					return;
				}

				item.request_proto = protosit->second.get<string>("<xmlattr>.req_name", "");
				if (item.request_proto.empty())
				{
					std::cout << "db query req_name empty\n";
					return;
				}

				item.response_id = protosit->second.get<string>("<xmlattr>.response_id", "");
				if (item.response_id.empty())
				{
					std::cout << "db query response_id empty\n";
					return;
				}

				item.response_proto = protosit->second.get<string>("<xmlattr>.response_name", "");
				if (item.response_proto.empty())
				{
					std::cout << "db query response_name empty\n";
					return;
				}

				item.request_key = protosit->second.get<string>("<xmlattr>.key", "");
				if (item.request_key.empty())
				{
					std::cout << "db query key empty\n";
					return;
				}
			}
		}

		clss.push_back(item);
	}
}

void writedbs_h(DBItem&item)
{
	std::stringstream ss;

	ss << "#pragma once\n\n";

	ss << "class MysqlStmt;\n";
	ss << "struct Stmt;\n";
	ss << "struct StmtExData;\n\n";

	ss << "class " << item.class_name << "\n";
	ss << "{\n";
	ss << "public:\n";
	ss << "\t" << item.class_name << "();\n";
	ss << "\t" << "~" << item.class_name << "();\n";
	ss << "\t" << "void registerMessage();\n";

	ss << "private:\n";
	ss << "\t" << "void processInsert(PackPtr& pPack);\n";
	ss << "\t" << "void processUpdate(PackPtr& pPack);\n";
	ss << "\t" << "void processDelete(PackPtr& pPack);\n";
	ss << "\t" << "void processRequest(PackPtr& pPack);\n";
	ss << "\t" << "void cbRequest(StmtExData* result, ConnectPtr& conn, int32 playerid);\n";
	ss << "};\n";

	string filename = "dbs/";
	filename += item.class_name;
	filename += ".h";
#ifdef _WIN32
	FILE * m_hFile = _fsopen(filename.c_str(), "w+", _SH_DENYWR);
#else
	m_hFile = fopen(filename.c_str(), "a+");
#endif

	fwrite(ss.str().c_str(), ss.str().length(), 1, m_hFile);
	fflush(m_hFile);
	fclose(m_hFile);
}

void writedbs_cpp(DBItem&item)
{
	std::stringstream ss;
	ss << "#include \"stdafx.h\"\n";
	ss << "#include \"Application.h\"\n";
	ss << "#include \"" << item.class_name << ".h\"\n\n";

	ss << item.class_name << "::" << item.class_name << "()\n";
	ss << "{\n";
	ss << "\t" << "registerMessage();\n";
	ss << "}\n";

	ss << item.class_name << "::~" << item.class_name << "()\n";
	ss << "{\n\n";
	ss << "}\n";

	ss << "void " << item.class_name << "::registerMessage()\n";
	ss << "{\n";
	ss << "\t" << "sNetService.RegisterMessage(" << item.insert_id << ", std::bind(&" << 
		item.class_name << "::processInsert, this, std::placeholders::_1));\n";
	ss << "\t" << "sNetService.RegisterMessage(" << item.update_id << ", std::bind(&" <<
		item.class_name << "::processUpdate, this, std::placeholders::_1));\n";
	ss << "\t" << "sNetService.RegisterMessage(" << item.delete_id << ", std::bind(&" <<
		item.class_name << "::processDelete, this, std::placeholders::_1));\n";
	ss << "\t" << "sNetService.RegisterMessage(" << item.request_id << ", std::bind(&" <<
		item.class_name << "::processRequest, this, std::placeholders::_1));\n";
	ss << "}\n";

	ss << "void " << item.class_name << "::processInsert(PackPtr& pPack)\n";
	ss << "{\n";
	ss << "\t" << item.proto_name << " notify;\n";
	ss << "\t" << "CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));\n\n";
	xmap<xstring, proto::proto_data>::iterator it = proto::_proto_data_mgr.find(item.proto_name);
	if (it == proto::_proto_data_mgr.end())
	{
		std::cout << "could not find proto " << item.proto_name << "\n";
		return;
	}

	ss << "\t" << "std::string sql = \"insert into " << item.table_name << "(";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		ss << it->second[i].name;
		if (i != it->second.size() - 1)
		{
			ss << ",";
		}
	}
	ss << ") values(";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		ss << "?" << it->second[i].name;
		if (i != it->second.size() - 1)
		{
			ss << ",";
		}
	}
	ss << ")\";\n\n";

	ss << "\t" << "StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());\n";

	for (size_t i = 0; i < it->second.size(); i++)
	{
		auto subit = item.dbtype.find(it->second[i].name);
		if (subit != item.dbtype.end())
		{
			if (subit->second == "datetime")
				ss << "\t\t" << "pBind->SetDate(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
			continue;
		}

		if (it->second[i].type == "int32")
		{
			ss << "\t" << "pBind->SetInt32(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "int64")
		{
			ss << "\t" << "pBind->SetInt64(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "string")
		{
			ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "proto" || it->second[i].type == "proto_s" ||
			it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",JsonConvert::ProtoBufToJsonString(notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			//ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",StringJoin(notify." << it->second[i].name << "()));\n";
		}
	}
	ss << "\t" << "sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);\n";
	ss << "}\n\n";

	ss << "void " << item.class_name << "::processUpdate(PackPtr& pPack)\n";
	ss << "{\n";
	ss << "\t" << item.proto_name << " notify;\n";
	ss << "\t" << "CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));\n\n";
	ss << "\t" << "std::string sql = \"update " << item.table_name << " set ";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		if (it->second[i].name == item.primary_key)
			continue;

		ss << it->second[i].name << "=?" << it->second[i].name;
		if (i != it->second.size() - 1)
		{
			ss << ",";
		}
	}
	ss << "\";\n\tStmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());\n";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		auto subit = item.dbtype.find(it->second[i].name);
		if (subit != item.dbtype.end())
		{
			if (subit->second == "datetime")
				ss << "\t\t" << "pBind->SetDate(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
			continue;
		}

		if (it->second[i].type == "int32")
		{
			ss << "\t" << "pBind->SetInt32(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "int64")
		{
			ss << "\t" << "pBind->SetInt64(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "string")
		{
			ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "proto" || it->second[i].type == "proto_s" ||
			it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",JsonConvert::ProtoBufToJsonString(notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			//ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",StringJoin(notify." << it->second[i].name << "()));\n";
		}
	}
	ss << "\tsStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);\n";
	ss << "}\n\n";


	ss << "void " << item.class_name << "::processDelete(PackPtr& pPack)\n";
	ss << "{\n";
	ss << "\t" << item.proto_name << " notify;\n";
	ss << "\t" << "CHECKERRORANDRETURN(notify.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));\n\n";

	ss << "\t" << "std::string sql = \"delete from " << item.table_name << " where " << item.primary_key << " = ?" << item.primary_key << "\";\n";

	ss << "\t" << "StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());\n";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		if (it->second[i].name != item.primary_key)
			continue;

		auto subit = item.dbtype.find(it->second[i].name);
		if (subit != item.dbtype.end())
		{
			if (subit->second == "datetime")
				ss << "\t\t" << "pBind->SetDate(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
			continue;
		}

		if (it->second[i].type == "int32")
		{
			ss << "\t" << "pBind->SetInt32(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "int64")
		{
			ss << "\t" << "pBind->SetInt64(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "string")
		{
			ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",notify." << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "proto" || it->second[i].type == "proto_s" ||
			it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			ss << "\t" << "pBind->SetString(\"" << it->second[i].name << "\",JsonConvert::ProtoBufToJsonString(notify." << it->second[i].name << "());\n";
		}
	}
	ss << "\t" << "sStmt.AsynExecuteQueryVariable(sql, pBind, nullptr);\n";
	ss << "}\n\n";

	ss << "void " << item.class_name << "::processRequest(PackPtr& pPack)\n";
	ss << "{\n";
	ss << "\t" << item.request_proto << " request;\n";
	ss << "\t" << "CHECKERRORANDRETURN(request.ParseFromArray(pPack->getBuffer().c_str(), pPack->getBufferSize()));\n";
	ss << "\t" << "std::string sql = \"select * from " << item.table_name << " where playerid = ?playerid\";\n";
	ss << "\t" << "StmtBindData* pBind = sStmt.PrepareQuery(sql.c_str());\n";
	ss << "\t" << "pBind->SetInt32(\"playerid\",request.playerid());\n";
	ss << "\t" << "sStmt.AsynExecuteQueryVariable(sql, nullptr, std::bind(&"<<item.class_name<<"::cbRequest, this, std::placeholders::_1, pPack->m_Connect, request.playerid()));\n";
	ss << "}\n\n";

	ss << "void " << item.class_name << "::cbRequest(StmtExData* result, ConnectPtr& conn ,int32 playerid)\n";
	ss << "{\n";
	ss << "\t" << "if (!result->GetResult())\n";
	ss << "\t" << "{\n";
	ss << "\t\t" << "std::cout << result->geterror() << \"\\n\";\n";
	ss << "\t\t" << "return;\n";
	ss << "\t" << "}\n\n";
	ss << "\t" << item.response_proto << " response;\n";
	ss << "\t" << "response.set_result(0);\n\n";
	ss << "\t" << "response.set_playerid(playerid);\n\n";
	ss << "\t" << "while (!result->eof())\n";
	ss << "\t" << "{\n";
	ss << "\t\t" << item.proto_name << "* pdata = response.";
	xmap<xstring, proto::proto_data>::iterator iit = proto::_proto_data_mgr.find(item.response_proto);
	if (iit == proto::_proto_data_mgr.end())
	{
		std::cout << "could not find proto " << item.response_proto << "\n";
		return;
	}
	if (iit->second[1].type == "proto")
		ss << "mutable_" << iit->second[1].name << "();\n";
	else if (iit->second[1].type == "proto_s")
		ss << "add_" << iit->second[1].name << "();\n";


	for (size_t i = 0; i < it->second.size(); i++)
	{
		auto subit = item.dbtype.find(it->second[i].name);
		if (subit != item.dbtype.end())
		{
			if (subit->second == "datetime")
				ss << "\t\t" << "pdata->set_" << it->second[i].name << "(result->GetDate(\"" << it->second[i].name << "\"));\n";
				continue;
		}

		if (it->second[i].type == "int32")
		{
			ss << "\t\t" << "pdata->set_" << it->second[i].name << "(result->GetInt32(\"" << it->second[i].name << "\"));\n";
		}

		if (it->second[i].type == "int64")
		{
			ss << "\t\t" << "pdata->set_" << it->second[i].name << "(result->GetInt64(\"" << it->second[i].name << "\"));\n";
		}

		if (it->second[i].type == "string")
		{
			ss << "\t\t" << "pdata->set_" << it->second[i].name << "(result->GetString(\"" << it->second[i].name << "\"));\n";
		}

		if (it->second[i].type == "proto" || it->second[i].type == "proto_s" ||
			it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			ss << "\t\tJsonConvert::JsonStringToProtoBuf(result->GetString(\"" << it->second[i].name << "\"),*pdata->mutable_" << it->second[i].name << "());\n";
		}

		if (it->second[i].type == "int32_s" || it->second[i].type == "int64_s" || it->second[i].type == "string_s")
		{
			//ss << "\t\tStringToRepeatedField(pdate->mutable_" << it->second[i].name << "(),result->GetString(\"" << it->second[i].name << "\"));\n";
		}
	}
	ss << "\t\t" << "result->nextRow();\n";
	ss << "\t" << "}\n\n";
	ss << "\t" << "conn->SendBuffer(" << item.response_id << ", response, 0);\n";
	ss << "}\n\n";

	string filename = "dbs/";
	filename += item.class_name;
	filename += ".cpp";
#ifdef _WIN32
	FILE * m_hFile = _fsopen(filename.c_str(), "w+", _SH_DENYWR);
#else
	m_hFile = fopen(filename.c_str(), "a+");
#endif

	fwrite(ss.str().c_str(), ss.str().length(), 1, m_hFile);
	fflush(m_hFile);
	fclose(m_hFile);
}

void writeentity(DBItem&item)
{
	std::stringstream ss;
	ss << "#pragma once\n\n";
	ss << "struct " << item.table_name << "Item\n";
	ss << "{\n";

	auto it = proto::_proto_data_mgr.find(item.proto_name);
	if (it == proto::_proto_data_mgr.end())
	{
		std::cout << "could not find " << item.proto_name;
		return;
	}
	for (size_t i = 0; i < it->second.size(); i++)
	{
		if (it->second[i].type == "string")
		{
			ss << "\t" << "xstring" << "\t\t\t\t" << it->second[i].name << ";\n";
		}
		else
		{
			ss << "\t" << it->second[i].type << "\t\t\t\t" << it->second[i].name << ";\n";
		}
	}                                                         
	ss << "\n\n";
	ss << "\t" << "void fromPBMessage(const "<<item.proto_name<<" &proto_item)\n";
	ss << "\t" << "{\n";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		ss << "\t\t" << it->second[i].name << " = proto_item." << it->second[i].name << "();\n";
	}
	ss << "\t" << "}\n";
	ss << "\t" << "void toPBMessage(" << item.proto_name << " &proto_item) const\n";
	ss << "\t" << "{\n";
	for (size_t i = 0; i < it->second.size(); i++)
	{
		ss << "\t\t" << "proto_item.set_" << it->second[i].name << "(" << it->second[i].name << "); \n";
	}
	ss << "\t" << "}\n";
	ss << "};\n";

	string filename = "dbs/";
	filename += item.table_name;
	filename += "_entity";
	filename += ".h";
#ifdef _WIN32
	FILE * m_hFile = _fsopen(filename.c_str(), "w+", _SH_DENYWR);
#else
	m_hFile = fopen(filename.c_str(), "a+");
#endif

	fwrite(ss.str().c_str(), ss.str().length(), 1, m_hFile);
	fflush(m_hFile);
	fclose(m_hFile);
}

void writedbs(DBItem& item)
{
	writedbs_h(item);
	writedbs_cpp(item);

	writeentity(item);
}

void loaddb()
{
	xvector<xstring> file;
	proto::auto_load_proto("", "*.db", file);

	for (size_t i = 0; i < file.size(); i++)
	{
		ptree pt;
		string str = file[i];

		try
		{
			read_xml(str, pt);
		}
		catch (std::exception& e)
		{
			cout << e.what();
		}

		//ptree classes = pt.get<ptree>("config");
		vector<DBItem> clss;
		for (ptree::iterator it = pt.begin(); it != pt.end(); ++it)
		{
			if (it->first == "dbs")
			{
				loaddbs(it->second, clss);
			}
		}

		for (size_t i = 0; i < clss.size(); i++)
		{
			writedbs(clss[i]);
		}
	}

}