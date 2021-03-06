// htmlTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "HtmlParser.h"

using namespace std;
using namespace liigo;

bool isNeed(const string &str)
{
	static vector<string> keys = {
		"Petri",
		"Invariants",
		"bounded",
	};
	for (vector<string>::const_iterator it = keys.begin(); it != keys.end(); it++)
	{
		if (str.find(*it) != string::npos) // 找到
			return true; 
	}
	return false;
}

/*******************************************
 * 去除前后缀空格('\n','\r','\t',' ')
 * after deleted, if str is empty,return true, else return false
 *******************************************/
bool isSpaceLine(string &str,ofstream &fout)
{
	int count = 0;
	// 删除前缀空格
	for (string::iterator it = str.begin(); it != str.end(); it++)
	{
		if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') count++;
		else break;
	}
	str.erase(0,count);
	// 删除后缀空格
	count = 0;
	for (string::const_reverse_iterator it = str.rbegin(); it != str.rend(); it++)
	{
		if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') count++;
		else break;
	}
	str.erase(str.length() - count);

	if (str.empty()) return true;
	else return false;
}

void process(const vector<string> fromHtml)
{
	vector<string> others;
	bool start = false;
	int count = 0;
	vector<string>::const_iterator it;
	///////////// T-Invariants
	multimap<string, int> Tinvariants;
	map<int,string> Tkeys;
	string str("T-Invariants");
	for ( it = fromHtml.begin(); it != fromHtml.end(); it++)
	{
		if (!start) {
			if(*it == str) start = true;
		    else others.push_back(*it);
		}
		else {
			if ((*it)[0] == 'T') {
				Tkeys.insert(pair<int,string>(count,*it));  count++; 
			}
			else break;
		}
	}
	int Tnum = count - 1; // T_invariants number
	cout << "Tnum =" << Tnum << endl;
	if (Tnum >= 1)
	{
		for (count = 0; it != fromHtml.end(); it++, count++)
		{
			if ((*it)[0]<'0' || (*it)[0]>'9') break; // 非'0'到'9'字符
			Tinvariants.insert(pair<string, int>(Tkeys[count], stoi(*it)));
			if (count >= Tnum) count = 0;
		}
	}
	///////////// P-Invariants
	multimap<string, int> Pinvariants;
	map<int, string> Pkeys;
	str = "P-Invariants";
	start = false; count = 0;
	for (; it != fromHtml.end(); it++)
	{
		if (!start) {
			if (*it == str) start = true;
			else others.push_back(*it);
		}
		else {
			if ((*it)[0] == 'P') { 
				Pkeys.insert(pair<int, string>(count, *it));  count++; 
			}
			else break;
		}
	}
	int Pnum = count - 1; // P_invariants number
	cout << "Pnum =" << Pnum << endl;
	if (Pnum >= 1)
	{
		for (count = 0; it != fromHtml.end(); it++, count++)
		{
			if ((*it)[0]<'0' || (*it)[0]>'9') break; // 非'0'到'9'字符
			Pinvariants.insert(pair<string, int>(Pkeys[count], stoi(*it)));
			if (count >= Pnum) count = 0;
		}
	}
	/////////// others
	for (; it != fromHtml.end(); it++)
	{
		others.push_back(*it);
	}
	/// view
	for (vector<string>::const_iterator it = others.begin(); it != others.end(); it++)
	{
		cout<<"###"<<*it<<endl;
	}
}

/**************************************
* 遍历所有节点（标签）
***************************************/
void traverse(const string htmlFile, const string outFile)
{
	vector<string> fromHtml;
	MemBuffer memHtml;
	if (!memHtml.loadFromFile(htmlFile.c_str()))
		return;
	ofstream fout(outFile);
	fout << "n,r,t====" << (int)'\n' << "," << (int)'\r' << "," << (int)'\t' << endl;

	HtmlParser htmlParser;
	htmlParser.parseHtml((const char*)memHtml.getData(), true);
	string petri = "Petri net invariant analysis results"; // 此文本及以后的文本，开始保留
	bool start = false; // 开始保留
	bool saved = false; // 是否保留
    int count = htmlParser.getHtmlNodeCount(); // 节点数
	for (int index = 0; index < count; index++)
	{
		HtmlNode* pNode = htmlParser.getHtmlNode(index);
		if (!start && pNode->text)
		{
			if (pNode->type != NODE_CONTENT) continue; // 非普通文本
			string str(pNode->text);
			if (str.find(petri, 0) != string::npos)
				start = true;
		}
		if (!start) continue;
		saved = false;
		// fout << "======" << pNode->tagName << "," << pNode->attributeCount << "," << pNode->tagType << endl;
		switch (pNode->type)
		{
		case NODE_CONTENT:
			// fout << "内容: 介于开始标签和/或结束标签之间的普通文本" << endl;
			saved = true;
			break;
		case NODE_START_TAG:
			//fout << "开始标签，如 <a href=\"liigo.com\"> 或 <br/>" << endl;
			break;
		case NODE_END_TAG:
			//fout << "结束标签，如 </a>" << endl;
			break;
		case NODE_REMARKS:
			//fout << "注释: <!-- -->" << endl;
			break;
		case NODE_NULL:
			//fout << "作为所有节点的终结标记" << endl;
		case _NODE_USER_:
			//fout << "用户定义的其他节点类型" << endl;
		case NODE_UNKNOWN:
		default:
			//fout << "未知的节点类型" << endl;
			break;
		}
		if (!saved) continue;
		if (pNode->text)
		{
			//cout << pNode->text << endl;
			//fout << pNode->text << endl;
			string str(pNode->text);
			if (!isSpaceLine(str, fout))
			{
				fout << "---" << str.length() << "===" << str << endl;
				fromHtml.push_back(str);
			}
		}
	}
	process(fromHtml);
	fout.close();
}

int main()
{
	string htmlFile = "Invariant Analysis.html";
	string outFile = "htmlout.txt";
	
	traverse(htmlFile, outFile);

	system("pause");
	return 0;
}

