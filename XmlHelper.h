#pragma once
#include <string>
#include <vector>
#include <map>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

/*******************************************************************
 * pnt to PIPE xml file
 * 调用: bool pnt2xml(const string &pntFile, const string &outXml);
 * 读取PIPE输入的.xml文件，解析后生成文本文件
 * 调用: bool readXml(const string& xmlFile, const string &outFile);
 * 依赖下列文件：
 * tinyxml2.h
 * tinyxml2.cpp
 * Note: tinyxml2.cpp 编译设置（fopen函数使用）：
 * 项目->属性->配置属性->C/C++ ->预处理器->预处理器定义添加：_CRT_SECURE_NO_WARNINGS
 ********************************************************************/
class XmlHelper
{
public:
	XmlHelper();
	~XmlHelper();
	/**************************************
	 * 读取PIPE输入的.xml文件，解析后生成文本文件
	 * 参数：
	 * const string& xmlFile:  PIPE输入的.xml文件
	 * const string& outFile:  解析后生成文本文件
	 * 成功返回true，否则返回false
	 ***************************************/
	bool readXml(const string& xmlFile, const string &outFile);
    
	/***************************************
	 * pnt to PIPE xml file
	 * const string& pntFile: input pnt file
	 * const string& outXml: output xml file和outXml.txt文件(供查看验证)
	 * 成功返回true，否则返回false
	 ***************************************/
	bool pnt2xml(const string &pntFile, const string &outXml);

	// 根据source和target生成arcId, eg. "P10 to T2" source="P10" target="T2"
	string& createArcId(const string &source, const string &target, string &arcId) 
	{
		arcId.assign(source).append(" to ").append(target);
		return arcId;
	}

protected:
	/**************************************
	 * 根据数据成员，生成xml文件
	 * const string& outXml:  output xml文件
	 * 成功返回true，否则返回false
	 ***************************************/
	bool saveXml(const string &outXml);

	/**************************************
	 * 遍历<net> 中的所有兄弟节点(SiblingElement) </net>
	 * 成功返回true,否则返回false
	 ***************************************/
	bool traverse(XMLElement *ele, ofstream &fout);

	/***************************************
	 * 遍历<place> ... </place>
	 * 成功返回true,否则返回false
	 ****************************************/
	bool place(XMLElement* place, ofstream &fout);

	/***************************************
	 * 遍历<transition> ... </transition>
	 * 成功返回true,否则返回false
	 ***************************************/
	bool transition(XMLElement* place, ofstream &fout);

	/***************************************
	 * 遍历<arc> ... </arc>
	 * 成功返回true,否则返回false
	****************************************/
	bool arc(XMLElement* place, ofstream &fout);

	// 显示和记录PIPE输入xml文件的解析结果信息
	virtual void view(ofstream &fout);

	// 清除数据成员
	virtual void clear();

	// save place element, ignore <graphics> 
	void place(XMLDocument &doc, XMLNode* net);

	// save transition element, ignore <graphics> etc. 
	void transition(XMLDocument &doc, XMLNode* net);

	// save arc element, ignore <graphics> etc. 
	void arc(XMLDocument &doc, XMLNode* net);

	/***************************************
	 * 每行三个部分字符串
	 * string parts[3]; // 3部分字符串，P = parts[0], M = parts[2], Pre,Post = parts[3];
	 ***************************************/
	bool parts3(const string &line, string parts[]);

	/**********************************************************
	 * 分析pre,post, 填充数据成员: Transitions,source,target,weight
	 * pre,post: eg. 1:3 4 5 6:7 注：程序解释要求':'前后无空格
	 * 参数：
	 * const string &place：  pre,post行的place
	 * const string &prePost  pre/post string
	 * bool pre: true表示prePost是pre, false表示prePost是post
	 ************************************************************/
	bool prePost(const string &place, const string &prePost, bool pre);

	vector<string> Places;  // place id
	vector<string> Transitions; //transition id
	map<string, int> initialMarking;  // key: place id
	map<string, int> capacity;  // key: place id
	// key: arc id, eg. "P10 to T2" source="P10" target="T2"
	map<string, string> source; 
	// key: arc id, eg. "P10 to T2" source="P10" target="T2"
	map<string, string> target; 
	// key: arc id, eg. "P10 to T2" source="P10" target="T2"
	map<string, int> weight; 
};

