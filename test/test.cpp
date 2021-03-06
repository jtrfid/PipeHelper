// test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

/***************************************************
 * map key compare
 * include compare string's length
 * 用于排序map的keys，如："P1","P2","P10",...
 * 用法：
 * （1）map键值排序：map<string,int,cmpByKey> map1;
 * （2）set排序: set<string,cmpByKey> set1;
 * （3）vector<string>排序：
 *      #include <algorithm>
 *      vector<string> vec1;
 *      std::sort(vec1.begin(), vec1.end(), cmpByKey());
 ****************************************************/
struct cmpByKey {
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		bool ret;
		if (lhs.size() == rhs.size())
			ret = lhs < rhs ? true : false;
		else
			ret = lhs.size() < rhs.size() ? true : false;
		return ret;
	}
};

int main1()
{
	cout << "map内置比较函数排序(升序)" << endl;
	map<string, int, less<string>> map1;  // 使用默认系统定义的键值比较函数(升序)，等效于map<string, int> map1;
	map1.insert(make_pair("P10", 1));
	map1.insert(make_pair("P2", 2));
	map1.insert(make_pair("P3", 3));
	map1.insert(make_pair("P3", 33)); // 无效
	map1["P4"] = 4;  // 插入key "P4"的value为4
	map1["P4"] = 44;  // 修改key "P4"的value为44
	for (auto it = map1.begin(); it != map1.end(); it++)
	{
		if (next(it) == map1.end()) cout << it->first << "," << it->second << endl;
		else cout << it->first << "," << it->second << " | ";
	} // P10, 1 | P2, 2 | P3, 3 | P4, 44

	cout << "map内置比较函数排序(降序)" << endl;
	map<string, int, greater<string>> map2; // 使用系统定义的键值比较函数(降序)
	map2.insert(make_pair("P10", 1));
	map2.insert(make_pair("P2", 2));
	map2.insert(make_pair("P3", 3));
	for (auto it = map2.begin(); it != map2.end(); it++)
	{
		if (next(it) == map2.end()) cout << it->first << "," << it->second << endl;
		else cout << it->first << "," << it->second << " | ";
	} // P3,3 | P2,2 | P10,1

	cout << "set内置比较函数排序(降序)" << endl;
	set<string, greater<string>> set1;  // 降序(greater)，升序(less)
	set1.insert("P10");
	set1.insert("P2");
	set1.insert("P3");
	for (auto it = set1.begin(); it != set1.end(); it++)
	{
		if (next(it) == set1.end()) cout << *it << endl;
		else cout << *it << ",";
	} // P3,P2,P10
	
	cout << "map自定义比较函数排序" << endl;
	map<string, int, cmpByKey> map3;
	map3.insert(make_pair("P10", 1));
	map3.insert(make_pair("P2", 2));
	map3.insert(make_pair("P3", 3));
	map3.insert(make_pair("P3", 33)); // 无效
	map3["P4"] = 4;  // 插入key "P4"的value为4
	map3["P4"] = 44;  // 修改key "P4"的value为44

	for (auto it = map3.begin(); it != map3.end(); it++)
	{
		if(next(it) == map3.end()) cout << it->first << "," << it->second << endl;
		else cout << it->first << "," << it->second << " | ";
	} // P2,2 | P3,3 | P4,44 | P10,1

	cout << "set自定义比较函数排序" << endl;
	set<string, cmpByKey> set2;
	set2.insert("P10");
	set2.insert("P2");
	set2.insert("P3");
	set2.insert("P3"); // 无效
	for (auto it = set2.begin(); it != set2.end(); it++)
	{
		if (next(it) == set2.end()) cout << *it << endl;
		else cout << *it << ",";
	} // P2,P3,P10
	
	cout << "vector<>内置比较函数排序(升序)" << endl;
	vector<string> vect1 = { "P10","P2","P3","P2" };
	// std::sort(vect1.begin(), vect1.end(), less<string>()); // 等效默认
	std::sort(vect1.begin(), vect1.end()); // 默认
	for (auto it = vect1.begin(); it != vect1.end(); it++)
	{
		if (next(it) == vect1.end()) cout << *it << endl;
		else cout << *it << ",";
	} // P10,P2,P2,P3

	cout << "vector<>内置比较函数排序(降序)" << endl;
	vector<string> vect2 = { "P10","P2","P3","P2" };
	std::sort(vect2.begin(), vect2.end(), greater<string>());
	for (auto it = vect2.begin(); it != vect2.end(); it++)
	{
		if (next(it) == vect2.end()) cout << *it << endl;
		else cout << *it << ",";
	} // P3,P2,P2,P10

	cout << "vector<>自定义比较函数排序" << endl;
	vector<string> vect3 = { "P10","P2","P3","P2" };
	std::sort(vect3.begin(), vect3.end(), cmpByKey());
	for (auto it = vect3.begin(); it != vect3.end(); it++)
	{
		if (next(it) == vect3.end()) cout << *it << endl;
		else cout << *it << ",";
	} // P2,P2,P3,P10

	system("pause");
    return 0;
}

/********************************************
 检测两个集合是否相等:
 定义于头文件 <algorithm>的算法：
 (1) 如果范围 [first1, last1) 和范围 [first2, last2) 相等，返回 true ，否则返回 false
 template< class InputIt1, class InputIt2 >
 bool equal( InputIt1 first1, InputIt1 last1,
 InputIt2 first2, InputIt2 last2 );
 
 (2) 如果范围 [first1, last1) 和范围 [first2, first2 + (last1 - first1) 相等，返回 true ，否则返回 false
 template< class InputIt1, class InputIt2 >
 bool equal( InputIt1 first1, InputIt1 last1, InputIt2 first2 );
********************************************/

// 下面的代码使用 equal() 来测试字符串是否是回文
void test(const std::string& s)
{
	if (std::equal(s.begin(), s.begin() + s.size() / 2, s.rbegin())) {
		std::cout << "\"" << s << "\" is a palindrome\n";
	}
	else {
		std::cout << "\"" << s << "\" is not a palindrome\n";
	}
}

int main()
{
	// 相同比较函数，检测是否相等才有意义,按顺序比较。
	// 排序后前
	map<string, int, less<string>> map1 = { {"P10",1},{"P2",2},{"P3",3} };
	map<string, int, less<string>> map2 = { { "P3",3 },{ "P2",2 },{ "P10",1 },{ "P4",4 } };
	
	// 排序后前三个元素不同
	//map<string, int, greater<string>> map1 = { { "P10",1 },{ "P2",2 },{ "P3",3 } };
	//map<string, int, greater<string>> map2 = { { "P3",3 },{ "P2",2 },{ "P10",1 },{ "P4",4 } };

	// 排序后前三个元素不同
	//map<string, int, cmpByKey> map1 = { { "P10",1 },{ "P2",2 },{ "P3",3 } };
	//map<string, int, cmpByKey> map2 = { { "P3",3 },{ "P2",2 },{ "P10",1 },{ "P4",4 } };

	for (auto it = map1.begin(); it != map1.end(); it++)
	{
		if (next(it) == map1.end()) cout << it->first << "," << it->second << endl;
		else cout << it->first << "," << it->second << " | ";
	} // P10, 1 | P2, 2 | P3, 3

	for (auto it = map2.begin(); it != map2.end(); it++)
	{
		if (next(it) == map2.end()) cout << it->first << "," << it->second << endl;
		else cout << it->first << "," << it->second << " | ";
	} // P10, 1 | P2, 2 | P3, 3 | P4, 4

	// 检测两个集合是否相等，如果范围 [first1, last1) 和范围 [first2, last2) 相等，返回 true ，否则返回 false
	bool isEqual = std::equal(map1.begin(), map1.end(), map2.begin(), map2.end());
	cout << isEqual << endl; // 0, 表示不相等

	// 检测两个集合是否相等，仅比较范围[first1, last1) 和范围 [first2, first2 + (last1 - first1)
	isEqual = std::equal(map1.begin(), map1.end(),map2.begin());
	cout << isEqual << endl; // 1, 表示相等

	cout << "回文检测" << endl;
	test("radar"); // "radar" is a palindrome
	test("hello"); // "hello" is not a palindrome

	system("pause");
	return 0;
}

/*********************************************
 查找集合中出现的首个特定元素 
 返回范围 [first, last) 中满足特定判别标准的首个元素。
 定义于头文件 <algorithm>
 template< class InputIt, class T >
 InputIt find( InputIt first, InputIt last, const T& value );
 *********************************************/
int main3333333()
{
	// set<>示例
	set<string> set1 = { "s1","s2","s3" };
	if (set1.find("s2") != set1.end()) cout << "found\n"; // found
	else cout << "not found!\n";
	if (set1.find("s4") != set1.end()) cout << "found\n";
	else cout << "not found!\n";  // not found!

	// vector<>示例
	int n1 = 3;
	int n2 = 5;
	std::vector<int> v{ 0, 1, 2, 3, 4 };
	auto result1 = std::find(std::begin(v), std::end(v), n1);
	auto result2 = std::find(std::begin(v), std::end(v), n2);
	if (result1 != std::end(v)) {
		std::cout << "v contains: " << n1 << '\n'; // v contains: 3
	}
	else {
		std::cout << "v does not contain: " << n1 << '\n';
	}

	if (result2 != std::end(v)) {
		std::cout << "v contains: " << n2 << '\n';
	}
	else {
		std::cout << "v does not contain: " << n2 << '\n'; // v does not contain: 5
	}

	system("pause");
	return 0;
}

int mainbbbbbbbbbbb()
{
	
	std::vector<int> v1{ 1,2,3,4,5,6,7,8 };
	std::vector<int> v2{ 5,  7,  9,10 };
	std::sort(v1.begin(), v1.end());  // 升序排序
	std::sort(v2.begin(), v2.end());  

	// A交B
	cout << "set_intersection:" << endl;
	std::vector<int> v_intersection;
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));

	for (int n : v_intersection) std::cout << n << ' '; cout << endl; // 5 7

	// A并B
	cout << "set_union" << endl;
	std::vector<int> dest1;
	std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(dest1));
	for (const auto &i : dest1) std::cout << i << ' ';	std::cout << '\n';  // 1 2 3 4 5 6 7 8 9 10

	// A - B
	cout << "set_difference" << endl;
	std::vector<int> diff;
	std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(diff));
	for (int n : diff) std::cout << n << ' '; cout << endl;  // 1 2 3 4 6 8
	
	// 对称差(symmetric_difference,A并B - A交B)
	cout << "set_symmetric_difference" << endl;
	std::vector<int> v_symDifference;
	std::set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_symDifference));
	for (int n : v_symDifference) std::cout << n << ' '; // 1 2 3 4 6 8 9 10
	cout << endl;

	system("pause");
	return 0;
}

int mainaaaa()
{
	
	std::vector<int> v1{ 1,2,3,4,5,6,7,8 };
	std::vector<int> v2{ 5,  7,  9,10 };
	//std::sort(v1.begin(), v1.end());  // 升序排序,默认使用less<int>比较函数
	//std::sort(v2.begin(), v2.end());

	// 采用非默认比较函数，
	std::sort(v1.begin(), v1.end(), greater<int>());  // 降序排序
	std::sort(v2.begin(), v2.end(), greater<int>());

	// 集合运算必须与集合的排序规则一致，因此最后一个参数需设置比较函数
	// A交B
	cout << "set_intersection:" << endl;
	std::vector<int> v_intersection;
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection),greater
	<int>());

	for (int n : v_intersection) std::cout << n << ' '; cout << endl; // 7 5

	// A并B
	cout << "set_union" << endl;
	std::vector<int> dest1;
	std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(dest1),greater<int>());
	for (const auto &i : dest1) std::cout << i << ' ';	std::cout << '\n';  // 10 9 8 7 6 5 4 3 2 1 
																			
	// A - B
	cout << "set_difference" << endl;
	std::vector<int> diff;
	std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(diff),greater<int>());
	for (int n : diff) std::cout << n << ' '; cout << endl;  // 8 6 4 3 2 1

	// 对称差(symmetric_difference,A并B - A交B)
	cout << "set_symmetric_difference" << endl;
	std::vector<int> v_symDifference;
	std::set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_symDifference),greater<int>());
	for (int n : v_symDifference) std::cout << n << ' '; // 10 9 8 6 4 3 2 1
	cout << endl;

	set<string, cmpByKey> set1 = { "P1","P10","P3","P20" };
	set<string, cmpByKey> set2 = { "P10","P2" };
	set<string, cmpByKey> results;
	// 编译出错，修改为下句
	//std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::back_inserter(results), cmpByKey());
	std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(results, results.end()), cmpByKey());
	for (string s : results) std::cout << s << ' '; cout << endl;  // P10
	results.clear();
	std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(results, results.end()), cmpByKey());
	for (string s : results) std::cout << s << ' '; cout << endl;  // P1 P2 P3 P10 P20
	results.clear();
	std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(results, results.end()), cmpByKey());
	for (string s : results) std::cout << s << ' '; cout << endl;  // P1 P3 P20
	results.clear();
	std::set_symmetric_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(results, results.end()), cmpByKey());
	for (string s : results) std::cout << s << ' '; cout << endl;  // P1 P2 P20

	system("pause");
	return 0;
}




