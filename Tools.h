#pragma once
#include<iostream>
#include<string>
#include<vector>

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
};

