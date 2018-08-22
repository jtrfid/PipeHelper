// test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
//#include <cstring>
#include <cassert>
#include <iterator>
#include <locale>
#include <cstring>
#include <vector>

using namespace std;

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

	// 删除s的后缀空格
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
	if (s.empty()) return sVector;
	string str(s); // 复制s至str
	trim(str); // 去除前后缀空格('\n','\r','\t',' ')
	if (str.empty()) return sVector;
	string::size_type pos1 = 0, pos2 = 0;
	while (1)
	{
		pos2 = str.find(delimit, pos1); // 从pos1开始找分隔符
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

int main()
{
	{
		string s = " abc ，321,123";
		//string s = " abc , 321,123";
		cout << s << endl;
		for (char c : s)cout << c << "|" << (int)c << endl;
		vector<string> v;
		split(s, v, ',');
		for (string a : v) cout << a << endl;
	}
	{
		string s1("ABc");
		string s2("abc");
		int a = _strcmpi(s1.c_str(), s2.c_str());
		cout << s1 << "\t" << s2 << "\t" << a << endl;
	}
	{
		string s;
		int a = 10;
		s += "P" + to_string(a);
		cout << s << endl;
	}
	{
		string s = { 'a','b' };
		string s1(s),s2;
		cout << s << "\t" << s1 << "\t" << s1.empty() << "\t" << s2.empty() << "\t" << s2.size() << endl;
	}
	{
		string s = "012345";
		cout << s.replace(1, 2, "abc") << endl; // 0abc345
		s = "012345";
		cout << s.replace(s.find('1'), 1, "abc") << endl; // 0abc2345
		s = "012345";
		cout << s.replace(s.find('1'),s.find('4'), "abc") << endl; // 0abc5
	}
	{
		string s1 = "P2",s2="P10";
		cout << s1.compare(s2) << endl; // 1
		cout << (s1 > s2) << endl; // 1
	}
	{
		string s = "012345";
		s.erase(s.find('4')); // 0123
		cout << s << "," << endl;
	}
	{
		string s = "abc.xt"; 
		s.insert(s.cbegin() + s.find_last_of('.') + 1, 't'); //abc.txt 
		cout << s << endl;
	}
	{
		// insert(size_type index, const char* s)
		string s = "abc";
		s.insert(1, "123");
		cout << s << endl;
	}
	{
		std::string s = "xmplr";

		// insert(size_type index, size_type count, char ch)
		s.insert(0, 1, 'E');
		assert("Exmplr" == s);

		// insert(size_type index, const char* s)
		s.insert(2, "e");
		assert("Exemplr" == s);

		// insert(size_type index, string const& str)
		s.insert(6, "a"s);
		assert("Exemplar" == s);

		// insert(size_type index, string const& str,
		//     size_type index_str, size_type count)
		s.insert(8, " is an example string."s, 0, 14);
		assert("Exemplar is an example" == s);

		// insert(const_iterator pos, char ch)
		s.insert(s.cbegin() + s.find_first_of('n') + 1, ':');
		assert("Exemplar is an: example" == s);

		// insert(const_iterator pos, size_type count, char ch)
		s.insert(s.cbegin() + s.find_first_of(':') + 1, 2, '=');
		assert("Exemplar is an:== example" == s);

		// insert(const_iterator pos, InputIt first, InputIt last)
		{
			std::string seq = " string";
			s.insert(s.begin() + s.find_last_of('e') + 1,
				std::begin(seq), std::end(seq));
			assert("Exemplar is an:== example string" == s);
		}

		// insert(const_iterator pos, std::initializer_list<char>)
		s.insert(s.cbegin() + s.find_first_of('g') + 1, { '.' });
		assert("Exemplar is an:== example string." == s);
	}
	{
		string s("a");
		cout << s.capacity() << endl;
		//s = "0123456789123456789";
		cout << s << endl;
		//s.append("123");
		cout << s << endl;
		cout << s[1] << endl;
		//cout << s.at(1) << endl; // 异常
	}
	{
		string const s("123");
		assert(s.size() == std::strlen(s.data()));
		//assert(std::equal(s.begin(), s.end(), s.data()));
		assert(std::equal(s.data(), s.data() + s.size(), s.begin()));
		assert(0 == *(s.data() + s.size()));
		int a = stoi(s);
		cout << s << endl;
	}
	{
		string s1("example1");
		//string const s2 = "const example";
		const string s2 = "const example";
		//s2[1] = 'a'; // 非法
		s1[1] = 'a';
		cout << s2 << endl;
		for (auto it = s1.rbegin(); it != s1.rend(); it++)
			cout << *it << "\t";
		cout << endl;
		for (auto it = s2.begin(); it != s2.end(); it++)
			cout << *it << "\t";
		cout << endl;
		for (string::iterator it = s1.begin(); it != s1.end(); it++)
			cout << *it << "\t";
		cout << endl;
		for (string::const_iterator it = s2.cbegin(); it != s2.cend(); it++)
			cout << *it << "\t";
		cout << endl;
	}

	string s1; // 构造一个空的字符串
	assert(s1.empty() && (s1.length() == 0) && (s1.size() == 0)); // 验证
	int a = 10;
	s1.assign("P").append(to_string(a));  // s1成为P10
	string s2("abc");  // 构造字符串"abc"
	string s3 = "abc"; // 等效

	cout << s3.at(1) << "," << s3[1] << endl;
	cout << s3.front() << "," << s3.back() << endl;

	// 使用find()查找特定字符，找到返回字符所在的位置(0是第一个字符位置)，否则返回string::npos
	string::size_type pos;
	pos = s3.find('c');  
	// pos = s3.find('c', 0); // 等效，从第0个位置开始查找
	if (pos != string::npos) {  // 找到
		cout << s3 << "中找到字符c,at " << pos << endl;  // pos = 2
	}
	pos = s3.find('d');
	if (pos == string::npos) { // 没有找到
		cout << s3 << "中无字符d" << endl;
	}
	
    
	// 使用substr()提取子串
	string s4 = s3.substr(1); // 提取s3的子串，从位置1开始到结束，s4 = "bc"
	// 提取s3的子串，从位置pos开始，提取count个字符，获取子串[pos,pos+count)
	pos = 0;
	string s5 = s3.substr(pos, 2);  // s5 = "ab"

	cout << s1 << "," << s2 << "," << s3 << "," << s4 << "," << s5 << endl;

	system("pause");
    return 0;
}

