#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "tinyxml2.h"
#include "HtmlHelper.h"
#include "XmlHelper.h"

using namespace tinyxml2;
using namespace std;

XmlHelper::XmlHelper()
{
}


XmlHelper::~XmlHelper()
{
}

/***************************************
 * 遍历<place> ... </place>
 * 成功返回true,否则返回false
 ****************************************/
bool XmlHelper::place(XMLElement* place, ofstream &fout)
{
    // place id
	const XMLAttribute* attr = place->FirstAttribute();
	if (!attr || strcmp(attr->Name(), "id") != 0) {
		std::cout << "Error format in <place id=" << place->Name() << ">" << endl;
		return false;
	}
	string placeId = attr->Value();  // place id
	Places.push_back(placeId);

	// initialMarking
	XMLElement* element = place->FirstChildElement("initialMarking")->FirstChildElement("value");
	if (!element) {
		std::cout << "Error format in <place id=" << placeId << "> <initialMarking> <value>" << endl;
		return false;
	}
	string str(element->GetText()); // eg. Default,10
	string::size_type pos = str.find_last_of(',');
	if (pos != string::npos) str = str.substr(pos + 1);
	initialMarking.insert(make_pair(placeId, atoi(str.c_str())));

	// capcity
	element = place->FirstChildElement("capacity")->FirstChildElement("value");
	if (!element) {
		std::cout << "Error format in <place id=" << placeId << ">  <capacity> <value>" << endl;
		return false;
	}
	str = element->GetText();
	capacity.insert(make_pair(placeId, atoi(str.c_str())));
	return true;
}

/***************************************
 * 遍历<transition> ... </transition>
 * 成功返回true,否则返回false
 ****************************************/
bool XmlHelper::transition(XMLElement* transition, ofstream &fout)
{
	// transition id
	const XMLAttribute* attr = transition->FirstAttribute();
	if (!attr || strcmp(attr->Name(), "id") != 0) {
		std::cout << "Error format in <transition id=" << transition->Name() << ">" << endl;
		return false;
	}
	string transitionId = attr->Value();  // transition id
	Transitions.push_back(transitionId);
	return true;
}

/***************************************
 * 遍历<arc> ... </arc>
 * 成功返回true,否则返回false
 ****************************************/
bool XmlHelper::arc(XMLElement* arc, ofstream &fout)
{
	// arc id, eg. "P10 to T2" source="P10" target="T2"
	const XMLAttribute* attr = arc->FirstAttribute();
	string arcId;
	while (attr)
	{
		if (strcmp(attr->Name(), "id") == 0) {
			arcId = attr->Value();
			break;
		}
		attr = attr->Next();
	}
	attr = arc->FirstAttribute();
	while (attr)
	{
		if (strcmp(attr->Name(), "source") == 0) {
			source.insert(make_pair(arcId, attr->Value()));
		}
		if (strcmp(attr->Name(), "target") == 0) {
			target.insert(make_pair(arcId, attr->Value()));
		}
		attr = attr->Next();
	}

	// weight
	XMLElement* element = arc->FirstChildElement("inscription")->FirstChildElement("value");
	if (!element) {
		std::cout << "Error format in <arc id=" << arcId << "> <inscription> <value>" << endl;
		return false;
	}
	string str(element->GetText()); // eg. Default,1
	string::size_type pos = str.find_last_of(',');
	if (pos != string::npos) str = str.substr(pos + 1);
	weight.insert(make_pair(arcId, atoi(str.c_str())));

	return true;
}

/**************************************
 * 遍历<net> 中的所有兄弟节点(SiblingElement) </net>
 * 成功返回true,否则返回false
 ***************************************/
bool XmlHelper::traverse(XMLElement *ele, ofstream &fout)
{
	XMLElement* element = ele->FirstChildElement("net")->FirstChildElement("place");
	do
	{
		if (strcmp(element->Name(), "place") == 0) {
			if (!place(element, fout)) return false;
		}
		if (strcmp(element->Name(), "transition") == 0) {
			if (!transition(element, fout)) return false;
		}
		if (strcmp(element->Name(), "arc") == 0) {
			if (!arc(element, fout)) return false;
		}
	} while (element = element->NextSiblingElement());
	return true;
}

// 显示和记录PIPE输入xml文件的解析结果信息
void XmlHelper::view(ofstream &fout)
{
	std::cout << "Places:" << endl;
	fout << "Places:" << endl;
	Tools::vectorPrint(Places);
	Tools::vectorPrint(Places, fout);
	std::cout << "Transitions:" << endl;
	fout << "Transitions:" << endl;
	Tools::vectorPrint(Transitions);
	Tools::vectorPrint(Transitions, fout);
	std::cout << "initialMarking:" << endl;
	fout << "initialMarking:" << endl;
	for (map<string, int>::const_iterator it = initialMarking.begin(); it != initialMarking.end(); it++)
	{
		std::cout << it->first << "(" << it->second << "),";
		fout << it->first << "(" << it->second << "),";
	}
	std::cout << endl;
	fout << endl;
	std::cout << "capacity:" << endl;
	fout << "capacity:" << endl;
	for (map<string, int>::const_iterator it = capacity.begin(); it != capacity.end(); it++)
	{
		std::cout << it->first << "(" << it->second << "),";
		fout << it->first << "(" << it->second << "),";
	}
	std::cout << endl;
	fout << endl;
	std::cout << "arc:" << endl;
	fout << "arc:" << endl;
	for (map<string, string>::const_iterator it = source.begin(); it != source.end(); it++)
	{
		std::cout << "id=" << it->first << " source=" << it->second << " target=" << target[it->first]
			<< ",weight=" << weight[it->first] << endl;
		fout << "id=" << it->first << " source=" << it->second << " target=" << target[it->first]
			<< ",weight=" << weight[it->first] << endl;
	}
}

// 清除数据成员
void XmlHelper::clear()
{
	Places.clear();
	Transitions.clear();
	initialMarking.clear();
	capacity.clear();
	source.clear();
	target.clear();
	weight.clear();
}

/**************************************
 * 读取PIPE输入的.xml文件，解析后生成文本文件
 * 参数：
 * const string& xmlFile:  PIPE输入的.xml文件
 * const string& outFile:  解析后生成文本文件
 * 成功返回true，否则返回false
 ***************************************/
bool XmlHelper::readXml(const string& xmlFile, const string &outFile)
{
	XMLDocument doc;
	doc.LoadFile(xmlFile.c_str());
	if (doc.Error())
	{
		std::cout << "\n=== Error!! read " << xmlFile << " === " << endl;
		doc.PrintError();
		return false;
	}
	clear();
	ofstream fout(outFile);
	XMLElement* pnml = doc.FirstChildElement("pnml");
	if(!traverse(pnml, fout)) { 
		std::cout << "\n=== Error!! read " << xmlFile << " === " << endl;
		doc.PrintError();
		fout.close(); 
		return false; 
    }
	view(fout);
	fout.close();
	std::cout << "\n=== ok read " << xmlFile << " === " << endl;
	std::cout << "\n=== ok save " << outFile << " === " << endl;
	return true;
}

/****************************************************
 * save place element, ignore <graphics> 
 * <place id="P1">
 *   <graphics>
 *      <position x="285.0" y="315.0"/>
 *   </graphics>
 *   <name>
 *     <value>P1</value>
 *     <graphics>
 *        <offset x="0.0" y="0.0"/>
 *     </graphics>
 *   </name>
 *   <initialMarking>
 *      <value>Default,10</value>
 *      <graphics>
 *        <offset x="0.0" y="0.0"/>
 *      </graphics>
 *   </initialMarking>
 *   <capacity>
 *      <value>0</value>
 *   </capacity>
 *  </place>
 ****************************************************/
void XmlHelper::place(XMLDocument &doc, XMLNode* net)
{
	XMLElement* place;
	XMLElement* name;
	XMLElement* initMarking; // initialMarking
	XMLElement* value;
	XMLElement* cap; // capacity

	for (vector<string>::const_iterator it = Places.begin(); it != Places.end(); it++)
	{
		// place element
		place = doc.NewElement("place");
		place->SetAttribute("id", it->c_str());
		// name
		name = doc.NewElement("name");
		value = doc.NewElement("value");
		value->SetText(it->c_str());
		name->InsertEndChild(value);
		place->InsertEndChild(name);
		// initialMarking
		initMarking = doc.NewElement("initialMarking");
		value = doc.NewElement("value");
		string str = "Default," + to_string(initialMarking[*it]);
		value->SetText(str.c_str());
		initMarking->InsertEndChild(value);
		place->InsertEndChild(initMarking);
		// capacity 
		cap = doc.NewElement("capacity");
		value = doc.NewElement("value");
		value->SetText(to_string(capacity[*it]).c_str());
		cap->InsertEndChild(value);
		place->InsertEndChild(cap);

		net->InsertEndChild(place);
	}
}

/**********************************************
 * save transition element, ignore <graphics> etc. 
 * <transition id="T1">
 *   <graphics>
 *      <position x="375.0" y="135.0"/>
 *   </graphics>
 *   <name>
 *     <value>T1</value>
 *     <graphics>
 *        <offset x="-5.0" y="35.0"/>
 *     </graphics>
 *   </name>
 *   <orientation>
 *      <value>90</value>
 *   </orientation>
 *   <rate>
 *      <value>1.0</value>
 *   </rate>
 *   <timed>
 *      <value>false</value>
 *   </timed>
 *   <infiniteServer>
 *      <value>false</value>
 *   </infiniteServer>
 *   <priority>
 *      <value>1</value>
 *   </priority>
 * </transition>
 **********************************************/
void XmlHelper::transition(XMLDocument &doc, XMLNode* net)
{
	XMLElement* transition;
	XMLElement* name;
	XMLElement* value;
	XMLElement* priority;

	for (vector<string>::const_iterator it = Transitions.begin(); it != Transitions.end(); it++)
	{
		// transition element
		transition = doc.NewElement("transition");
		transition->SetAttribute("id", it->c_str());
		// name
		name = doc.NewElement("name");
		value = doc.NewElement("value");
		value->SetText(it->c_str());
		name->InsertEndChild(value);
		transition->InsertEndChild(name);
		// priority
		priority = doc.NewElement("priority");
		value = doc.NewElement("value");
		value->SetText("1");
		priority->InsertEndChild(value);
		transition->InsertEndChild(priority);

		net->InsertEndChild(transition);
	}
}

/**********************************************
 * save arc element, ignore <graphics> etc.
 * <arc id="P1 to T1" source="P1" target="T1">
 *   <graphics/>
 *   <inscription>
 *      <value>Default,1</value>
 *      <graphics/>
 *   </inscription>
 *   <tagged>
 *      <value>false</value>
 *   </tagged>
 *   <arcpath id="000" x="297" y="312" curvePoint="false"/>
 *   <arcpath id="001" x="297" y="147" curvePoint="false"/>
 *   <arcpath id="002" x="371" y="147" curvePoint="false"/>
 *   <type value="normal"/>
 * </arc>
 **********************************************/
void XmlHelper::arc(XMLDocument &doc, XMLNode* net)
{
	XMLElement* arc;
	XMLElement* inscription;
	XMLElement* value;
	XMLElement* tagged;
	XMLElement* type;

	for (map<string,string>::const_iterator it = source.begin(); it != source.end(); it++)
	{
		// arc element
		arc = doc.NewElement("arc");
		arc->SetAttribute("id", it->first.c_str());
		arc->SetAttribute("source", source[it->first].c_str());
		arc->SetAttribute("target", target[it->first].c_str());
		// inscription
		inscription = doc.NewElement("inscription");
		value = doc.NewElement("value");
		string str = "Default," + to_string(weight[it->first]);
		value->SetText(str.c_str());
		inscription->InsertEndChild(value);
		arc->InsertEndChild(inscription);
		// tagged
		tagged = doc.NewElement("tagged");
		value = doc.NewElement("value");
		value->SetText("false");
		tagged->InsertEndChild(value);
		arc->InsertEndChild(tagged);
		// type
		type = doc.NewElement("type");
		type->SetAttribute("value", "normal");
		arc->InsertEndChild(type);

		net->InsertEndChild(arc);
	}
}

/**************************************
 * 根据数据成员，生成xml文件
 * const string& outXml:  xml文件
 * 成功返回true，否则返回false
 ***************************************/
bool XmlHelper::saveXml(const string &outXml)
{
	//const char* declaration = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	const char* declaration = "xml version=\"1.0\" encoding=\"ISO-8859-1\"";
	const char* comment = "PIPI xml file";
	XMLDocument doc;
	XMLNode* dec1 = doc.NewDeclaration(declaration);
	XMLNode* comment1 = doc.NewComment(comment);
	XMLNode* pnml = doc.NewElement("pnml");
	XMLNode* net = doc.NewElement("net");
	// <net id = "Net-One" type = "P/T net">
	XMLElement* net_ele = net->ToElement();
	net_ele->SetAttribute("id", "Net-One");
	net_ele->SetAttribute("type", "P/T net");

	doc.InsertFirstChild(dec1);
	doc.InsertAfterChild(dec1, comment1);
	doc.InsertEndChild(pnml);
	pnml->InsertEndChild(net);

	// <token id="Default" enabled="true" red="0" green="0" blue="0"/>
	XMLElement* token = doc.NewElement("token");
	token->SetAttribute("id", "Default");
	token->SetAttribute("enabled", "true");
	token->SetAttribute("red", "0");
	token->SetAttribute("green", "0");
	token->SetAttribute("blue", "0");
	net->InsertEndChild(token);

	// save place element, ignore <graphics>
	place(doc,net);
	// save transition element, ignore <graphics> etc. 
	transition(doc, net);
	// save arc element, ignore <graphics> etc. 
	arc(doc, net);

	doc.Print();
	
	if (doc.SaveFile(outXml.c_str()) != 0) {
		std::cout << "Error!! save " << outXml << endl;
		doc.PrintError();
		return false;
	}
	std::cout << "\n=== ok save " << outXml << " === " << endl;
	return true;
}

/***************************************
 * 每行三个部分字符串
 * string parts[3]; // 3部分字符串，P = parts[0], M = parts[2], Pre,Post = parts[3];
 ***************************************/
bool XmlHelper::parts3(const string &line,string parts[])
{
	bool pre_space = true; // 前导空格
	string::size_type index = 0, start = 0, count = 0;
	int i = 0; // 三部分字符串索引
	for (string::const_iterator it = line.begin(); it != line.end(); it++)
	{
		if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') 
		{
			if (!pre_space) {
				parts[i] = line.substr(start, count); // 第1,2部分
				i++;
				count = 0;
				pre_space = true;
			}
		}
		else 
		{
			if (pre_space) {
				pre_space = false;
				start = index;
			}
			// 第三部分：从index取至末尾
			if (!pre_space && i == 2) {
				parts[i] = line.substr(start);
				break; // 已经取出三部分
			}
			count++;
		}
		index++;
	}
	if (i == 2) return true;
	else return false;
}

/**********************************************************
 * 分析pre,post, 填充数据成员: Transitions,source,target,weight
 * pre,post: eg. 1:3 4 5 6:7 注：程序解释要求':'前后无空格
 * 参数：
 * const string &place：  pre,post行的place 
 * const string &prePost  pre/post string
 * bool pre: true表示prePost是pre, false表示prePost是post
 ************************************************************/
bool XmlHelper::prePost(const string &place, const string &prePost, bool pre)
{
	bool pre_space = true; // 前导空格
	string::size_type index = 0, start = 0, count = 0;
	vector<string> parts; // 用空格隔开的字符串，eg. 1:2,3,4:5,6:7
	for (string::const_iterator it = prePost.begin(); it != prePost.end(); it++)
	{
		if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ')
		{
			if (!pre_space) {
				parts.push_back(prePost.substr(start, count)); // 有效子串
				count = 0;
				pre_space = true;
			}
		}
		else 
		{
			if (pre_space) {
				pre_space = false;
				start = index;
			}
			// 最后一个有效子串
			if (!pre_space && it == prePost.end() - 1) {
				parts.push_back(prePost.substr(start)); 
				break;
			}
			count++;
		}
		index++;
	}
	
	// 填充数据成员: Transitions,source,target,weight
	for (vector<string>::const_iterator it = parts.begin(); it != parts.end(); it++)
	{
		string::size_type pos;
		string transition;
		string weightStr;
		pos = (*it).find(":");
		if (pos != string::npos) {
			transition = (*it).substr(0, pos);
			weightStr = (*it).substr(pos + 1);
		}
		else {
			transition = *it;
			weightStr = "1";
		}
		// 添加T1,T2,...
		string T("T");
		T.append(transition); // 形成T1，T2，...
		vector<string>::const_iterator it1 = Transitions.begin();
		for (; it1 != Transitions.end(); it1++) if (*it1 == T) break;
		if (it1 == Transitions.end()) Transitions.push_back(T); // 不存在，添加之。
		// arc
		string arcId;
		if (pre) {
			arcId.append(T).append(" to ").append(place);
			source.insert(make_pair(arcId, T));
			target.insert(make_pair(arcId, place));
		}
		else {
			arcId.append(place).append(" to ").append(T);
			source.insert(make_pair(arcId, place));
			target.insert(make_pair(arcId, T));
		}
		weight.insert(make_pair(arcId, atoi(weightStr.c_str())));
	}
	return true;
}

/*****************************************
 * pnt to PIPE xml file
 * const string& pntFile: input pnt file
 * const string& outXml: output xml file和outXml.txt文件(供查看验证)
 * 成功返回true，否则返回false
 *
 * pnt file format: (程序解释要求':'前后无空格)
 * P   M   PRE,POST     
 * 0 2     4:2 5 6, 1:2 2 3
 * 1 0     1, 4
 * 2 0     2, 5
 * 3 0     3, 6
 * 4 1     4, 1
 * 5 1     5, 2
 * 6 1     6, 3
 * @
 *****************************************/
bool XmlHelper::pnt2xml(const string &pntFile, const string &outXml)
{
	bool ret = true;
	ifstream fin(pntFile);
	if (!fin) {
		std::cout << "Error!! read " << pntFile << endl;
		return false;
	}
	// 清除数据成员
	clear();

	// 读取pnt文件，填充数据成员
	string line; // 每行字符串
	int lineNum = 0;
	while (!fin.eof())
	{
		lineNum++;
		getline(fin, line);
		// std::cout << line << endl;
		if (lineNum == 1)  continue;               // first line: eg. P   M   PRE,POST 
		if (line.find('@') != string::npos) break; // end flag

		// 每行三个部分字符串
		string parts[3]; // 3部分字符串，P = parts[0], M = parts[2], Pre,Post = parts[3];
		if (!parts3(line, parts)) { ret = false;	break; }
		
		// pre,post
		string::size_type pos;
		string pre;
		string post;
		pos = parts[2].find(',');
		if (pos == string::npos) { ret = false;	break; }
		pre = parts[2].substr(0,pos);
		post = parts[2].substr(pos + 1);
	    
		// place 添加前缀'P',生成"P1","P2",...
		string place("P");
		place.append(parts[0]);
		// 处理pre,post
		prePost(place, pre, true);
		prePost(place, post, false);
		// 填充数据成员: place,initialMarking,capacity
		Places.push_back(place);
		initialMarking.insert(make_pair(place, atoi(parts[1].c_str())));
		capacity.insert(make_pair(place, 0)); // capacity = 0;
	}
	
	if (!ret) {
		std::cout << "Error format pnt file: " << pntFile << endl;
		fin.close();
		return false;
	}
	else {
		// 排序 Places,Transitions
		std::sort(Places.begin(), Places.end(), cmpByKey());
		std::sort(Transitions.begin(), Transitions.end(), cmpByKey());
	}

	// 根据数据成员，生成xml文件
	if (!saveXml(outXml)) {
		std::cout << "Error!! save " << outXml << endl;
		fin.close();
		return false;
	}

	// 生成outXml + ".txt",供查看验证
	ofstream fout(outXml + ".txt");
	view(fout);
	fout.close();

	std::cout << "\n=== ok read " << pntFile << " === " << endl;
	std::cout << "\n=== ok save " << outXml << " === " << endl;
	std::cout << "\n=== ok save " << outXml << ".txt" << " === " << endl;
	fin.close();
	return true;
}


