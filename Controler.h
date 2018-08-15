#pragma once
#include <set>
#include "XmlHelper.h"

enum nodeType
{
	PLACE, TRANSITION
};

// P/T节点, 控制库所有type,id,name, 原有place和transition无这些信息，可以通过ptNodes的key id（"P1"/"T1"） 获取，
struct ptNode
{
	nodeType type; // Place or Transition
	int id;        // 来自pnt文件的place编号或transition编号，控制库所的id是在原有Places后顺序排列
	string name;   // name.assign("P/T").append(to_string(cp.id));
	set<string> Pres;  // P/T node 前置集
	set<string> Posts; // P/T node 后置集
};

// 信标
struct Siphon
{
	bool clean;    // ture: 被清空，false: 非清空
	vector<string> siphon;
};

class Controler : public XmlHelper
{
public:
	Controler();
	~Controler();

	/************************************
	 * 由.pnt和SESSION.ina文件计算控制器，生成新的含有控制器的pnt文件
	 * const string& infoFile: information file信息文件
	 * const string& pntCPFile: 生成新的含有控制器的pnt文件
	 * 成功返回true，否则返回false
	 ************************************/
	bool pntCP(const string& pntFile, const string& SESSIONina, const string& infoFile, const string& pntCPFile);

	/************************************
	 * 由.pnt和SESSION.ina求信标补集
	 * const string& pntFile: 有资源库所和工序库所信息的pnt文件
	 * const string& SESSION.ina 文件
	 * const string& outFile: outpu file
	 * 成功返回true，否则返回false
	 ************************************/
	bool comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile);

	/************************************
	 * 根据信标和PR参数求信标补集
	 * 输入参数:
	 * const vector<string>& siphons: 信标集
	 * const vector<string>& Pr: 资源库所集
	 * 输出参数:
	 * set<string>& comSiphon: 返回信标补集
	 * 成功返回true, 否则，返回false(例如，补集为空 )
	 ************************************/
	bool comSiphons(const vector<string>& siphons, const set<string>& Pr, set<string>& comSiphon);

	/************************************
	 * Union H(r), r in Siphon与PR的交集
	 * 输入参数:
	 * const vector<string>& siphons: 信标集
	 * const set<string>& Pr: 资源库所集
	 * 输出参数:
	 * set<string>& unionHr: 返回Union H(r)
	 * 如果holder非空，函数返回true, 否则返回false
	 ************************************/
	bool UnionHr(const vector<string>& siphons, const set<string>& Pr, set<string>& unionHr);
	/************************************
	 * 获取资源的Holder, resource前置集的前置集与PA的交集: H(r)
	 * 输入参数:
	 * const string& resource: place name
	 * 输出参数:
	 * set<string>& H: 返回resource的holder
	 * 如果holder非空，函数返回true, 否则返回false
	 ************************************/
	bool holder(const string& resource, set<string>& H);

	/************************************
	 * read 有资源库所和工序库所信息的pnt文件，填充数据成员
	 * const string& pntFile: input pnt file
	 * 成功返回true，否则返回false
	 *************************************/
	bool readPnt(const string &pntFile);

	/************************************
	 * 获得节点的前置集的前置集
	 * 输入参数:
	 * const &string ptName: place/transition name
	 * 输出参数:
	 * set<string> prePre: 返回ptName的前置集的前置集
	 * 如果有前置集的前置集，函数返回true, 否则返回false
	 ************************************/
	bool prePre(const string& ptName, set<string> &prePre);

	/*****************************************
	 * 读取SESSION.ina, 填充数据成员 Siphons
	 * const string& SESSION.ina文件名
	 * 成功返回true，否则返回false
	 *****************************************/
	bool sessionIna(const string& SESSIONina);

protected:
	// 显示和记录信息
	virtual void view(ofstream &fout);
    
	// 清除数据成员
	virtual void clear();

	/*********************************************
	 * 从controlerPlace中筛选合法控制器,填充数据成员legalControler,emptyCP,sameCP
	 * 筛选规则：
	 * (1) 剔除Pres和Posts为空的controlerPlace
	 * (2) Pres和Posts一致的controlerPlace，取marking最小者。
	 ***********************************************/
	void legalRule();

	// 获取最大Place编号，
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
	// 添加控制器，填充数据成员controlerPlace和initialMarkingCP
	bool addControler();

	/************************************
	 * 处理PR or PA line
	 * 成功返回true，否则返回false
	 ************************************/
	bool PRPA(const string &line); 

	/************************************
	 * 填充数据成员ptNodesy以及node成员Pres，Posts
	 ************************************/
	void PrePosts();

	set<string> PR; // 资源库所
	set<string> PA; // 工序库所
	map<string, ptNode> ptNodes;         // key(Place or Transition name) P/T nodes
	map<int, Siphon> Siphons;            // key(id，start id = 1) 信标集
	map<int, set<string>> ComSiphons;    // key(id，与Siphons对应) 被清空信标的补集
	/**
	 * key(id，与Siphons对应) 对应被清空信标的补集的控制库所
	 * ptNode中的id, 编号规则：原有Places后顺序排列
	 **/
	map<int, ptNode> controlerPlace; 
	map<string, int> initialMarkingCP;   // key(id, controlerPlace name)控制器初始标识
   /**
    * 从controlerPlace筛选出的合法控制器
	* key(id,对应controlerPlace),
	* value: 从controlerPlace筛选出的合法控制器
	* 筛选规则：
	* (1) 剔除Pres和Posts为空的controlerPlace
	* (2) Pres和Posts一致的controlerPlace，取marking最小者。
    **/
	map<int, ptNode> legalControler; 
	map<int, ptNode> emptyCP;  // key(id,对应controlerPlace), pres and posts为空的controlerPlace
	map<int, vector<int>> sameCP;   // key(id,对应controlerPlace), pres and posts相同的controlerPlace
};

