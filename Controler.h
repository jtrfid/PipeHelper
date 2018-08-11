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
	bool clean;    // ture: 被清空，false: 非清空
	vector<string> siphon;
};

class Controler :
	public XmlHelper
{
public:
	Controler();
	~Controler();

	/************************************
	 * 信标补集
	 * const string& pntFile: 有资源库所和工序库所信息的pnt文件
	 * const string& SESSION.ina 文件
	 * const string& outFile: outpu file
	 * 成功返回true，否则返回false
	 ************************************/
	bool comSiphons(const string& pntFile, const string& SESSIONina, const string& outFile);

	/************************************
	 * 信标补集
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

private:
	/************************************
	 * 处理PR or PA line
	 * 成功返回true，否则返回false
	 ************************************/
	bool PRPA(const string &line); 

	/************************************
	 * 填充数据成员Pres，Posts
	 ************************************/
	void PrePosts();

	set<string> PR; // 资源库所
	set<string> PA; // 工序库所
	//map<string, set<string>> Holder; // key(Place)资源的持有者
	map<string, set<string>> Pres;     // key(Place or Transition)的前置集
	map<string, set<string>> Posts;    // key(Place or Transition)的后置集
	map<string, ptNode> ptNodes;       // key(Place or Transition) P/T nodes
	map<int, Siphon> Siphons;          // key(id，start id = 1) 信标集
	map<int, set<string>> ComSiphons;       // key(id，与Siphons对应) 信标(被清空)的补集
};

