#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <iterator>
#include "Controler.h"
#include "Tools.h"

using namespace std;

Controler::Controler()
{
}


Controler::~Controler()
{
}

/************************************
* 信标,补集等
* const string& pntFile: 有资源库所和工序库所信息的pnt文件
* const string& SESSION.ina 文件
* const string& outFile: outpu file
* 成功返回true，否则返回false
************************************/
bool Controler::comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile)
{
	bool ret = true;
	ofstream fout(outFile);
	// read 有资源库所和工序库所信息的pnt文件，填充数据成员
	readPnt(pntFile);
	//读取SESSION.ina, 填充数据成员Siphons
	sessionIna(SESSIONina);
	// 填充信标补集ComSiphons
	for (auto it = Siphons.begin(); it != Siphons.end(); it++)
	{
		Siphon s = it->second;
		if (s.clean) {  // 清空的信标
			set<string> comSiphon;
			ret = comSiphons(s.siphon, PR, comSiphon);
			if (!ret) break;
			ComSiphons.insert(make_pair(it->first, comSiphon));
		}
	}

	view(fout);

	if (!ret) cout << "\nError, in function comSiphons()." << endl;
	fout.close();
	return ret;
}

/************************************
 * 信标补集
 * 输入参数:
 * const vector<string>& siphons: 信标集
 * const vector<string>& Pr: 资源库所集
 * 输出参数:
 * set<string>& comSiphon: 返回信标补集
 * 成功返回true, 否则，返回false(例如，补集为空 )
 ************************************/
bool Controler::comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string>& comSiphon)
{
	bool ret = true;
	set<string> unionHr;
	ret = UnionHr(siphons, Pr, unionHr);
	// 差集
	for (auto it = unionHr.begin(); it != unionHr.end(); it++)
	{
		auto result = std::find(std::begin(siphons), std::end(siphons), *it);
		if (result != std::end(siphons)) continue; // 在siphons中
		comSiphon.insert(*it);
	}
	if (unionHr.empty()) ret = false;
	if (!ret) cout << "\nError, in function comSiphons()." << endl;
	return ret;
}

/************************************
 * Union H(r), r in Siphon与PR的交集
 * 输入参数:
 * const vector<string>& siphons: 信标集
 * const vector<string>& Pr: 资源库所集
 * 输出参数:
 * set<string>& unionHr: 返回Union H(r)
 * 成功返回true, 否则，返回false(例如，Holder为空或unionHr为空 )
 ************************************/
bool Controler::UnionHr(const vector<string>& siphons, const set<string>& Pr, set<string>& unionHr)
{
	bool ret = true;
	for (auto it = siphons.begin(); it != siphons.end(); it++)
	{
		auto result = std::find(std::begin(Pr),std::end(Pr), *it);
		if (result == std::end(Pr)) continue; // 不在PR中
		set<string> H;
		ret = holder(*it,H);
		if (!ret) break;
		unionHr.insert(H.begin(),H.end());
	}
	if (unionHr.empty()) ret = false;
	if (!ret) cout << "\nError, in function UnionHr()." << endl;
	return ret;
}

/************************************
 * 获取资源的Holder, resource前置集的前置集与PA的交集: H(r)
 * 输入参数:
 * const string& resource: place name
 * 输出参数:
 * set<string>& H: 返回resource的holder
 * 如果holder非空，函数返回true, 否则返回false
 ************************************/
bool Controler::holder(const string& resource, set<string>& H)
{
	H.clear();
	bool ret = false;
	set<string> prepres;
	if (!prePre(resource, prepres)) {
		return false;
	}
	// 两个集合求交前必须排序就绪
	std::set_intersection(prepres.begin(), prepres.end(), PA.begin(), PA.end(), inserter(H, H.end()));
	//std::set_intersection(prepres.begin(), prepres.end(), PA.begin(), PA.end(), inserter(H, H.begin())); // 等效
	if (H.empty()) ret = false;
	else ret = true;
	if (!ret) cout << "\nError, in function holder()." << endl;
	return ret;
}

// 显示和记录信息
void Controler::view(ofstream &fout)
{
	XmlHelper::view(fout);
	std::cout << "Pres,Posts:" << endl;
	fout << "Pres,Posts:" << endl;
	for (auto it = ptNodes.begin(); it != ptNodes.end(); it++)
	{
		std::cout << it->first << " Pres: ";
		fout << it->first << " Pres: ";
		for (auto it1 = it->second.Pres.begin(); it1 != it->second.Pres.end(); it1++)
		{
			if (it1 != prev(it->second.Pres.end())) {
				std::cout << *it1 << ",";
				fout << *it1 << ",";
			}
			else {
				std::cout << *it1;
				fout << *it1;
			}
		}
		std::cout << endl;
		fout << endl;
		std::cout << it->first << " Posts: ";
		fout << it->first << " Posts: ";
		for (auto it1 = it->second.Posts.begin(); it1 != it->second.Posts.end(); it1++)
		{
			if (it1 != prev(it->second.Posts.end())) {
				std::cout << *it1 << ",";
				fout << *it1 << ",";
			}
			else {
				std::cout << *it1;
				fout << *it1;
			}
		}
		std::cout << endl;
		fout << endl;
	}

	std::cout << "\nPR:" << endl;
	fout << "\nPR:" << endl;
	for (string s : PR) {
		std::cout << s << " ";
		fout << s << " ";
	}
	cout << endl;
	fout << endl;
	std::cout << "\nPA:" << endl;
	fout << "\nPA:" << endl;
	for (string s : PA) {
		std::cout << s << " ";
		fout << s << " ";
	}
	cout << endl;
	fout << endl;

	std::cout << "\nSiphons:" << endl;
	fout << "\nSiphons:" << endl;
	for (auto it = Siphons.begin(); it != Siphons.end(); it++)
	{
		if (it->second.clean) {
			cout << it->first << ":[empty clean] ";
			fout << it->first << ":[empty clean] ";
		}
		else {
			cout << it->first << ": ";
			fout << it->first << ": ";
		}
		for (string place : it->second.siphon) {
			cout << place << " ";
			fout << place << " ";
		}
		cout << endl;
		fout << endl;
	}

	std::cout << "\nComSiphons:" << endl;
	fout << "\nComSiphons:" << endl;
	for (auto it = ComSiphons.begin(); it != ComSiphons.end(); it++)
	{
		cout << it->first << ": ";
		fout << it->first << ": ";
		for (string place : it->second) {
			cout << place << " ";
			fout << place << " ";
		}
		cout << endl;
		fout << endl;
	}
}

/************************************
 * 获取节点的前置集的前置集
 * 输入参数:
 * const &string ptName: place/transition name
 * 输出参数:
 * set<string> &prePre: 返回ptName的前置集的前置集
 * 如果有前置集的前置集，函数返回true, 否则返回false
 ************************************/
bool Controler::prePre(const string& ptName,set<string> &prePre)
{
	bool ret = false;
	set<string> pres = ptNodes[ptName].Pres;
	for (auto it = pres.begin(); it != pres.end(); it++)
	{
		set<string> prepre = ptNodes[*it].Pres;
		for (auto it1 = prepre.begin(); it1 != prepre.end(); it1++)
		{
			prePre.insert(*it1);
			ret = true;
		}
	}
	return ret;
}

/************************************
 * 填充数据成员Pres，Posts
 ************************************/
void Controler::PrePosts()
{
	for (map<string, string>::const_iterator it = source.begin(); it != source.end(); it++)
	{
		// it->second的后置集添加target[it->first]
		auto it1 = ptNodes.find(it->second);
		if(it1 != ptNodes.end()) // 找到
		{
			ptNode n = ptNodes[it->second];
			n.Posts.insert(target[it->first]); 
			ptNodes[it->second] = n; // key 已存在，新值覆盖旧值, operator =
		}
		else // 没找到
		{
			ptNode n;
			n.Posts.insert(target[it->first]);
			ptNodes.insert(make_pair(it->second, n)); // key 不存在, insert
		}
		// target[it->first]的前置集添加it->second
		auto it2 = ptNodes.find(target[it->first]);
		if (it2 != ptNodes.end()) // 找到
		{
			ptNode n = ptNodes[target[it->first]];
			n.Pres.insert(it->second);
			ptNodes[target[it->first]] = n; // key 已存在，新值覆盖旧值, operator =
		}
		else // 没找到
		{
			ptNode n;
			n.Pres.insert(it->second);
			ptNodes.insert(make_pair(target[it->first], n)); // key 不存在,insert
		}
	}
}

/*****************************************
 * read 有资源库所和工序库所信息的pnt文件，填充数据成员
 * const string& pntFile: input pnt file
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
 * PR 9,10,11
 * PA 2,3,4,6,7,8
 *****************************************/
bool Controler::readPnt(const string &pntFile)
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
		pre = parts[2].substr(0, pos);
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

	if (fin.eof()) {
		ret = false;
		std::cout << "Error(无PR，PA行)!! read " << pntFile << endl;
	}
	// PR and PA
	while (!fin.eof())
	{
		getline(fin, line);
		if (!PRPA(line)) { 
			ret = false; 
			break; 
		}
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

	//填充数据成员Pres，Posts
	PrePosts();

	fin.close();
	return true;
}

/************************************
 * 处理PR or PA line
 * 成功返回true，否则返回false
 ************************************/
bool Controler::PRPA(const string &line)
{
	bool pa = false;
	string::size_type pos1,pos2;
	pos1 = line.find("PR");
	if (pos1 == string::npos) { // PR line
		pos1 = line.find("PA"); // PA line
		pa = true;
	}
	if (pos1 != string::npos)
	{
		pos1 += 2;
		while(1)
		{
			pos2 = line.find(',', pos1);
			if (pos2 == string::npos) break;
			string p = line.substr(pos1, pos2 - pos1);
			string place("P");
			place.append(to_string(atoi(p.c_str())));
			if (pa) PA.insert(place);
			else PR.insert(place);
			pos1 = pos2 + 1;
		}
		// 最后一个place
		string p = line.substr(pos1);
		string place("P");
		place.append(to_string(atoi(p.c_str())));
		if (pa) PA.insert(place);
		else PR.insert(place);
	}
	else return false;
	return true;
}

/*****************************************
 * read SESSION.ina文件，填充数据成员Siphons
 * const string& SESSION.ina文件名
 * 成功返回true，否则返回false
 * 
 * SESSION.ina文件格式
 * There are  23 minimal deadlocks:必然
 * -------------------------------必然
 *  1:  8,  9, 10, 11, 12, 13, 14,
 *  2:  2,  9, 15,
 *  3:  7,  9, 15, 16, 17, 18, 19, 20,
 *  4:  7, 10, 16, 17, 18, 19, 20,
 *  ....
 *  必然有一个空行
 * Corresponding maximal traps:必然
 * ----------------------------必然
 *  1:  8,  9, 10, 11, 12, 13, 14,
 *  2:  2,  9, 15,
 *  3: empty  clean
 *  4:  7, 10, 16, 17, 18, 19, 20,
 *  ....
 *  必然有一个空行
 *****************************************/
bool Controler::sessionIna(const string& SESSIONina)
{
	bool ret = true;
	ifstream fin(SESSIONina);
	if (!fin) {
		std::cout << "Error!! read " << SESSIONina << endl;
		return false;
	}
	// 读取pnt文件，填充数据成员Siphons
	Siphons.clear(); // 清空旧数据
	string line; // 每行字符串
	bool start1 = false, start2 = false;
	string::size_type pos;
	int id = 1;
	///////// 第一部分，全部信标
	while (!fin.eof())
	{
		getline(fin, line);
		if (!start1)
		{
			if (line.find("------") != string::npos) start1 = true;
			continue;
		}
		if (line.empty()) break; // 空行，第一部分结束
		// cout << "id = "<< id <<","<<line << endl;
		pos = line.find(':'); 
		if (pos == string::npos) { ret = false; break; } // 格式错误
		string s = line.substr(pos + 1);
		Tools::trim(s); // 去除前后缀空格
		if (s.back() == ',') s.erase(prev(s.end()));// 如果最后是','删除之
		// cout << s << endl;
		// 填充信标
		Siphon siphon1;
		siphon1.clean = false;
		vector<string> ps;
		Tools::split(s, ps, ',');
		for (string s : ps) {
			string ss = "P";
			ss.append(s);
			siphon1.siphon.push_back(ss);
		}
		Siphons.insert(make_pair(id, siphon1));
		id++;
	}
	///////// 第二部分，信标清空清苦
	while (!fin.eof() && start1)
	{
		getline(fin, line);
		if (!start2)
		{
			if (line.find("Corresponding maximal traps") != string::npos) {
				getline(fin, line);
				if (line.find("------") != string::npos) {
					start2 = true;
					id = 1;
				}
				else {	ret = false; break;	} // 格式错误
			}
			continue;
		}
		if (line.empty()) break; // 空行结束
		// cout << line << endl;
		if (line.find("empty  clean") != string::npos) { // id 对应的信标被清空
			Siphons[id].clean = true;
			// cout << "clean id="<<id<<endl;
		}
		id++;
	}
	if (fin.eof()) ret = false;  // 不应该就此结束

	if(!ret) cout << "Error farmat in file: " << SESSIONina << endl;
	fin.close();
	return ret;
}
