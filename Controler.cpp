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
 * ��.pnt��SESSION.ina�ļ�����������������µĺ��п�������pnt�ļ�
 * const string& infoFile: information file��Ϣ�ļ�
 * const string& pntCPFile: �����µĺ��п�������pnt�ļ�
 * �ɹ�����true�����򷵻�false
 ************************************/
bool Controler::pntCP(const string& pntFile, const string& SESSIONina, const string& infoFile, const string& pntCPFile)
{
	clear();
	bool ret = true;
	ofstream fout(infoFile);
	ofstream fpnt(pntCPFile);

	//��.pnt��SESSION.ina���ű겹��
	ret = comSiphons(pntFile, SESSIONina, infoFile);
	// ��ӿ�������������ݳ�ԱcontrolerPlace��initialMarkingCP
	addControler();
	//��controlerPlace��ɸѡ�Ϸ�������,������ݳ�ԱlegalControler
	legalRule();

	// ����pnt�ļ�
	fpnt << "p  m   pre,pos"<< endl;
	for (auto it = Places.begin(); it != Places.end(); it++)
	{
		// ��ȥԭ��PR
		auto ft = find(PR.begin(), PR.end(), *it);
		if (ft != PR.end()) continue;  // *it������PR��

		// ��ӿ������Ժ�, ����������ԭ��Places
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
		if (it->second.removed || it->second.isEmpty) continue; // ���޳�
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

	// ��ʾ�ͼ�¼controler��Ϣ
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

// ��ӿ�������������ݳ�ԱcontrolerPlace��initialMarkingCP
bool Controler::addControler()
{
	int placeNum = getMaxPlaceNum();
	int i = 1;
	// ������ݳ�ԱcontrolerPlace��initialMarkingCP
	for (auto it = ComSiphons.begin(); it != ComSiphons.end(); it++, i++) 
	{
		set<string,cmpByKey> comSiphon = it->second; // �ű겹��
		set<string,cmpByKey> pres, posts; // �ű겹����ǰ���ü�
		for (string s : comSiphon)
		{
			pres.insert(ptNodes[s].Pres.begin(), ptNodes[s].Pres.end());
			posts.insert(ptNodes[s].Posts.begin(), ptNodes[s].Posts.end());
		}
		set<string, cmpByKey> pres1, posts1;  // pres1 = pres - posts, posts1 = post - pres
	    std::set_difference(pres.begin(), pres.end(), posts.begin(), posts.end(), inserter(pres1, pres1.end()), cmpByKey());
	    std::set_difference(posts.begin(), posts.end(), pres.begin(), pres.end(), inserter(posts1, posts1.end()), cmpByKey());
		// ���ƿ�����ǰ�ü���posts1�����ü���pres1
		ptNode cp;
		cp.type = CONTROLER;
		cp.id = placeNum + i; // ��Ź���ԭ��Places�����ź�˳������
		cp.name.assign("P").append(to_string(cp.id));
		cp.cpKey = it->first; // controlerPlace/Siphons/ComSiphons/CPInfo key һ��, ͳ��cpKey
		cp.Pres.insert(posts1.begin(), posts1.end());
		cp.Posts.insert(pres1.begin(), pres1.end());
		controlerPlace.insert(make_pair(it->first,cp));

        // ���ƿ���marking�Ǳ�����ű��marking�ܺ� - 1
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
 * ��controlerPlace��ɸѡ�Ϸ�������,������ݳ�ԱCPInfo
 * ��CPInfo��ȡ��Ч����������Դ������PRafterCP
 * ɸѡ����
 * (1) �޳�Pres��PostsΪ�յ�controlerPlace
 * (2) Pres��Postsһ�µ�controlerPlace��ȡmarking��С�ߡ�
 ***********************************************/
void Controler::legalRule()
{
	set<int> samed; // �洢controlerPlace keyֵ, ��¼�Ƿ��ѱȽ�
	for (auto it = controlerPlace.begin(); it != controlerPlace.end(); it++)  // for#1
	{
		ControlerInfo info;
		info.cpKey = it->first;
		set<string> pres(it->second.Pres.begin(), it->second.Pres.end());
		set<string> posts(it->second.Posts.begin(),it->second.Posts.end()); 
		if (pres.empty() && posts.empty()) {
			info.isEmpty = true;
			CPInfo.insert(make_pair(it->first, info));
			continue; // ��ֵ����
		}
		if (samed.find(it->first) != samed.end())
			continue; // �Ѿ�����ֵͬ����¼�ڰ������ñȽ��ˡ�

		// it,it1�����Ƚϣ����pres��posts��ͬ��ȡmakingС��Ϊ�Ϸ�������
		for (auto it1 = next(it); it1 != controlerPlace.end(); it1++) // for#2
		{
			set<string> pres1(it1->second.Pres.begin(), it1->second.Pres.end());  // ����ʹ��Tools::sameSet(),������������������Ϊ<string,cmpByKey>
			set<string> posts1(it1->second.Posts.begin(), it1->second.Posts.end());
			if (pres1.empty() && posts1.empty()) continue; // ��ֵ����
			if (Tools::sameSet(pres, pres1) && Tools::sameSet(posts, posts1)) // ��ͬ��pres��posts
			{
				samed.insert(it1->first);  // ��¼�Ѿ��Ƚ�
				info.sameCP.push_back(it1->first);
			}
		} // for#2 end
		// û����ͬ��Pres��Posts������
		if (info.sameCP.empty()) {
			info.removed = false;
			CPInfo.insert(make_pair(it->first, info));
			continue;
		}
		//Pres��Postsһ�µ�controlerPlace��ȡmarking��С�ߡ�
		int smallerMarking = initialMarkingCP[it->second.name];
		int smallerId = it->first;
		for (int cpKey : info.sameCP) { // ��smallerId,smallerMarking
			string name = controlerPlace[cpKey].name;
			if (initialMarkingCP[name] < smallerMarking)
			{
				smallerId = cpKey;
				smallerMarking = initialMarkingCP[name];
			}
		}
		// it->firstû�б��޳�
		if (smallerId == it->first)
		{
			info.removed = false;
			CPInfo.insert(make_pair(it->first, info));
			// ����info.sameCP��ȫ�����޳�����it->firstȡ��
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
		// it->first���޳�����smallerIdȡ��
		else
		{
			info.removed = true;
			info.replacedId = smallerId;
			CPInfo.insert(make_pair(it->first, info));
			// ����info.sameCP����smallerIdȡ��
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

	// ��CPInfo��ȡ��Ч����������Դ������PRafterCP
	for (auto it = CPInfo.begin(); it != CPInfo.end(); it++)
	{
		if (it->second.removed || it->second.isEmpty) continue;
		PRafterCP.insert(controlerPlace[it->first].name);
	}
}

/************************************
 * ��.pnt��SESSION.ina���ű겹��
 * const string& pntFile: ����Դ�����͹��������Ϣ��pnt�ļ�
 * const string& SESSION.ina �ļ�
 * const string& outFile: outpu file
 * �ɹ�����true�����򷵻�false
 ************************************/
bool Controler::comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile)
{
	clear();
	bool ret = true;
	ofstream fout(outFile);
	// read ����Դ�����͹��������Ϣ��pnt�ļ���������ݳ�Ա
	readPnt(pntFile);
	//��ȡSESSION.ina, ������ݳ�ԱSiphons
	sessionIna(SESSIONina);
	// ����ű겹��ComSiphons
	for (auto it = Siphons.begin(); it != Siphons.end(); it++)
	{
		Siphon s = it->second;
		if (s.clean) {  // ��յ��ű�
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
 * �����ű��PR�������ű겹��
 * �������:
 * const vector<string>& siphons: �ű꼯
 * const vector<string>& Pr: ��Դ������
 * �������:
 * set<string>& comSiphon: �����ű겹��
 * �ɹ�����true, ���򣬷���false(���磬����Ϊ�� )
 ************************************/
bool Controler::comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string, cmpByKey>& comSiphon)
{
	bool ret = true;
	set<string> unionHr;
	ret = UnionHr(siphons, Pr, unionHr);
	// �
	for (auto it = unionHr.begin(); it != unionHr.end(); it++)
	{
		auto result = std::find(std::begin(siphons), std::end(siphons), *it);
		if (result != std::end(siphons)) continue; // ��siphons��
		comSiphon.insert(*it);
	}
	if (unionHr.empty()) ret = false;
	if (!ret) cout << "\nError, in function comSiphons()." << endl;
	return ret;
}

/************************************
 * Union H(r), r in Siphon��PR�Ľ���
 * �������:
 * const vector<string>& siphons: �ű꼯
 * const vector<string>& Pr: ��Դ������
 * �������:
 * set<string>& unionHr: ����Union H(r)
 * �ɹ�����true, ���򣬷���false(���磬HolderΪ�ջ�unionHrΪ�� )
 ************************************/
bool Controler::UnionHr(const vector<string>& siphons, const set<string>& Pr, set<string>& unionHr)
{
	bool ret = true;
	for (auto it = siphons.begin(); it != siphons.end(); it++)
	{
		auto result = std::find(std::begin(Pr),std::end(Pr), *it);
		if (result == std::end(Pr)) continue; // ����PR��
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
 * ��ȡ��Դ��Holder, resourceǰ�ü���ǰ�ü���PA�Ľ���: H(r)
 * �������:
 * const string& resource: place name
 * �������:
 * set<string>& H: ����resource��holder
 * ���holder�ǿգ���������true, ���򷵻�false
 ************************************/
bool Controler::holder(const string& resource, set<string>& H)
{
	H.clear();
	bool ret = false;
	set<string> prepres;
	if (!prePre(resource, prepres)) {
		return false;
	}
	// ����������ǰ�����������
	std::set_intersection(prepres.begin(), prepres.end(), PA.begin(), PA.end(), inserter(H, H.end()));
	//std::set_intersection(prepres.begin(), prepres.end(), PA.begin(), PA.end(), inserter(H, H.begin())); // ��Ч
	if (H.empty()) ret = false;
	else ret = true;
	if (!ret) cout << "\nError, in function holder()." << endl;
	return ret;
}

// ������ݳ�Ա
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

// ��ʾ�ͼ�¼��������ptNode��Ϣ
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

// ��ʾ�ͼ�¼controler��Ϣ
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
			std::cout << "ȡ�����¿�����([�ű겻ͬ,������ͬ,������Marking��ı��޳���]" << info.sameCP.size() << "��):" << endl;
			fout << "ȡ�����¿�����([�ű겻ͬ,������ͬ,������Marking��ı��޳���]" << info.sameCP.size() << "��):" << endl;
			for (int cpKey : info.sameCP)
			{
				ptNodeView(controlerPlace[cpKey], fout);
				int marking1 = initialMarkingCP[controlerPlace[cpKey].name];
				if (marking == marking1) {
					std::cout << "***********marking��ͬ(=" << marking << "),��ȡһ����" << endl;
					fout << "***********marking��ͬ(=" << marking << "),��ȡһ����" << endl;
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

	/****************************************��������ˣ���ͳ������
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
		/****************************************��������ˣ���ͳ������
		ptNodeView(it->second, fout);
		std::cout << "-------------------------------" << endl;
		fout << "-------------------------------" << endl;
		*****************************************/
	}

	// �ǿտ��������޳�����replacedIdȡ��
	/****************************************��������ˣ���ͳ������
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
		/****************************************��������ˣ���ͳ������
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

	// ��ӿ������Ժ�, ����������ԭ��Places
	std::cout << "\n��ӿ������Ժ�, ����������ԭ��Places(marking):" << endl;
	fout << "\n��ӿ������Ժ�, ����������ԭ��Places(marking):" << endl;
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

// ��ʾ�ͼ�¼��Ϣ
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
 * ��ȡ�ڵ��ǰ�ü���ǰ�ü�
 * �������:
 * const &string ptName: place/transition name
 * �������:
 * set<string> &prePre: ����ptName��ǰ�ü���ǰ�ü�
 * �����ǰ�ü���ǰ�ü�����������true, ���򷵻�false
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
 * ������ݳ�ԱptNodesy�Լ�node��ԱPres��Posts
 ************************************/
void Controler::PrePosts()
{
	for (map<string, string>::const_iterator it = source.begin(); it != source.end(); it++)
	{
		// it->second�ĺ��ü����target[it->first]
		auto it1 = ptNodes.find(it->second);
		if(it1 != ptNodes.end()) // �ҵ�
		{
			ptNode n = ptNodes[it->second];
			n.Posts.insert(target[it->first]); 
			ptNodes[it->second] = n; // key �Ѵ��ڣ���ֵ���Ǿ�ֵ, operator =
		}
		else // û�ҵ�
		{
			ptNode n;
			n.Posts.insert(target[it->first]);
			ptNodes.insert(make_pair(it->second, n)); // key ������, insert
		}
		// target[it->first]��ǰ�ü����it->second
		auto it2 = ptNodes.find(target[it->first]);
		if (it2 != ptNodes.end()) // �ҵ�
		{
			ptNode n = ptNodes[target[it->first]];
			n.Pres.insert(it->second);
			ptNodes[target[it->first]] = n; // key �Ѵ��ڣ���ֵ���Ǿ�ֵ, operator =
		}
		else // û�ҵ�
		{
			ptNode n;
			n.Pres.insert(it->second);
			ptNodes.insert(make_pair(target[it->first], n)); // key ������,insert
		}
	}
}

/*****************************************
 * read ����Դ�����͹��������Ϣ��pnt�ļ���������ݳ�Ա
 * const string& pntFile: input pnt file
 * �ɹ�����true�����򷵻�false
 *
 * pnt file format: (�������Ҫ��':'ǰ���޿ո�)
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
	// ������ݳ�Ա
	clear();

	// ��ȡpnt�ļ���������ݳ�Ա
	string line; // ÿ���ַ���
	int lineNum = 0;
	while (!fin.eof())
	{
		lineNum++;
		getline(fin, line);
		// std::cout << line << endl;
		if (lineNum == 1)  continue;               // first line: eg. P   M   PRE,POST 
		if (line.find('@') != string::npos) break; // end flag

												   // ÿ�����������ַ���
		string parts[3]; // 3�����ַ�����P = parts[0], M = parts[2], Pre,Post = parts[3];
		if (!parts3(line, parts)) { ret = false;	break; }

		// pre,post
		string::size_type pos;
		string pre;
		string post;
		pos = parts[2].find(',');
		if (pos == string::npos) { ret = false;	break; }
		pre = parts[2].substr(0, pos);
		post = parts[2].substr(pos + 1);

		// place ���ǰ׺'P',����"P1","P2",...
		string place("P");
		place.append(parts[0]);
		// ����pre,post��������ݳ�Ա: Transitions,source,target,weight
		prePost(place, pre, true);
		prePost(place, post, false);
		// ������ݳ�Ա: place,initialMarking,capacity
		Places.push_back(place);
		initialMarking.insert(make_pair(place, atoi(parts[1].c_str())));
		capacity.insert(make_pair(place, 0)); // capacity = 0;
	}

	if (fin.eof()) {
		ret = false;
		std::cout << "Error(��PR��PA��)!! read " << pntFile << endl;
	}
	// PR and PA
	while (!fin.eof())
	{
		getline(fin, line);
		if (line.empty()) continue; // ���м����������ļ�ĩβ�п���
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
	
	// ���� Places,Transitions
	std::sort(Places.begin(), Places.end(), cmpByKey());
	std::sort(Transitions.begin(), Transitions.end(), cmpByKey());
	//������ݳ�ԱptNodesy�Լ�node��ԱPres��Posts
	PrePosts();
	fin.close();
	return true;
}

/************************************
 * ����PR or PA line
 * �ɹ�����true�����򷵻�false
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
		// ���һ��place
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
 * read SESSION.ina�ļ���������ݳ�ԱSiphons
 * const string& SESSION.ina�ļ���
 * �ɹ�����true�����򷵻�false
 * 
 * SESSION.ina�ļ���ʽ
 * There are  23 minimal deadlocks:��Ȼ
 * -------------------------------��Ȼ
 *  1:  8,  9, 10, 11, 12, 13, 14,
 *  2:  2,  9, 15,
 *  3:  7,  9, 15, 16, 17, 18, 19, 20,
 *  4:  7, 10, 16, 17, 18, 19, 20,
 *  ....
 *  ��Ȼ��һ������
 * Corresponding maximal traps:��Ȼ
 * ----------------------------��Ȼ
 *  1:  8,  9, 10, 11, 12, 13, 14,
 *  2:  2,  9, 15,
 *  3: empty  clean
 *  4:  7, 10, 16, 17, 18, 19, 20,
 *  ....
 *  ��Ȼ��һ������
 *****************************************/
bool Controler::sessionIna(const string& SESSIONina)
{
	bool ret = true;
	ifstream fin(SESSIONina);
	if (!fin) {
		std::cout << "Error!! read " << SESSIONina << endl;
		return false;
	}
	// ��ȡpnt�ļ���������ݳ�ԱSiphons
	Siphons.clear(); // ��վ�����
	string line; // ÿ���ַ���
	bool start1 = false, start2 = false;
	string::size_type pos;
	int id = 1;
	///////// ��һ���֣�ȫ���ű�
	while (!fin.eof())
	{
		getline(fin, line);
		if (!start1)
		{
			if (line.find("------") != string::npos) start1 = true;
			continue;
		}
		if (line.empty()) break; // ���У���һ���ֽ���
		// cout << "id = "<< id <<","<<line << endl;
		pos = line.find(':'); 
		if (pos == string::npos) { ret = false; break; } // ��ʽ����
		string s = line.substr(pos + 1);
		Tools::trim(s); // ȥ��ǰ��׺�ո�
		if (s.back() == ',') s.erase(prev(s.end()));// ��������','ɾ��֮
		// cout << s << endl;
		// ����ű�
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
	///////// �ڶ����֣��ű�������
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
				else {	ret = false; break;	} // ��ʽ����
			}
			continue;
		}
		if (line.empty()) break; // ���н���
		// cout << line << endl;
		if (line.find("empty  clean") != string::npos) { // id ��Ӧ���ű걻���
			Siphons[id].clean = true;
			// cout << "clean id="<<id<<endl;
		}
		id++;
	}
	if (fin.eof()) ret = false;  // ��Ӧ�þʹ˽���

	if(!ret) cout << "Error farmat in file: " << SESSIONina << endl;
	fin.close();
	return ret;
}
