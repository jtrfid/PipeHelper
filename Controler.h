#pragma once
#include <set>
#include "XmlHelper.h"

struct ptNode
{
	set<string> Pres;
	set<string> Posts;
};

struct Siphon
{
	bool clean;    // ture: ����գ�false: �����
	vector<string> siphon;
};

class Controler :
	public XmlHelper
{
public:
	Controler();
	~Controler();

	/************************************
	 * �ű겹��
	 * const string& pntFile: ����Դ�����͹��������Ϣ��pnt�ļ�
	 * const string& SESSION.ina �ļ�
	 * const string& outFile: outpu file
	 * �ɹ�����true�����򷵻�false
	 ************************************/
	bool comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile);

	/************************************
	 * �ű겹��
	 * �������:
	 * const vector<string>& siphons: �ű꼯
	 * const vector<string>& Pr: ��Դ������
	 * �������:
	 * set<string>& comSiphon: �����ű겹��
	 * �ɹ�����true, ���򣬷���false(���磬����Ϊ�� )
	 ************************************/
	bool comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string>& comSiphon);

	/************************************
	 * Union H(r), r in Siphon��PR�Ľ���
	 * �������:
	 * const vector<string>& siphons: �ű꼯
	 * const set<string>& Pr: ��Դ������
	 * �������:
	 * set<string>& unionHr: ����Union H(r)
	 * ���holder�ǿգ���������true, ���򷵻�false
	 ************************************/
	bool UnionHr(const vector<string>& siphons, const set<string>& Pr, set<string>& unionHr);
	/************************************
	 * ��ȡ��Դ��Holder, resourceǰ�ü���ǰ�ü���PA�Ľ���: H(r)
	 * �������:
	 * const string& resource: place name
	 * �������:
	 * set<string>& H: ����resource��holder
	 * ���holder�ǿգ���������true, ���򷵻�false
	 ************************************/
	bool holder(const string& resource, set<string>& H);

	/************************************
	 * read ����Դ�����͹��������Ϣ��pnt�ļ���������ݳ�Ա
	 * const string& pntFile: input pnt file
	 * �ɹ�����true�����򷵻�false
	 *************************************/
	bool readPnt(const string &pntFile);

	/************************************
	 * ��ýڵ��ǰ�ü���ǰ�ü�
	 * �������:
	 * const &string ptName: place/transition name
	 * �������:
	 * set<string> prePre: ����ptName��ǰ�ü���ǰ�ü�
	 * �����ǰ�ü���ǰ�ü�����������true, ���򷵻�false
	 ************************************/
	bool prePre(const string& ptName, set<string> &prePre);

	/*****************************************
	 * ��ȡSESSION.ina, ������ݳ�Ա Siphons
	 * const string& SESSION.ina�ļ���
	 * �ɹ�����true�����򷵻�false
	 *****************************************/
	bool sessionIna(const string& SESSIONina);

protected:
	// ��ʾ�ͼ�¼��Ϣ
	virtual void view(ofstream &fout);

private:
	/************************************
	 * ����PR or PA line
	 * �ɹ�����true�����򷵻�false
	 ************************************/
	bool PRPA(const string &line); 

	/************************************
	 * ������ݳ�ԱPres��Posts
	 ************************************/
	void PrePosts();

	set<string> PR; // ��Դ����
	set<string> PA; // �������
	//map<string, set<string>> Holder; // key(Place)��Դ�ĳ�����
	map<string, set<string>> Pres;     // key(Place or Transition)��ǰ�ü�
	map<string, set<string>> Posts;    // key(Place or Transition)�ĺ��ü�
	map<string, ptNode> ptNodes;       // key(Place or Transition) P/T nodes
	map<int, Siphon> Siphons;          // key(id��start id = 1) �ű꼯
	map<int, set<string>> ComSiphons;       // key(id����Siphons��Ӧ) �ű�(�����)�Ĳ���
};

