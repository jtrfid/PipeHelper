#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "HtmlParser.h"
#include "HtmlHelper.h"
#include "Tools.h"

using namespace std;
using namespace liigo;

HtmlHelper::HtmlHelper()
{
}

HtmlHelper::~HtmlHelper()
{
}

/**************************************
 * 读取PIPE输出的html文件，解析后生成文本文件
 * 参数：
 * const string& htmlFile: PIPE输出的Invariant Analysis.html文件
 * const string& outFile:  解析后生成文本文件
 * 成功返回true，否则返回false
 ***************************************/
bool HtmlHelper::InvariantsHtml(const string& htmlFile, const string &outFile)
{
	vector<string> fromHtml;
	if (!InvariantHtml(htmlFile, fromHtml)) return false;
	ofstream fout(outFile);
	if (!InvariantHtml(fromHtml))
	{
		std::cout << "\n=== Error!! read " << htmlFile << " === " << endl;
		fout.close();
		return false;
	}
	viewInvariants(fout);
	fout.close();
	std::cout << "\n=== ok read " << htmlFile << " === " << endl;
	std::cout << "\n=== ok save " << outFile << " === " << endl;
	return true;
}

/**************************************
 * 读取PIPE输出的Invariant Analysis.html文件
 * 参数输入:
 * const string& htmlFile: PIPE输出的html文件
 * 参数返回:
 * vector<string>& fromHtml: 去掉标签标记后的有效信息
 * 成功返回true，否则返回false
 ***************************************/
bool HtmlHelper::InvariantHtml(const string& htmlFile, vector<string>& fromHtml)
{
	MemBuffer memHtml;
	if (!memHtml.loadFromFile(htmlFile.c_str()))
	{
		std::cout << "不存在：" << htmlFile << endl;
		return false;
	}
	clear();
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
		// std::cout << "======" << pNode->tagName << "," << pNode->attributeCount << "," << pNode->tagType << endl;
		switch (pNode->type)
		{
		case NODE_CONTENT:
			// cout << "内容: 介于开始标签和/或结束标签之间的普通文本" << endl;
			saved = true;
			break;
		case NODE_START_TAG:
			//std::cout << "开始标签，如 <a href=\"liigo.com\"> 或 <br/>" << endl;
			break;
		case NODE_END_TAG:
			//std::cout << "结束标签，如 </a>" << endl;
			break;
		case NODE_REMARKS:
			//std::cout << "注释: <!-- -->" << endl;
			break;
		case NODE_NULL:
			//std::cout << "作为所有节点的终结标记" << endl;
		case _NODE_USER_:
			//std::cout << "用户定义的其他节点类型" << endl;
		case NODE_UNKNOWN:
		default:
			//std::cout << "未知的节点类型" << endl;
			break;
		}
		if (!saved) continue;
		if (pNode->text)
		{
			//std::cout << pNode->text << endl;
			string str(pNode->text);
			if (!Tools::isSpaceLine(str))
			{
				//std::cout << "---" << str.length() << "===" << str << endl;
				fromHtml.push_back(str);
			}
		}
	}
	return true;
}

/********************************************
 * 处理来自Invariant Analysis.html的文件
 * 参数：
 * formHtml去掉标签标记后的有效信息
 * 成功返回true，否则返回false
 *********************************************/
bool HtmlHelper::InvariantHtml(const vector<string> &fromHtml)
{
	bool ret = true;
	bool start = false;
	int count = 0;
	vector<string>::const_iterator it;
	///////////// T-Invariants
	map<string, int, cmpByKey> Tkeys; // value: index of the element in invariants
	string str("T-Invariants");
	for (it = fromHtml.begin(); it != fromHtml.end(); it++)
	{
		if (!start) {
			if ((*it).find(str) != string::npos) start = true;
			else others.push_back(*it);
		}
		else {
			if ((*it)[0] == 'T') {
				Tkeys.insert(pair<string, int>(*it, count));
				count++;
			}
			else break;
		}
	}
	// 记录至Tvector
	for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
	{
		Tvector.push_back(it->first);
	}
	int Tnum = count; // T_invariants number
	// std::cout << "Tnum =" << Tnum << endl;
	if (Tnum >= 1)
	{
		vector<int> invariants;
		int row = 0;
		for (count = 0; it != fromHtml.end(); it++, count++)
		{
			if ((*it)[0]<'0' || (*it)[0]>'9') break; // 非'0'到'9'字符
			invariants.push_back(stoi(*it));
			if (count == Tnum - 1) {
				// invariants[index] from Tkeys.second
				vector<int> invariants1;
				for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
				{
					invariants1.push_back(invariants[it->second]);
				}
				Tinvariants.insert(make_pair(row, invariants1));
				invariants.clear();
				count = -1;
				row++;
			}
		}
	}
	///////////// P-Invariants
	map<string, int, cmpByKey> Pkeys; // value: index of the element in invariants
	str = "P-Invariants";
	start = false; count = 0;
	for (; it != fromHtml.end(); it++)
	{
		if (!start) {
			if ((*it).find(str) != string::npos) start = true;
			else others.push_back(*it);
		}
		else {
			if ((*it)[0] == 'P') {
				Pkeys.insert(pair<string, int>(*it, count));
				count++;
			}
			else break;
		}
	}
	// 记录至Pvector
	for (map<string, int>::const_iterator it = Pkeys.begin(); it != Pkeys.end(); it++)
	{
		Pvector.push_back(it->first);
	}
	int Pnum = count; // P_invariants number
	//std::cout << "Pnum =" << Pnum << endl;

	if (Pnum >= 1)
	{
		vector<int> invariants;
		int row = 0;
		for (count = 0; it != fromHtml.end(); it++, count++)
		{
			if ((*it)[0]<'0' || (*it)[0]>'9') break; // 非'0'到'9'字符
			invariants.push_back(stoi(*it));
			if (count == Pnum - 1) {
				// invariants[index] from Pkeys.second
				vector<int> invariants1;
				for (map<string, int>::const_iterator it = Pkeys.begin(); it != Pkeys.end(); it++)
				{
					invariants1.push_back(invariants[it->second]);
				}
				Pinvariants.insert(make_pair(row, invariants1));
				invariants.clear();
				count = -1;
				row++;
			}
		}
	}
	/////////// others
	for (; it != fromHtml.end(); it++)
	{
		others.push_back(*it);
	}
	return ret;
}

// 显示和记录Invariant Analysis.html文件的解析结果信息
void HtmlHelper::viewInvariants(ofstream &fout)
{
	// others
	for (vector<string>::const_iterator it = others.begin(); it != others.end(); it++)
	{
		std::cout << "### " << *it << endl;
		fout << "### " << *it << endl;
	}

	std::cout << endl << "T-invariants(" << Tinvariants.size() << " x " << Tvector.size() << "):" << endl;
	fout << endl << "T-invariants(" << Tinvariants.size() << " x " << Tvector.size() << "):" << endl;
	for (vector<string>::const_iterator it = Tvector.begin(); it != Tvector.end(); it++)
	{
		if (it == Tvector.end() - 1) {
			std::cout << setw(4) << *it;
			fout << setw(4) << *it;
		}
		else {
			std::cout << setw(4) << *it << " ";
			fout << setw(4) << *it << " ";
		}
	}
	std::cout << endl;
	fout << endl;
	for (map<int, vector<int>>::const_iterator it = Tinvariants.begin(); it != Tinvariants.end(); it++)
	{
		for (vector<int>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
		{
			if (it1 == it->second.end() - 1) {
				std::cout << setw(4) << *it1;
				fout << setw(4) << *it1;
			}
			else {
				std::cout << setw(4) << *it1 << " ";
				fout << setw(4) << *it1 << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}
	std::cout << endl << "P-invariants(" << Pinvariants.size() << " x " << Pvector.size() << "):" << endl;
	fout << endl << "P-invariants(" << Pinvariants.size() << " x " << Pvector.size() << "):" << endl;
	for (vector<string>::const_iterator it = Pvector.begin(); it != Pvector.end(); it++)
	{
		if (it == Pvector.end() - 1) {
			std::cout << setw(4) << *it;
			fout << setw(4) << *it;
		}
		else {
			std::cout << setw(4) << *it << " ";
			fout << setw(4) << *it << " ";
		}
	}
	std::cout << endl;
	fout << endl;
	for (map<int, vector<int>>::const_iterator it = Pinvariants.begin(); it != Pinvariants.end(); it++)
	{
		for (vector<int>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
		{
			if (it1 == it->second.end() - 1) {
				std::cout << setw(4) << *it1;
				fout << setw(4) << *it1;
			}
			else {
				std::cout << setw(4) << *it1 << " ";
				fout << setw(4) << *it1 << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}
}

// 清除数据成员
void HtmlHelper::clear()
{
	Tvector.clear();
	Pvector.clear();
	Tinvariants.clear();
	Pinvariants.clear();
	others.clear();
	ForwardMatrix.clear();
	BackwardMatrix.clear();
	CombinedMatrix.clear();
	InhibitionMatrix.clear();
	Marking.clear();
	EnableTransitions.clear();
}

/**************************************
 * 读取PIPE输出的Incidence Matrix.html文件
 * 参数：
 * const string& htmlFile: PIPE输出的Incidence Matrix.html文件
 * const string& outFile:  解析后生成文本文件
 * 成功返回true，否则返回false
 ***************************************/
bool HtmlHelper::IncidencesHtml(const string& htmlFile, const string &outFile)
{
	vector<string> fromHtml;
	if (!IncidencesHtml(htmlFile, fromHtml)) return false;
	ofstream fout(outFile);
	if (!IncidencesHtml(fromHtml)) {
		std::cout << "\n=== Error!! read " << htmlFile << " == = " << endl;
		fout.close();
		return false;
	}
	viewIncidences(fout);
	fout.close();
	std::cout << "\n=== ok read " << htmlFile << " === " << endl;
	std::cout << "\n=== ok save " << outFile << " === " << endl;
	return true;
}

/**************************************
 * 读取PIPE输出的Incidence Matrix.html.html文件
 * 参数输入:
 * const string& htmlFile: PIPE输出的html文件
 * 参数返回:
 * vector<string>& fromHtml: 去掉标签标记后的有效信息
 * 成功返回true，否则返回false
 ***************************************/
bool HtmlHelper::IncidencesHtml(const string& htmlFile, vector<string>& fromHtml)
{
	MemBuffer memHtml;
	if (!memHtml.loadFromFile(htmlFile.c_str()))
	{
		std::cout << "不存在：" << htmlFile << endl;
		return false;
	}
	clear();
	HtmlParser htmlParser;
	htmlParser.parseHtml((const char*)memHtml.getData(), true);
	string petri = "Petri net incidence and marking"; // 此文本及以后的文本，开始保留
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
		// std::cout << "======" << pNode->tagName << "," << pNode->attributeCount << "," << pNode->tagType << endl;
		switch (pNode->type)
		{
		case NODE_CONTENT:
		    //std::cout << "内容: 介于开始标签和/或结束标签之间的普通文本" << endl;
			saved = true;
			break;
		case NODE_START_TAG:
			//std::cout << "开始标签，如 <a href=\"liigo.com\"> 或 <br/>" << endl;
			break;
		case NODE_END_TAG:
			//std::cout << "结束标签，如 </a>" << endl;
			break;
		case NODE_REMARKS:
			//std::cout << "注释: <!-- -->" << endl;
			break;
		case NODE_NULL:
			//std::cout << "作为所有节点的终结标记" << endl;
		case _NODE_USER_:
			//std::cout << "用户定义的其他节点类型" << endl;
		case NODE_UNKNOWN:
		default:
			//std::cout << "未知的节点类型" << endl;
			break;
		}
		if (!saved) continue;
		if (pNode->text)
		{
			//std::cout << pNode->text << endl;
			string str(pNode->text);
			if (!Tools::isSpaceLine(str))
			{
				//std::cout << "---" << str.length() << "===" << str << endl;
				fromHtml.push_back(str);
			}
		}
	}
	return true;
}

/********************************************
 * 处理来自Incidence Matrix.html的文件
 * 参数：
 * formHtml去掉标签标记后的有效信息
 * 成功返回true，否则返回false
 *********************************************/
bool HtmlHelper::IncidencesHtml(const vector<string> &fromHtml)
{
	bool ret = true;
	bool start = false;
	int count = 0;
	vector<string>::const_iterator it;
	///////////// Forwards incidence matrix I+
	map<string, int, cmpByKey> Tkeys; // value: count是 incidence matrix列index
									  // key = {"T1","T2",...};
	string str("Forwards incidence matrix");
	for (it = fromHtml.begin(); it != fromHtml.end(); it++)
	{
		if (!start) {
			if ((*it).find(str) != string::npos) {
				start = true; // 找到str, *(it + 1) = "I", *(it + 2) = "+"
				if (*(it+1) == "I") it++;
				else {
					std::cout << "Error format: Forwards incidence matrix I";
					return false;
				}
				if (*(it+1) == "+") it++;
				else {
					std::cout << "Error format: Forwards incidence matrix I+";
					return false;
				}
			}
			else others.push_back(*it); // 其他信息
		}
		else {
			if ((*it)[0] == 'T') {
				Tkeys.insert(pair<string, int>(*it, count)); // count是matrix列index
				count++;
			}
			else break;
		}
	}
	// 记录至Tvector = {"T1","T2",...}
	for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
	{
		Tvector.push_back(it->first);
	}
	int Tnum = count; // T numbe
	while (Tnum >= 1)
	{
		vector<int> ts;  // 对应T1,T2,...的int
		string place = *it; // P1，P2，....
		if (place[0] != 'P') break;
		it++;
		for (count = 0; count < Tnum; it++, count++)
		{
			if ((*it)[0]<'0' || (*it)[0]>'9') {
				std::cout << "Error format, " << *it << endl; // 非'0'到'9'字符
				return false;
			}
			ts.push_back(stoi(*it));
		}
		// ts[index] from Tkeys.second
		vector<int> ts1;
		for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
		{
			ts1.push_back(ts[it->second]);
		}
		ForwardMatrix.insert(make_pair(place, ts1));
	}
	
	///////////// Backwards incidence matrix I-
	str = "Backwards incidence matrix";
	if ((*it).find(str) != string::npos) {
		// 找到str, *(it + 1) = "I", *(it + 2) = "-"
	    if (*(it + 1) == "I") it++;
		else {
			std::cout << "Error format: Forwards incidence matrix I";
			return false;
		}
		if (*(it + 1) == "-") it++;
		else {
			std::cout << "Error format: Forwards incidence matrix I-";
			return false;
		}
		it++; // *it= "-"
	}
	else {
		std::cout << "Error format," << *it << endl;
		return false;
	}
	// T1,T2,....
	for (count = 0; count < Tnum; it++, count++)
	{
		if ((*it)[0] != 'T') {
			std::cout << "Error format," << *it << endl;
			return false;
		}
	}
	while (Tnum >= 1)
	{
		vector<int> ts;  // 对应T1,T2,...的int
		string place = *it; // P1，P2，....
		if (place[0] != 'P') break;
		it++;
		for (count = 0; count < Tnum; it++, count++)
		{
			if ((*it)[0]<'0' || (*it)[0]>'9') {
				std::cout << "Error format," << *it << endl; // 非'0'到'9'字符
				return false;
			}
			ts.push_back(stoi(*it));
		}
		// ts[index] from Tkeys.second
		vector<int> ts1;
		for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
		{
			ts1.push_back(ts[it->second]);
		}
		BackwardMatrix.insert(make_pair(place, ts1));
	}
    ///////////Combined incidence matrix I
	str = "Combined incidence matrix";
	if ((*it).find(str) == string::npos) {
		std::cout << "Error format," << *it << endl;
		return false;
	}
	else it += 2; // 略去 *it = "I"
	// T1,T2,....
	for (count = 0; count < Tnum; it++, count++)
	{
		if ((*it)[0] != 'T') {
			std::cout << "Error format," << *it << endl;
			return false;
		}
	}
	while (Tnum >= 1)
	{
		vector<int> ts;  // 对应T1,T2,...的int
		string place = *it; // P1，P2，....
		if (place[0] != 'P') break;
		it++;
		for (count = 0; count < Tnum; it++, count++) ts.push_back(stoi(*it));
		// ts[index] from Tkeys.second
		vector<int> ts1;
		for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
		{
			ts1.push_back(ts[it->second]);
		}
		CombinedMatrix.insert(make_pair(place, ts1));
	}
	////////////Inhibition matrix H
	str = "Inhibition matrix";
	if ((*it).find(str) == string::npos) {
		std::cout << "Error format," << *it << endl;
		return false;
	}
	else it += 2; // 略去*it = "H"
	// T1,T2,....
	for (count = 0; count < Tnum; it++, count++)
	{
		if ((*it)[0] != 'T') {
			std::cout << "Error format," << *it << endl;
			return false;
		}
	}
	while (Tnum >= 1)
	{
		vector<int> ts;  // 对应T1,T2,...的int
		string place = *it; // P1，P2，....
		if (place[0] != 'P') break;
		it++;
		for (count = 0; count < Tnum; it++, count++) ts.push_back(stoi(*it));
		// ts[index] from Tkeys.second
		vector<int> ts1;
		for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
		{
			ts1.push_back(ts[it->second]);
		}
		InhibitionMatrix.insert(make_pair(place, ts1));
	}
	///////////Marking
	map<string, int, cmpByKey> Pkeys; // value: count是Marking列index
									  // key = {"P1","P2",...};
	str = "Marking";
	if ((*it).find(str) == string::npos) {
		std::cout << "Error format," << *it << endl;
		return false;
	}
	else it++; // 略去*it = "Marking"
	for (count = 0; it != fromHtml.end(); it++)
	{
		if ((*it)[0] == 'P') {
			Pkeys.insert(pair<string, int>(*it, count)); // count是matrix列index
			count++;
		}
		else break;
	}
    int Pnum = count; // place number
	for (int i = 0; i < 2; i++) {
		vector<int> ps;  // 对应P1,P2,...的int
		string str;
		if (i == 0) str = "Initial";
		else str = "Current";
		if ((*it).find(str) == string::npos) {
			std::cout << "Error format," << *it << endl;
			return false;
		}
		it++;
		for (count = 0; count < Pnum; it++, count++)
		{
			ps.push_back(stoi(*it));
		}
		// ps[index] from Pkeys.second
		vector<int> ps1;
		for (map<string, int>::const_iterator it = Pkeys.begin(); it != Pkeys.end(); it++)
		{
			ps1.push_back(ps[it->second]);
		}
		Marking.insert(make_pair(str, ps1));
	}
	///////////Enabled transitions
	//vector<string> EnableTransitions;
	if ((*it).find("Enabled transitions") == string::npos)
	{
		std::cout << "Error format," << *it << endl;
		return false;
	}
	it++;
	// T1,T2,...
	for (int i = 0; i < Tnum; i++) 
	{
		if ((*it)[0] != 'T') {
			std::cout << "Error format," << *it << endl;
			return false;
		}
		it++;
	}
	vector<string> ts;
	for (; it != fromHtml.end(); it++)	ts.push_back(*it);
	for (map<string, int>::const_iterator it = Tkeys.begin(); it != Tkeys.end(); it++)
	{
		EnableTransitions.push_back(ts[it->second]); // ts[index] from Tkeys.second
	}
	
	if (it != fromHtml.end()) {
		std::cout << "Error format end." << endl;
		return false;
	}

	// 记录至Pvector
	for (map<string, int>::const_iterator it = Pkeys.begin(); it != Pkeys.end(); it++)
	{
		Pvector.push_back(it->first);
	}
	
	return ret;
}

// 显示和记录Incidence Matrix.html文件的解析结果信息
void HtmlHelper::viewIncidences(ofstream &fout)
{
	// others
	for (vector<string>::const_iterator it = others.begin(); it != others.end(); it++)
	{
		std::cout << "### " << *it << endl;
		fout << "### " << *it << endl;
	}
	std::cout << endl << "T-Vector(" << Tvector.size() << "):" << endl;
	fout << endl << "T-Vector(" << Tvector.size() << "):" << endl;
	for (vector<string>::const_iterator it = Tvector.begin(); it != Tvector.end(); it++)
	{
		if (it == Tvector.end() - 1) {
			std::cout << *it;
			fout << *it;
		}
		else {
			std::cout << *it << " ";
			fout << *it << " ";
		}
	}
	std::cout << endl;
	fout << endl;
	std::cout << endl << "P-Vector(" << Pvector.size() << "):" << endl;
	fout << endl << "P-Vector(" << Pvector.size() << "):" << endl;
	for (vector<string>::const_iterator it = Pvector.begin(); it != Pvector.end(); it++)
	{
		if (it == Pvector.end() - 1) {
			std::cout << *it;
			fout << *it;
		}
		else {
			std::cout << *it << " ";
			fout << *it << " ";
		}
	}
	std::cout << endl;
	fout << endl;

	//std::cout << endl << "Forwards incidence matrix I+ (P x T: " << ForwardMatrix.size() << " x " << Tvector.size() << "):" << endl;
	//fout << endl << "Forwards incidence matrix I+ (P x T:" << ForwardMatrix.size() << " x " << Tvector.size() << "):" << endl;
	//for (map<string, vector<int>>::const_iterator it = ForwardMatrix.begin(); it != ForwardMatrix.end(); it++)
	//{
	//	for (vector<int>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
	//	{
	//		if (it1 == it->second.end() - 1) {
	//			std::cout << *it1;
	//			fout << *it1;
	//		}
	//		else {
	//			std::cout << *it1 << " ";
	//			fout << *it1 << " ";
	//		}
	//	}
	//	std::cout << endl;
	//	fout << endl;
	//}

	std::cout << endl << "Forwards incidence matrix I+ (T x P: " << Tvector.size() << " x " << ForwardMatrix.size() << "):" << endl;
	fout << endl << "Forwards incidence matrix I+ (T x P: " << Tvector.size() << " x " << ForwardMatrix.size() << "):" << endl;
	for ( int i = 0; i < Tvector.size(); i++)
	{
		int j = 0;
		for (map<string, vector<int>>::const_iterator it = ForwardMatrix.begin(); it != ForwardMatrix.end(); it++,j++)
		{
			if (j == ForwardMatrix.size() -1) {
				std::cout << it->second[i];
				fout << it->second[i];
			}
			else {
				std::cout << it->second[i] << " ";
				fout << it->second[i] << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}

	std::cout << endl << "Backwards incidence matrix I- (T x P: " << Tvector.size() << " x " << BackwardMatrix.size() << "):" << endl;
	fout << endl << "Backwards incidence matrix I- (T x P: " << Tvector.size() << " x " << BackwardMatrix.size() << "):" << endl;
	for (int i = 0; i < Tvector.size(); i++)
	{
		int j = 0;
		for (map<string, vector<int>>::const_iterator it = BackwardMatrix.begin(); it != BackwardMatrix.end(); it++, j++)
		{
			if (j == BackwardMatrix.size() - 1) {
				std::cout << it->second[i];
				fout << it->second[i];
			}
			else {
				std::cout << it->second[i] << " ";
				fout << it->second[i] << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}

	std::cout << endl << "Combined incidence matrix I (P x T: " << CombinedMatrix.size() << " x " << Tvector.size() << "):" << endl;
	fout << endl << "Combined incidence matrix I (P x T: " << CombinedMatrix.size() << " x " << Tvector.size() << "):" << endl;
	for (map<string, vector<int>>::const_iterator it = CombinedMatrix.begin(); it != CombinedMatrix.end(); it++)
	{
		for (vector<int>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
		{
			if (it1 == it->second.end() - 1) {
				std::cout << setw(4) << *it1;
				fout << setw(4) << *it1;
			}
			else {
				std::cout << setw(4) << *it1 << " ";
				fout << setw(4) << *it1 << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}
	
	std::cout << endl << "Inhibition matrix H (P x T: " << InhibitionMatrix.size() << " x " << Tvector.size() << "):" << endl;
	fout << endl << "Inhibition matrix H (P x T: " << InhibitionMatrix.size() << " x " << Tvector.size() << "):" << endl;
	for (map<string, vector<int>>::const_iterator it = InhibitionMatrix.begin(); it != InhibitionMatrix.end(); it++)
	{
		for (vector<int>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
		{
			if (it1 == it->second.end() - 1) {
				std::cout << *it1;
				fout << *it1;
			}
			else {
				std::cout << *it1 << " ";
				fout << *it1 << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}

	std::cout << endl << "Marking(" << Marking.size() << " x " << Pvector.size() << "):" << endl;
	fout << endl << "Marking(" << Marking.size() << " x " << Pvector.size() << "):" << endl;
	for (map<string, vector<int>>::const_reverse_iterator it = Marking.rbegin(); it != Marking.rend(); it++)
	{
		std::cout << it->first << endl;
		fout << it->first << endl;
		for (vector<int>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
		{
			if (it1 == it->second.end() - 1){
				std::cout << *it1;
				fout << *it1;
			}
			else{
				std::cout << *it1 << " ";
				fout << *it1 << " ";
			}
		}
		std::cout << endl;
		fout << endl;
	}

	std::cout << endl << "Enabled transitions(" << Tvector.size() << "):" << endl;
	fout << endl << "Enabled transitions(" << Tvector.size() << "):" << endl;
	for (vector<string>::const_iterator it = EnableTransitions.begin(); it != EnableTransitions.end(); it++)
	{
		if (it == EnableTransitions.end() - 1) {
			std::cout << *it;
			fout << *it;
		}
		else {
			std::cout << *it << " ";
			fout << *it << " ";
		}
	}
	std::cout << endl;
	fout << endl;
}