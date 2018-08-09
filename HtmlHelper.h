#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "Tools.h"

using namespace std;

/*******************************************************************
 * 解析PIPE输出的html文件: Invariant Analysis.html,Incidence Matrix.html
 * 前者调用：bool InvariantsHtml(const string& htmlFile, const string &outFile);
 * 后者调用: bool IncidencesHtml(const string& htmlFile, const string &outFile);
 * 依赖下列文件：
 * HtmlParser.h
 * HtmlParse.cpp
 * Note: HtmlParse.cpp 编译设置（fopen函数使用）：
 * 项目->属性->配置属性->C/C++ ->预处理器->预处理器定义添加：_CRT_SECURE_NO_WARNINGS
 ********************************************************************/
class HtmlHelper
{
public:
	HtmlHelper();
	~HtmlHelper();

///////////////////Invariant Analysis.html
public:
	/**************************************
	 * 读取PIPE输出的Invariant Analysis.html文件，解析后生成文本文件
	 * 参数：
	 * const string& htmlFile: PIPE输出的Invariant Analysis.html文件
	 * const string& outFile:  解析后生成文本文件
	 * 成功返回true，否则返回false
	 ***************************************/
	bool InvariantsHtml(const string& htmlFile, const string &outFile);
private:
	// 解析Html文件获得的T向量
	vector<string> Tvector;
	// 解析Html文件获得的P向量
	vector<string> Pvector;
	// 解析Html文件获得的T不变式
	map<int, vector<int>> Tinvariants;
	// 解析Html文件获得的P不变式
	map<int, vector<int>> Pinvariants;
	// 解析Html文件,除了T、P不变式的其他信息
	vector<string> others;

	/**************************************
	 * 读取PIPE输出的Invariant Analysis.html文件
	 * 参数输入:
	 * const string& htmlFile: PIPE输出的html文件
	 * 参数返回:
	 * vector<string>& fromHtml: 去掉标签标记后的有效信息
	 * 成功返回true，否则返回false
	 ***************************************/
	bool InvariantHtml(const string& htmlFile, vector<string>& fromHtml);

	/********************************************
	 * 处理来自Invariant Analysis.html的文件
	 * 参数：
	 * formHtml去掉标签标记后的有效信息
	 * 成功返回true，否则返回false
	 *********************************************/
	bool InvariantHtml(const vector<string> &fromHtml);

	// 显示和记录Invariant Analysis.html文件的解析结果信息
	void viewInvariants(ofstream &fout);

	// 清除数据成员
	void clear();

////////////////// Incidence Matrix.html
public:
	/**************************************
	 * 读取PIPE输出的Incidence Matrix.html文件，解析后生成文本文件
	 * 参数：
	 * const string& htmlFile: PIPE输出的Incidence Matrix.html文件
	 * const string& outFile:  解析后生成文本文件
	 * 成功返回true，否则返回false
	 ***************************************/
	bool IncidencesHtml(const string& htmlFile, const string &outFile);
private:
	// 解析Html文件获得的T向量
	// vector<string> Tvector; // 共用
	// 解析Html文件获得的P向量
	// vector<string> Pvector; // 共用
	// 解析Html文件获得的T不变式
	// Forwards incidence matrix I+
	map<string, vector<int>, cmpByKey> ForwardMatrix;
	// Backwards incidence matrix I-
	map<string, vector<int>, cmpByKey> BackwardMatrix;
	// Combined incidence matrix I
	map<string, vector<int>, cmpByKey> CombinedMatrix;
	// Inhibition matrix H
	map<string, vector<int>, cmpByKey> InhibitionMatrix;
	// Marking
	map<string, vector<int>, cmpByKey> Marking;
	// Enabled transitions
	vector<string> EnableTransitions;

	/**************************************
	 * 读取PIPE输出的Invariant Analysis.html文件
	 * 参数输入:
	 * const string& htmlFile: PIPE输出的html文件
	 * 参数返回:
	 * vector<string>& fromHtml: 去掉标签标记后的有效信息
	 * 成功返回true，否则返回false
	 ***************************************/
	bool IncidencesHtml(const string& htmlFile, vector<string>& fromHtml);

	/********************************************
	 * 处理来自Incidence Matrix.html的文件
	 * 参数：
	 * formHtml去掉标签标记后的有效信息
	 * 成功返回true，否则返回false
	 *********************************************/
	bool IncidencesHtml(const vector<string> &fromHtml);

	// 显示和记录Incidence Matrix.html文件的解析结果信息
	void viewIncidences(ofstream &fout);

};


