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
* �ű�,������
* const string& pntFile: ����Դ�����͹��������Ϣ��pnt�ļ�
* const string& SESSION.ina �ļ�
* const string& outFile: outpu file
* �ɹ�����true�����򷵻�false
************************************/
bool Controler::comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile)
{
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
 * �ű겹��
 * �������:
 * const vector<string>& siphons: �ű꼯
 * const vector<string>& Pr: ��Դ������
 * �������:
 * set<string>& comSiphon: �����ű겹��
 * �ɹ�����true, ���򣬷���false(���磬����Ϊ�� )
 ************************************/
bool Controler::comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string>& comSiphon)
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

// ��ʾ�ͼ�¼��Ϣ
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
 * ������ݳ�ԱPres��Posts
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
		// ����pre,post
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
		// ���� Places,Transitions
		std::sort(Places.begin(), Places.end(), cmpByKey());
		std::sort(Transitions.begin(), Transitions.end(), cmpByKey());
	}

	//������ݳ�ԱPres��Posts
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
