#include "stdafx.h"
#include <iostream>
#include <iomanip>
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
 * 由.pnt和SESSION.ina文件计算控制器，生成新的含有控制器的pnt文件
 * const string& infoFile: information file信息文件
 * const string& pntCPFile: 生成新的含有控制器的pnt文件
 * 成功返回true，否则返回false
 ************************************/
bool Controler::pntCP(const string& pntFile, const string& SESSIONina, const string& infoFile, const string& pntCPFile)
{
	clear();
	bool ret = true;
	ofstream fout(infoFile);
	ofstream fpnt(pntCPFile);

	//由.pnt和SESSION.ina求信标补集
	ret = comSiphons(pntFile, SESSIONina, infoFile);
	// 添加控制器，填充数据成员controlerPlace和initialMarkingCP
	addControler();
	//从controlerPlace中筛选合法控制器,填充数据成员legalControler
	legalRule();

	// 生成pnt文件
	fpnt << "p  m   pre,pos"<< endl;
	for (auto it = Places.begin(); it != Places.end(); it++)
	{
		// 舍去原有PR
		auto ft = find(PR.begin(), PR.end(), *it);
		if (ft != PR.end()) continue;  // *it存在于PR中

		// 添加控制器以后, 保留下来的原有Places
		PlacesAfterCP.push_back(*it);

		string place = (*it).substr(1);
		int marking = initialMarking[*it];
		//fpnt << left << setw(4);
		fpnt << place << " " << marking << " ";
		set<string, cmpByKey> pres = ptNodes[*it].Pres;
		for (auto it1 = pres.begin(); it1 != pres.end(); it1++)
		{
			fpnt << right << setw(4);
			fpnt << (*it1).substr(1);
			string arcId;
			createArcId(*it, *it1, arcId);
			int w = weight[arcId];
			if (w > 1) {
				fpnt << ":" << w;
			}
			if(it1 != prev(pres.end())) fpnt << " ";
		}
		fpnt << setw(1) <<  ',';
		set<string,cmpByKey> posts = ptNodes[*it].Posts;
		for (auto it1 = posts.begin(); it1 != posts.end(); it1++)
		{
			fpnt << left << setw(4);
			fpnt << (*it1).substr(1);
			string arcId;
			createArcId(*it, *it1, arcId);
			int w = weight[arcId];
			if (w > 1) fpnt << ":" << w;
			if (it1 != prev(posts.end())) fpnt << " ";
		}
		fpnt << endl;
	}
	// controler
	for (auto it = CPInfo.begin(); it != CPInfo.end(); it++)
	{
		if (it->second.removed || it->second.isEmpty) continue; // 被剔除
		// fpnt << left << setw(4);
		fpnt << controlerPlace[it->first].id << " " 
			 << initialMarkingCP[controlerPlace[it->first].name]
		     << " ";
		set<string, cmpByKey> pres = controlerPlace[it->first].Pres;
		for (auto it1 = pres.begin(); it1 != pres.end(); it1++)
		{
			if (it1 != prev(pres.end())) fpnt << (*it1).substr(1) << " ";
			else fpnt << (*it1).substr(1);
		}
		fpnt << ',';
		set<string,cmpByKey> posts = controlerPlace[it->first].Posts;
		for (auto it1 = posts.begin(); it1 != posts.end(); it1++)
		{
			if (it1 != prev(posts.end())) fpnt << (*it1).substr(1) << " ";
			else fpnt << (*it1).substr(1);
		}
		fpnt << endl;
	}
	fpnt << "@" << endl;

	fpnt << "PR ";
	for (auto it = PRafterCP.begin(); it != PRafterCP.end(); it++)
	{
		if(it != prev(PRafterCP.end())) fpnt << (*it).substr(1) << ",";
		else fpnt << (*it).substr(1) << endl;
	}
	fpnt << "PA ";
	for (auto it = PA.begin(); it != PA.end(); it++)
	{
		if (it != prev(PA.end())) fpnt << (*it).substr(1) << ",";
		else fpnt << (*it).substr(1) << endl;
	}

	// 显示和记录controler信息
	viewControler(fout);
	
	cout << endl;
	fout << endl;
	if (ret) {
		std::cout << "OK, read " << pntFile << "," << SESSIONina << endl;
		std::cout << "OK, output " << infoFile << "," << pntCPFile << endl;
		fout << "OK, read " << pntFile << "," << SESSIONina << endl;
		fout << "OK, output " << infoFile << "," << pntCPFile << endl;
	}
	else {
		std::cout << "Error, read " << pntFile << "," << SESSIONina << endl;
		std::cout << "Error, output " << infoFile << "," << pntCPFile << endl;
		fout << "Error, read " << pntFile << "," << SESSIONina << endl;
		fout << "Error, output " << infoFile << "," << pntCPFile << endl;
	}

	fpnt.close();
	fout.close();
	return ret;
}

// 添加控制器，填充数据成员controlerPlace和initialMarkingCP
bool Controler::addControler()
{
	int placeNum = getMaxPlaceNum();
	int i = 1;
	// 填充数据成员controlerPlace和initialMarkingCP
	for (auto it = ComSiphons.begin(); it != ComSiphons.end(); it++, i++) 
	{
		set<string,cmpByKey> comSiphon = it->second; // 信标补集
		set<string,cmpByKey> pres, posts; // 信标补集的前后置集
		for (string s : comSiphon)
		{
			pres.insert(ptNodes[s].Pres.begin(), ptNodes[s].Pres.end());
			posts.insert(ptNodes[s].Posts.begin(), ptNodes[s].Posts.end());
		}
		set<string, cmpByKey> pres1, posts1;  // pres1 = pres - posts, posts1 = post - pres
	    std::set_difference(pres.begin(), pres.end(), posts.begin(), posts.end(), inserter(pres1, pres1.end()), cmpByKey());
	    std::set_difference(posts.begin(), posts.end(), pres.begin(), pres.end(), inserter(posts1, posts1.end()), cmpByKey());
		// 控制库所的前置集是posts1，后置集是pres1
		ptNode cp;
		cp.type = CONTROLER;
		cp.id = placeNum + i; // 编号规则：原有Places最大序号后顺序排列
		cp.name.assign("P").append(to_string(cp.id));
		cp.cpKey = it->first; // controlerPlace/Siphons/ComSiphons/CPInfo key 一致, 统称cpKey
		cp.Pres.insert(posts1.begin(), posts1.end());
		cp.Posts.insert(pres1.begin(), pres1.end());
		controlerPlace.insert(make_pair(it->first,cp));

        // 控制库所marking是被清空信标的marking总和 - 1
		int marking = 0;
		for (string s : Siphons[it->first].siphon)
		{
			marking += initialMarking[s];
		}
		initialMarkingCP.insert(make_pair(cp.name, marking - 1));
	}
	return true;
}

/*********************************************
 * 从controlerPlace中筛选合法控制器,填充数据成员CPInfo
 * 从CPInfo提取有效控制器的资源库所至PRafterCP
 * 筛选规则：
 * (1) 剔除Pres和Posts为空的controlerPlace
 * (2) Pres和Posts一致的controlerPlace，取marking最小者。
 ***********************************************/
void Controler::legalRule()
{
	set<int> samed; // 存储controlerPlace key值, 记录是否已比较
	for (auto it = controlerPlace.begin(); it != controlerPlace.end(); it++)  // for#1
	{
		ControlerInfo info;
		info.cpKey = it->first;
		set<string> pres(it->second.Pres.begin(), it->second.Pres.end());
		set<string> posts(it->second.Posts.begin(),it->second.Posts.end()); 
		if (pres.empty() && posts.empty()) {
			info.isEmpty = true;
			CPInfo.insert(make_pair(it->first, info));
			continue; // 空值继续
		}
		if (samed.find(it->first) != samed.end())
			continue; // 已经有相同值，记录在案，不用比较了。

		// it,it1两两比较，如果pres和posts相同，取making小者为合法控制器
		for (auto it1 = next(it); it1 != controlerPlace.end(); it1++) // for#2
		{
			set<string> pres1(it1->second.Pres.begin(), it1->second.Pres.end());  // 由于使用Tools::sameSet(),这两个变量不能声明为<string,cmpByKey>
			set<string> posts1(it1->second.Posts.begin(), it1->second.Posts.end());
			if (pres1.empty() && posts1.empty()) continue; // 空值继续
			if (Tools::sameSet(pres, pres1) && Tools::sameSet(posts, posts1)) // 相同的pres和posts
			{
				samed.insert(it1->first);  // 记录已经比较
				info.sameCP.push_back(it1->first);
			}
		} // for#2 end
		// 没有相同的Pres和Posts，继续
		if (info.sameCP.empty()) {
			info.removed = false;
			CPInfo.insert(make_pair(it->first, info));
			continue;
		}
		//Pres和Posts一致的controlerPlace，取marking最小者。
		int smallerMarking = initialMarkingCP[it->second.name];
		int smallerId = it->first;
		for (int cpKey : info.sameCP) { // 找smallerId,smallerMarking
			string name = controlerPlace[cpKey].name;
			if (initialMarkingCP[name] < smallerMarking)
			{
				smallerId = cpKey;
				smallerMarking = initialMarkingCP[name];
			}
		}
		// it->first没有被剔除
		if (smallerId == it->first)
		{
			info.removed = false;
			CPInfo.insert(make_pair(it->first, info));
			// 考察info.sameCP，全部被剔除，被it->first取代
			for (auto it2 = info.sameCP.begin(); it2 != info.sameCP.end();it2++)
			{
				ControlerInfo info1;
				info1.cpKey = *it2;
				info1.sameCP.push_back(it->first);
				info1.removed = true;
				info1.replacedId = smallerId;
				for (auto it3 = info.sameCP.begin(); it3 != info.sameCP.end(); it3++)
				{   
					if (it3 != it2) info1.sameCP.push_back(*it3);
				}
				CPInfo.insert(make_pair(*it2, info1));
			}
		}
		// it->first被剔除，被smallerId取代
		else
		{
			info.removed = true;
			info.replacedId = smallerId;
			CPInfo.insert(make_pair(it->first, info));
			// 考察info.sameCP，被smallerId取代
			for (auto it2 = info.sameCP.begin(); it2 != info.sameCP.end(); it2++)
			{
				ControlerInfo info1;
				info1.cpKey = *it2;
				info1.sameCP.push_back(it->first);
				if (smallerId == *it2) {
					info1.removed = false;
				}
				else {
					info1.removed = true;
					info1.replacedId = smallerId;
				}
				for (auto it3 = info.sameCP.begin(); it3 != info.sameCP.end(); it3++)
				{
					if (it3 != it2) info1.sameCP.push_back(*it3);
				}
				CPInfo.insert(make_pair(*it2, info1));
			}
		}
	} // for#1 end

	// 从CPInfo提取有效控制器的资源库所至PRafterCP
	for (auto it = CPInfo.begin(); it != CPInfo.end(); it++)
	{
		if (it->second.removed || it->second.isEmpty) continue;
		PRafterCP.insert(controlerPlace[it->first].name);
	}
}

/************************************
 * 由.pnt和SESSION.ina求信标补集
 * const string& pntFile: 有资源库所和工序库所信息的pnt文件
 * const string& SESSION.ina 文件
 * const string& outFile: outpu file
 * 成功返回true，否则返回false
 ************************************/
bool Controler::comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile)
{
	clear();
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
			set<string,cmpByKey> comSiphon;
			ret = comSiphons(s.siphon, PR, comSiphon);
			if (!ret) break;
			ComSiphons.insert(make_pair(it->first, comSiphon));
		}
	}

	view(fout);

	cout << endl;
	fout << endl;
	if (ret) {
		std::cout << "OK, read " << pntFile << "," << SESSIONina << endl;
		std::cout << "OK, output " << outFile << endl;
		fout << "OK, read " << pntFile << "," << SESSIONina << endl;
		fout << "OK, output " << outFile << endl;
	}
	else {
		std::cout << "Error, read " << pntFile << "," << SESSIONina << endl;
		std::cout << "Error, output " << outFile << endl;
		fout << "Error, read " << pntFile << "," << SESSIONina << endl;
		fout << "Error, output " << outFile << endl;
	}

	fout.close();
	return ret;
}

/************************************
 * 根据信标和PR参数求信标补集
 * 输入参数:
 * const vector<string>& siphons: 信标集
 * const vector<string>& Pr: 资源库所集
 * 输出参数:
 * set<string>& comSiphon: 返回信标补集
 * 成功返回true, 否则，返回false(例如，补集为空 )
 ************************************/
bool Controler::comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string, cmpByKey>& comSiphon)
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

// 清除数据成员
void Controler::clear()
{
	XmlHelper::clear();
	PR.clear();
	PRafterCP.clear();
	PA.clear();
	ptNodes.clear();
	Siphons.clear();
	ComSiphons.clear();
	controlerPlace.clear();
	initialMarkingCP.clear();
	CPInfo.clear();
	PlacesAfterCP.clear();
}

// 显示和记录控制器的ptNode信息
void Controler::ptNodeView(ptNode node, ofstream &fout)
{
	if (node.type != CONTROLER) return;
	std::cout << node.cpKey << " : " << node.name << " " << initialMarkingCP[node.name] << "   ";
	fout << node.cpKey << " : " << node.name << " " << initialMarkingCP[node.name] << "   ";
	for (string s : node.Pres) {
		std::cout << s << " ";
		fout << s << " ";
	}
	std::cout << ",";
	fout << ",";
	for (string s : node.Posts) {
		std::cout << s << " ";
		fout << s << " ";
	}
	std::cout << endl;
	fout << endl;
	std::cout << node.cpKey << "    Siphon:";
	fout << node.id << "    Siphon:";
	Siphon s = Siphons[node.cpKey];
	vector<string> p = s.siphon;
	for (string s : p) {
		std::cout << s << " ";
		fout << s << " ";
	}
	std::cout << endl;
	fout << endl;

	std::cout << node.cpKey << " comSiphon:";
	fout << node.id << " comSiphon:";
	set<string, cmpByKey> com = ComSiphons[node.cpKey];
	for (string s : com) {
		std::cout << s << " ";
		fout << s << " ";
	}
	std::cout << endl;
	fout << endl;
}

// 显示和记录controler信息
void Controler::viewControler(ofstream &fout)
{
	view(fout);

	std::cout << "========Legal(Not removed)Controler:" << endl;
	fout << "========Legal(Not removed)Controler:" << endl;
	std::cout << "cpKey : P  M  Pres,Posts" << endl;
	fout << "cpKey : P  M  Pres,Posts" << endl;
	int legalNum = 0;
	for (auto it = controlerPlace.begin(); it != controlerPlace.end(); it++)
	{
		if (CPInfo[it->first].removed || CPInfo[it->first].isEmpty) continue;
		ptNodeView(it->second, fout);
		ControlerInfo info = CPInfo[it->first];
		int marking = initialMarkingCP[it->second.name];
		if (!info.sameCP.empty())
		{
			std::cout << "取代如下控制器([信标不同,补集相同,控制器Marking大的被剔除。]" << info.sameCP.size() << "个):" << endl;
			fout << "取代如下控制器([信标不同,补集相同,控制器Marking大的被剔除。]" << info.sameCP.size() << "个):" << endl;
			for (int cpKey : info.sameCP)
			{
				ptNodeView(controlerPlace[cpKey], fout);
				int marking1 = initialMarkingCP[controlerPlace[cpKey].name];
				if (marking == marking1) {
					std::cout << "***********marking相同(=" << marking << "),任取一个。" << endl;
					fout << "***********marking相同(=" << marking << "),任取一个。" << endl;
				}
				std::cout << "###" << endl;
				fout << "###" << endl;
			}
		}
		legalNum++;
		std::cout << "-------------------------------" << endl;
		fout << "-------------------------------" << endl;
	}
	//std::cout << "Legal(Not removed)Controler number:" << legalNum << endl;
	//fout << "Legal(Not removed)Controler number:" << legalNum << endl;

	/****************************************不用输出了，仅统计数量
	std::cout << "\n========Empty Controler:" << endl;
	fout << "\n========Empty Controler:" << endl;
	std::cout << "cpKey : P  M  Pres,Posts" << endl;
	fout << "cpKey : P  M  Pres,Posts" << endl;
	*****************************************/
	int emptyNum = 0;
	for (auto it = controlerPlace.begin(); it != controlerPlace.end(); it++)
	{
		if (!CPInfo[it->first].isEmpty) continue;
		emptyNum++;
		/****************************************不用输出了，仅统计数量
		ptNodeView(it->second, fout);
		std::cout << "-------------------------------" << endl;
		fout << "-------------------------------" << endl;
		*****************************************/
	}

	// 非空控制器被剔除，由replacedId取代
	/****************************************不用输出了，仅统计数量
	std::cout << "\n========Removed Controler:" << endl;
	fout << "\n========Removed Controler:" << endl;
	std::cout << "cpKey : P  M  Pres,Posts" << endl;
	fout << "cpKey : P  M  Pres,Posts" << endl;
	*****************************************/
	int removedNum = 0;
	for (auto it = controlerPlace.begin(); it != controlerPlace.end(); it++)
	{
		if (!CPInfo[it->first].removed || CPInfo[it->first].isEmpty) continue;
		removedNum++;
		/****************************************不用输出了，仅统计数量
		ptNodeView(it->second, fout);
		ControlerInfo info = CPInfo[it->first];
		std::cout << "Replaced by:" << endl;
		fout << "Replaced by:" << endl;
		ptNodeView(controlerPlace[info.replacedId], fout);
		std::cout << "**************************" << endl;
		fout << "**************************" << endl;
		if (!info.sameCP.empty())
		{
			std::cout << "it's a same as(" << info.sameCP.size() << "):" << endl;
			fout << "it's a same as(" << info.sameCP.size() << "):" << endl;
			for (int cpKey : info.sameCP)
			{
				ptNodeView(controlerPlace[cpKey], fout);
				std::cout << "###" << endl;
				fout << "###" << endl;
			}
		}
		std::cout << "-------------------------------" << endl;
		fout << "-------------------------------" << endl;
		****************************************/
	}

	std::cout << "\nTotal controller: " << controlerPlace.size() << ", Legal: " << legalNum << ", Empty: " << emptyNum << ", Removed: " << removedNum << endl;
	fout << "\nTotal controller: " << controlerPlace.size() << ", Legal: " << legalNum << ", Empty: " << emptyNum << ", Removed: " << removedNum << endl;

	std::cout << "\nLegal controler(marking):" << endl;
	fout << "\nLegal controler(marking):" << endl;
	for (auto it = controlerPlace.begin(); it != controlerPlace.end(); it++)
	{
		if (CPInfo[it->first].removed || CPInfo[it->first].isEmpty) continue;
		std::cout << it->second.name << "(" << initialMarkingCP[it->second.name] << "),";
		fout << it->second.name << "(" << initialMarkingCP[it->second.name] << "),";
	}
	std::cout << endl << endl;
	fout << endl << endl;

	// 添加控制器以后, 保留下来的原有Places
	std::cout << "\n添加控制器以后, 保留下来的原有Places(marking):" << endl;
	fout << "\n添加控制器以后, 保留下来的原有Places(marking):" << endl;
	for (auto it = PlacesAfterCP.begin(); it != PlacesAfterCP.end(); it++)
	{
		if (next(it) != PlacesAfterCP.end()) {
			std::cout << (*it) << "(" << initialMarking[*it] << "),";
			fout << (*it) << "(" << initialMarking[*it] << "),";
		}
		else {
			std::cout << (*it) << "(" << initialMarking[*it] << ")" << endl;
			fout << (*it) << "(" << initialMarking[*it] << ")" << endl;
		}
	}
	std::cout << endl;
	fout << endl;

	std::cout << "\nBefore add controller PR ";
	fout << "\nBefore add controller PR ";
	for (auto it = PR.begin(); it != PR.end(); it++)
	{
		if (it != prev(PR.end())) {
			cout << (*it).substr(1) << ",";
			fout << (*it).substr(1) << ",";
		}
		else {
			cout << (*it).substr(1) << endl;
			fout << (*it).substr(1) << endl;
		}
	}

	std::cout << "After add controller PR ";
	fout << "After add controller PR ";
	for (auto it = PRafterCP.begin(); it != PRafterCP.end(); it++)
	{
		if (it != prev(PRafterCP.end())) {
			cout << (*it).substr(1) << ",";
			fout << (*it).substr(1) << ",";
		}
		else {
			cout << (*it).substr(1) << endl;
			fout << (*it).substr(1) << endl;
		}
	}
	std::cout << "\nPA ";
	fout << "\nPA ";
	for (auto it = PA.begin(); it != PA.end(); it++)
	{
		if (it != prev(PA.end())) {
			cout << (*it).substr(1) << ",";
			fout << (*it).substr(1) << ",";
		}
		else {
			cout << (*it).substr(1) << endl;
			fout << (*it).substr(1) << endl;
		}
	}
}

// 显示和记录信息
void Controler::view(ofstream &fout)
{
	XmlHelper::view(fout);
	std::cout << "\nBefore add controller: \nP M Pres,Posts" << endl;
	fout << "\nBefore add controller: \nP M Pres,Posts" << endl;
	for (auto it = Places.begin(); it != Places.end(); it++)
	{
		std::cout << *it << " " << initialMarking[*it] << "   ";
		fout << *it << " " << initialMarking[*it] << "   ";
		for (auto it1 = ptNodes[*it].Pres.begin(); it1 != ptNodes[*it].Pres.end(); it1++)
		{
			if (it1 != prev(ptNodes[*it].Pres.end())) {
				std::cout << *it1 << " ";
				fout << *it1 << " ";
			}
			else {
				std::cout << *it1 << ",";
				fout << *it1 << ",";
			}
		}
		for (auto it1 = ptNodes[*it].Posts.begin(); it1 != ptNodes[*it].Posts.end(); it1++)
		{
			if (it1 != prev(ptNodes[*it].Posts.end())) {
				std::cout << *it1 << " ";
				fout << *it1 << " ";
			}
			else {
				std::cout << *it1;
				fout << *it1;
			}
		}
		std::cout << endl;
		fout << endl;
	}

	std::cout << "\nBefore add controller PR ";
	fout << "\nBefore add controller PR ";
	for (string s : PR) {
		std::cout << s << " ";
		fout << s << " ";
	}
	cout << endl;
	fout << endl;
	std::cout << "PA ";
	fout << "PA ";
	for (string s : PA) {
		std::cout << s << " ";
		fout << s << " ";
	}
	cout << endl << endl;
	fout << endl << endl;

	std::cout << "Siphons and empty clean ComSiphon:" << endl;
	fout << "Siphons and empty clean ComSiphons:" << endl;
	int cleanNum = 0;
	for (auto it = Siphons.begin(); it != Siphons.end(); it++)
	{
		if (it->second.clean) {
			cout << it->first << ":[empty clean] ";
			fout << it->first << ":[empty clean] ";
			cleanNum++;
		}
		else {
			cout << it->first << ": ";
			fout << it->first << ": ";
		}
		for (string place : it->second.siphon) {
			cout << place << " ";
			fout << place << " ";
		}
		if (it->second.clean) 
		{
			std::cout << "\nComSiphon: ";
			fout << "\nComSiphon: ";
			cout << it->first << ": ";
			fout << it->first << ": ";
			for (string s : ComSiphons[it->first]) {
				cout << s << " ";
				fout << s << " ";
			}
		}
		cout << endl;
		fout << endl;
	}
	std::cout << "===Siphons number:" << Siphons.size() << ", Clean number:" << cleanNum << endl;
	fout << "===Siphons number:" << Siphons.size() << ", Clean number:" << cleanNum << endl;
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
	set<string,cmpByKey> pres(ptNodes[ptName].Pres);
	for (auto it = pres.begin(); it != pres.end(); it++)
	{
		set<string,cmpByKey> prepre(ptNodes[*it].Pres);
		for (auto it1 = prepre.begin(); it1 != prepre.end(); it1++)
		{
			prePre.insert(*it1);
			ret = true;
		}
	}
	return ret;
}

/************************************
 * 填充数据成员ptNodesy以及node成员Pres，Posts
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
		// 处理pre,post，填充数据成员: Transitions,source,target,weight
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
		if (line.empty()) continue; // 空行继续，允许文件末尾有空行
		if (!PRPA(line)) { 
			ret = false; 
			break; 
		}
	}

	if (PR.empty() || PA.empty()) {
		std::cout << "Error format pnt file, none PR or PA: " << pntFile << endl;
		ret = false;
	}

	if (!ret) {
		std::cout << "Error format pnt file: " << pntFile << endl;
		fin.close();
		return false;
	}
	
	// 排序 Places,Transitions
	std::sort(Places.begin(), Places.end(), cmpByKey());
	std::sort(Transitions.begin(), Transitions.end(), cmpByKey());
	//填充数据成员ptNodesy以及node成员Pres，Posts
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
