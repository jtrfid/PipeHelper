#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<set>

using namespace std;

/***************************************************
 * map key compare(Ҫ���ϸ�����,�����Ԫ��,����false)
 * include compare string's length
 * ��������map��keys���磺"P1","P2","P10",...
 * �÷���
 * ��1��map��ֵ����map<string,int,cmpByKey> map1;
 * ��2��set����: set<string,cmpByKey> set1;
 * ��3��vector<string>����
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

/***************************************************
 * pari<string,int> compare
 * include compare string's length
 * ��������map��keys���磺"P1","P2","P10",...
 * �÷���map<string,int> map1;
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
	/////////////////////// ͨ�þ�̬����
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
	 * ��1��ȥ��ǰ��׺�ո�('\n','\r','\t',' ')
	 * ��2��ɾ���ַ����ڵ�'\n'
	 * ��3���ַ����������ո������һ���ո�
	 * �������ش������ַ���
	 * after deleted, if str is empty,return true, else return false
	 *******************************************/
	static bool isSpaceLine(string &str)
	{
		if (str.empty()) return true;

		int count = 0;
		// ɾ��ǰ׺�ո�
		for (string::iterator it = str.begin(); it != str.end(); it++)
		{
			if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') count++;
			else break;
		}
		str.erase(0, count);
		// ɾ����׺�ո�
		count = 0;
		for (string::const_reverse_iterator it = str.rbegin(); it != str.rend(); it++)
		{
			if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') count++;
			else break;
		}
		str.erase(str.length() - count);
		// ɾ���ַ����ڵ�'\n'
		string::size_type pos;
		while (1) {
			pos = str.find('\n');
			if (pos == string::npos) break; // û�ҵ�
			str.erase(pos, 1);
		}
		// �ַ����������ո������һ���ո�
		string::size_type pos1 = 0;
		while (1) {
			pos = str.find(' ', pos1); // ��pos1��ʼ�� 
			if (pos == string::npos) break;
			count = 0;  // ����ո���
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
	 * ȥ��ǰ��׺�ո�('\n','\r','\t',' ')
	 *************************************/
	static string& trim(string& s)
	{
		if (s.empty()) return s;
		// ȥ��s��ǰ׺�ո�
		string::size_type pos = 0;
		for (auto it = s.begin(); it < s.end(); it++) {
			if (*it == '\n' || *it == '\r' || *it == '\t' || *it == ' ') pos++;
			else break;
		}
		s = s.substr(pos);

		// ɾ��s�ĺ�׺�ո�
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
	 * ����ȥ��s��ǰ��׺�ո�
	 * Ȼ����ȡ��ָ���ָ������������ַ�����sVector.
	 * ���ұ�֤�����Ӵ�ǰ���޿ո�
	 * �մ���ȥ��ǰ��׺�Ŀո���ǿմ���ֱ�ӷ���
	 *************************************/
	static vector<string>& split(const string& s, vector<string>& sVector, char delimit)
	{
		if (s.empty()) return sVector;
		string str(s); // ����s��str
		trim(str); // ȥ��ǰ��׺�ո�('\n','\r','\t',' ')
		if (str.empty()) return sVector;
		string::size_type pos1 = 0, pos2 = 0;
		while (1)
		{
			pos2 = str.find(delimit,pos1); // ��pos1��ʼ�ҷָ���
			if (pos2 == string::npos)  break; // δ�ҵ��ָ���
			string sub = str.substr(pos1, pos2 - pos1);
			trim(sub);
			if (sub.empty()) std::cout << "Waring, have empty sub string in " << s << endl;
			sVector.push_back(sub);
			pos1 = pos2 + 1;
		}
		// ������һ��
		string sub = str.substr(pos1);
		trim(sub);
		if (sub.empty()) std::cout << "Waring, have empty splited substring in " << s << endl;
		sVector.push_back(sub);

		return sVector;
	}

	/*************************************
	 * ��ȡ�ļ���չ�����磬".docx")
	 * ����ext�����ļ���չ��
	 * ����չ������������false; ���򷵻�true
	 *************************************/
	static bool extFile(const string& file, string& ext)
	{
		string::size_type pos;
		pos = file.find_last_of('.');
		if (pos == string::npos) return false;
		ext = file.substr(pos);
		return true;
	}

	/*************************************
	 * �ַ����е�Сд��ĸתΪ��д��ĸ
	 *************************************/
	static string& toUpper(string &s)
	{
		for (string::iterator it = s.begin(); it != s.end(); ++it)
			if (*it >= 'a' && *it <= 'z') *it = *it - ('a' - 'A');
		return s;
	}

	/*************************************
	 * ����C�����ַ����⺯�������Դ�Сд���Ƚ������ַ���
	 * s1 > s2, s1= s2, s1 < s2 �ֱ𷵻�: ����0,0,С��0������ 
	 *************************************/
	static int compareI(const string& s1, const string& s2)
	{
		return _strcmpi(s1.c_str(), s2.c_str());
	}


	/********************************************
	 ������������Ƿ����:
	 ��ʵ�Ͽ����ö�����ͷ�ļ� <algorithm>���㷨��
	 (1) �����Χ [first1, last1) �ͷ�Χ [first2, last2) ��ȣ����� true �����򷵻� false
	 template< class InputIt1, class InputIt2 >
	 bool equal( InputIt1 first1, InputIt1 last1,
	 InputIt2 first2, InputIt2 last2 );

	 (2) �����Χ [first1, last1) �ͷ�Χ [first2, first2 + (last1 - first1) ��ȣ����� true �����򷵻� false
	 template< class InputIt1, class InputIt2 >
	 bool equal( InputIt1 first1, InputIt1 last1, InputIt2 first2 );
	********************************************/
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

