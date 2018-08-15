#pragma once
#include <set>
#include "XmlHelper.h"

enum nodeType
{
	PLACE, TRANSITION
};

// P/T�ڵ�, ���ƿ�����type,id,name, ԭ��place��transition����Щ��Ϣ������ͨ��ptNodes��key id��"P1"/"T1"�� ��ȡ��
struct ptNode
{
	nodeType type; // Place or Transition
	int id;        // ����pnt�ļ���place��Ż�transition��ţ����ƿ�����id����ԭ��Places��˳������
	string name;   // name.assign("P/T").append(to_string(cp.id));
	set<string> Pres;  // P/T node ǰ�ü�
	set<string> Posts; // P/T node ���ü�
};

// �ű�
struct Siphon
{
	bool clean;    // ture: ����գ�false: �����
	vector<string> siphon;
};

class Controler : public XmlHelper
{
public:
	Controler();
	~Controler();

	/************************************
	 * ��.pnt��SESSION.ina�ļ�����������������µĺ��п�������pnt�ļ�
	 * const string& infoFile: information file��Ϣ�ļ�
	 * const string& pntCPFile: �����µĺ��п�������pnt�ļ�
	 * �ɹ�����true�����򷵻�false
	 ************************************/
	bool pntCP(const string& pntFile, const string& SESSIONina, const string& infoFile, const string& pntCPFile);

	/************************************
	 * ��.pnt��SESSION.ina���ű겹��
	 * const string& pntFile: ����Դ�����͹��������Ϣ��pnt�ļ�
	 * const string& SESSION.ina �ļ�
	 * const string& outFile: outpu file
	 * �ɹ�����true�����򷵻�false
	 ************************************/
	bool comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile);

	/************************************
	 * �����ű��PR�������ű겹��
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
    
	// ������ݳ�Ա
	virtual void clear();

	/*********************************************
	 * ��controlerPlace��ɸѡ�Ϸ�������,������ݳ�ԱlegalControler,emptyCP,sameCP
	 * ɸѡ����
	 * (1) �޳�Pres��PostsΪ�յ�controlerPlace
	 * (2) Pres��Postsһ�µ�controlerPlace��ȡmarking��С�ߡ�
	 ***********************************************/
	void legalRule();

	// ��ȡ���Place��ţ�
	int getMaxPlaceNum()
	{
		int maxPlaceNum = 0;
		int num;
		for (string p : Places) {
			num = atoi(p.substr(1).c_str());
			if (num > maxPlaceNum) maxPlaceNum = num;
		}
		return maxPlaceNum;
	}

private:
	// ��ӿ�������������ݳ�ԱcontrolerPlace��initialMarkingCP
	bool addControler();

	/************************************
	 * ����PR or PA line
	 * �ɹ�����true�����򷵻�false
	 ************************************/
	bool PRPA(const string &line); 

	/************************************
	 * ������ݳ�ԱptNodesy�Լ�node��ԱPres��Posts
	 ************************************/
	void PrePosts();

	set<string> PR; // ��Դ����
	set<string> PA; // �������
	map<string, ptNode> ptNodes;         // key(Place or Transition name) P/T nodes
	map<int, Siphon> Siphons;            // key(id��start id = 1) �ű꼯
	map<int, set<string>> ComSiphons;    // key(id����Siphons��Ӧ) ������ű�Ĳ���
	/**
	 * key(id����Siphons��Ӧ) ��Ӧ������ű�Ĳ����Ŀ��ƿ���
	 * ptNode�е�id, ��Ź���ԭ��Places��˳������
	 **/
	map<int, ptNode> controlerPlace; 
	map<string, int> initialMarkingCP;   // key(id, controlerPlace name)��������ʼ��ʶ
   /**
    * ��controlerPlaceɸѡ���ĺϷ�������
	* key(id,��ӦcontrolerPlace),
	* value: ��controlerPlaceɸѡ���ĺϷ�������
	* ɸѡ����
	* (1) �޳�Pres��PostsΪ�յ�controlerPlace
	* (2) Pres��Postsһ�µ�controlerPlace��ȡmarking��С�ߡ�
    **/
	map<int, ptNode> legalControler; 
	map<int, ptNode> emptyCP;  // key(id,��ӦcontrolerPlace), pres and postsΪ�յ�controlerPlace
	map<int, vector<int>> sameCP;   // key(id,��ӦcontrolerPlace), pres and posts��ͬ��controlerPlace
};

