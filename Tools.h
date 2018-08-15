#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<set>

using namespace std;

/***************************************************
 * map key compare
 * include compare string's length
 * 用于排序map的keys，如："P1","P2","P10",...
 * 用法：
 * （1）map键值排序：map<sting,int,cmpByKey> map1;
 * （2）vector<string>排序：
 *      #include <algorithm>
 *      vector<string> vec1;
 *      std::sort(vec1.begin(), vec1.end(), cmpByKey());
 ****************************************************/
struct cmpByKey {
	bool operator()(const string& lhs, const string& rhs) {
		bool ret;
		if (lhs.size() == rhs.size())
			ret = lhs < rhs ? true : false;
		else
			ret = lhs.size() < rhs.size() ? true : false;
		return ret;
	}
};

/***************************************************
 * pari<string,int> compare
 * include compare string's length
 * 用于排序map的keys，如："P1","P2","P10",...
 * 用法：map<string,int> map1;
 * #include <algorithm>
 * sort(map1.begin(),map1.end(),cmpPair<int>);
 ****************************************************/
template<typename T>
struct cmpPair {
	bool operator()(const pair<string, T>& lhs, const pair<string, T>& rhs) {
		bool ret;
		if (lhs.first.size() == rhs.first.size())
			ret = lhs.first < rhs.first ? true : false;
		else
			ret = lhs.first.size() < rhs.first.size() ? true : false;
		return ret;
	}
};

class Tools
{
public:
	Tools();
	~Tools();
	/////////////////////// 通用静态函数
public:
	/******************************************
	 * print elements of vector<T> to cout
	 * flag = 1, output:
	 * element1,element2,...
	 * flag = 2, output:
	 * element1
	 * element2
	 * ....
	 ******************************************/
	template<typename T>
	static void vectorPrint(const vector<T> t, ofstream &fout, int flag = 1)
	{
		fout << "elements number: " << t.size() << endl;
		for (auto it = t.begin(); it != t.end(); it++)
		{
			if (flag == 1) {
				if (it == t.end() - 1) fout << *it;
				else fout << *it << ",";
			}
			else {
				fout << *it << endl;
			}
		}
		if (flag == 1) fout << endl;
	}

	/******************************************
	 * print elements of vector<T> to cout
	 * flag = 1, output:
	 * element1,element2,...
	 * flag = 2, output:
	 * element1
	 * element2
	 * ....
	 ******************************************/
	template<typename T>
	static void vectorPrint(const vector<T> t, int flag = 1)
	{
		cout << "elements number: " << t.size() << endl;
		for (auto it = t.begin(); it != t.end(); it++)
		{
			if (flag == 1) {
				if (it == t.end() - 1) cout << *it;
				else cout << *it << ",";
			}
			else {
				cout << *it << endl;
			}
		}
		if (flag == 1) cout << endl;
	}

	/*******************************************
	 * （1）去除前后缀空格('\n','\r','\t',' ')
	 * （2）删除字符串内的'\n'
	 * （3）字符串内连续空格仅保留一个空格
	 * 参数返回处理后的字符串
	 * after deleted, if str is empty,return true, else return false
	 *******************************************/
	static bool isSpaceLine(string &str)
	{
		if (str.empty()) return true;

		int count = 0;
		// 删除前缀空格
		for (string::iterator it = str.begin(); it != str.end(); it++)
		{
			if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') count++;
			else break;
		}
		str.erase(0, count);
		// 删除后缀空格
		count = 0;
		for (string::const_reverse_iterator it = str.rbegin(); it != str.rend(); it++)
		{
			if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') count++;
			else break;
		}
		str.erase(str.length() - count);
		// 删除字符串内的'\n'
		string::size_type pos;
		while (1) {
			pos = str.find('\n');
			if (pos == string::npos) break; // 没找到
			str.erase(pos, 1);
		}
		// 字符串内连续空格仅保留一个空格
		string::size_type pos1 = 0;
		while (1) {
			pos = str.find(' ', pos1); // 从pos1开始找 
			if (pos == string::npos) break;
			count = 0;  // 多余空格数
			pos1 = pos + 1;
			while (pos1 < str.length()) {
				if (str[pos1] == ' ') count++;
				else break;
				pos1++;
			}
			if (count >= 1) {
				str.erase(pos + 1, count);
				pos1 = pos + 1;
			}
		}

		if (str.empty()) return true;
		else return false;
	}

	/*************************************
	 * 去除前后缀空格('\n','\r','\t',' ')
	 *************************************/
	static string& trim(string& s)
	{
		if (s.empty()) return s;
		// 去掉s的前缀空格
		string::size_type pos = 0;
		for (auto it = s.begin(); it < s.end(); it++) {
			if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') pos++;
			else break;
		}
		s = s.substr(pos);

		// 删除后缀空格
		while (1)
		{
			char end = s.back();
			if (end == '\n' || end == '\r' || end == '\t' || end == ' ') 
				s.erase(prev(s.end()));
			else break;
		}
		return s;
	}

	/*************************************
	 * 首先去除s的前后缀空格，
	 * 然后提取以指定分隔符隔开的子字符串至sVector.
	 * 并且保证各个子串前后无空格
	 * 空串或去除前后缀的空格后是空串，直接返回
	 *************************************/
	static vector<string>& split(const string& s, vector<string>& sVector, char delimit)
	{
		string str(s);
		if (s.empty()) return sVector;
		trim(str);
		if (str.empty()) return sVector;
		string::size_type pos1 = 0, pos2 = 0;
		while (1)
		{
			pos2 = str.find(delimit,pos1); // 从pos1开始找分隔符
			if (pos2 == string::npos)  break; // 未找到分隔符
			string sub = str.substr(pos1, pos2 - pos1);
			trim(sub);
			if (sub.empty()) std::cout << "Waring, have empty sub string in " << s << endl;
			sVector.push_back(sub);
			pos1 = pos2 + 1;
		}
		// 添加最后一个
		string sub = str.substr(pos1);
		trim(sub);
		if (sub.empty()) std::cout << "Waring, have empty splited substring in " << s << endl;
		sVector.push_back(sub);

		return sVector;
	}

	// s1和s2集合是否相同
	static bool sameSet(const set<string>& s1, const set<string>& s2)
	{
		if (s1.size() != s2.size()) return false;
		for (auto it = s1.begin(); it != s1.end(); it++)
		{
			if(s2.count(*it)==0) return false;
		}
		return true;
	}
};

