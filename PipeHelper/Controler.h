#pragma once
#include <set>
#include "XmlHelper.h"
#include "Tools.h"

enum nodeType
{
	PLACE, TRANSITION, CONTROLER
};

// P/T�ڵ�, ���ƿ�����type,id,name,cpKey, ԭ��place��transition����Щ��Ϣ
struct ptNode
{
	nodeType type; // PLACE, TRANSITION��CONTROLER
	int id;        // ����pnt�ļ���place��Ż�transition��ţ����ƿ�����id����ԭ��Places�����ź�˳������
	string name;   // name.assign("P/T").append(to_string(cp.id));
	int cpKey;     // controlerPlace/Siphons/ComSiphons/CPInfo key һ��, ͳ��cpKey
	set<string, cmpByKey> Pres;  // P node ǰ�ü�
	set<string, cmpByKey> Posts; // P node ���ü�
};

// �ű�
struct Siphon
{
	Siphon():clean(false)
	{ }
	bool clean;    // ture: ����գ�false: �����
	vector<string> siphon;
};

// Controler information
struct ControlerInfo
{
	ControlerInfo():isEmpty(false),removed(false)
	{ }
	int cpKey;          // controlerPlace/Siphons/ComSiphons/CPInfo key һ��, ͳ��cpKey
	bool isEmpty;       // true: �յ�pres��posts; otherwise false
	bool removed;       // true: �ǿտ��������޳�,��replacedIdȡ��; otherwise false
	vector<int> sameCP; // ��ͬpres��posts��controllerPlace key����
	int replacedId;     // if(removed),��ȡ����controlerPlace key����same����Сmarking��controllerPlace��
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
	bool comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string, cmpByKey>& comSiphon);

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

	// ��ȡ���Places��ţ�
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

	/*********************************************
	 * ��controlerPlace��ɸѡ�Ϸ�������,������ݳ�ԱCPInfo
	 * ��CPInfo��ȡ��Ч����������Դ������PRafterCP
	 * ɸѡ����
	 * (1) �޳�Pres��PostsΪ�յ�controlerPlace
	 * (2) Pres��Postsһ�µ�controlerPlace��ȡmarking��С�ߡ�
	 ***********************************************/
	void legalRule();

	/************************************
	 * ����PR or PA line
	 * �ɹ�����true�����򷵻�false
	 ************************************/
	bool PRPA(const string &line); 

	/************************************
	 * ������ݳ�ԱptNodesy�Լ�node��ԱPres��Posts
	 ************************************/
	void PrePosts();

	// ��ʾ�ͼ�¼controler��Ϣ
	void viewControler(ofstream &fout);

	// ��ʾ�ͼ�¼��������ptNode��Ϣ
	void ptNodeView(ptNode node, ofstream &fout);

	vector<string> PlacesAfterCP;  // ��ӿ������Ժ�,����������ԭ��Places

	set<string> PR; // ��ӿ�������ǰ����Դ����
	set<string, cmpByKey> PRafterCP; // ��ӿ������Ժ����Դ����
	set<string> PA; // �������
	map<string, ptNode> ptNodes;         // key(Place or Transition name) P/T nodes

	///////////////////////////////////////////////
	// controlerPlace / Siphons / ComSiphons / CPInfo key һ��, ͳ��cpKey
	map<int, Siphon> Siphons;            // key(id��start id = 1) value���ű꼯�ϣ����б�����ű�
	map<int, set<string,cmpByKey>> ComSiphons;    // key(id����Siphons��Ӧ) value��������ű�Ĳ�����������ű�Ϊ��
    /**
	 * key(id����Siphons��Ӧ) ��Ӧ������ű�Ĳ����Ŀ��ƿ���
	 * ptNode�е�id, ��Ź���ԭ��Places�����ź�˳������
	 **/
	map<int, ptNode> controlerPlace; 
	
	map<int, ControlerInfo> CPInfo; // key(id,��ӦcontrolerPlace)

    // key(controlerPlace name)��������ʼ��ʶ
	// ���ƿ���marking�Ǳ�����ű��marking�ܺ� - 1
	map<string, int> initialMarkingCP;   
};

