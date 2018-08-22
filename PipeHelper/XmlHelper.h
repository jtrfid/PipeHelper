#pragma once
#include <string>
#include <vector>
#include <map>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

/*******************************************************************
 * pnt to PIPE xml file
 * ����: bool pnt2xml(const string &pntFile, const string &outXml);
 * ��ȡPIPE�����.xml�ļ��������������ı��ļ�
 * ����: bool readXml(const string& xmlFile, const string &outFile);
 * ���������ļ���
 * tinyxml2.h
 * tinyxml2.cpp
 * Note: tinyxml2.cpp �������ã�fopen����ʹ�ã���
 * ��Ŀ->����->��������->C/C++ ->Ԥ������->Ԥ������������ӣ�_CRT_SECURE_NO_WARNINGS
 ********************************************************************/
class XmlHelper
{
public:
	XmlHelper();
	~XmlHelper();
	/**************************************
	 * ��ȡPIPE�����.xml�ļ��������������ı��ļ�
	 * ������
	 * const string& xmlFile:  PIPE�����.xml�ļ�
	 * const string& outFile:  �����������ı��ļ�
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool readXml(const string& xmlFile, const string &outFile);
    
	/***************************************
	 * pnt to PIPE xml file
	 * const string& pntFile: input pnt file
	 * const string& outXml: output xml file��outXml.txt�ļ�(���鿴��֤)
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool pnt2xml(const string &pntFile, const string &outXml);

	// ����source��target����arcId, eg. "P10 to T2" source="P10" target="T2"
	string& createArcId(const string &source, const string &target, string &arcId) 
	{
		arcId.assign(source).append(" to ").append(target);
		return arcId;
	}

protected:
	/**************************************
	 * �������ݳ�Ա������xml�ļ�
	 * const string& outXml:  output xml�ļ�
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool saveXml(const string &outXml);

	/**************************************
	 * ����<net> �е������ֵܽڵ�(SiblingElement) </net>
	 * �ɹ�����true,���򷵻�false
	 ***************************************/
	bool traverse(XMLElement *ele, ofstream &fout);

	/***************************************
	 * ����<place> ... </place>
	 * �ɹ�����true,���򷵻�false
	 ****************************************/
	bool place(XMLElement* place, ofstream &fout);

	/***************************************
	 * ����<transition> ... </transition>
	 * �ɹ�����true,���򷵻�false
	 ***************************************/
	bool transition(XMLElement* place, ofstream &fout);

	/***************************************
	 * ����<arc> ... </arc>
	 * �ɹ�����true,���򷵻�false
	****************************************/
	bool arc(XMLElement* place, ofstream &fout);

	// ��ʾ�ͼ�¼PIPE����xml�ļ��Ľ��������Ϣ
	virtual void view(ofstream &fout);

	// ������ݳ�Ա
	virtual void clear();

	// save place element, ignore <graphics> 
	void place(XMLDocument &doc, XMLNode* net);

	// save transition element, ignore <graphics> etc. 
	void transition(XMLDocument &doc, XMLNode* net);

	// save arc element, ignore <graphics> etc. 
	void arc(XMLDocument &doc, XMLNode* net);

	/***************************************
	 * ÿ�����������ַ���
	 * string parts[3]; // 3�����ַ�����P = parts[0], M = parts[2], Pre,Post = parts[3];
	 ***************************************/
	bool parts3(const string &line, string parts[]);

	/**********************************************************
	 * ����pre,post, ������ݳ�Ա: Transitions,source,target,weight
	 * pre,post: eg. 1:3 4 5 6:7 ע���������Ҫ��':'ǰ���޿ո�
	 * ������
	 * const string &place��  pre,post�е�place
	 * const string &prePost  pre/post string
	 * bool pre: true��ʾprePost��pre, false��ʾprePost��post
	 ************************************************************/
	bool prePost(const string &place, const string &prePost, bool pre);

	vector<string> Places;  // place id
	vector<string> Transitions; //transition id
	map<string, int> initialMarking;  // key: place id
	map<string, int> capacity;  // key: place id
	// key: arc id, eg. "P10 to T2" source="P10" target="T2"
	map<string, string> source; 
	// key: arc id, eg. "P10 to T2" source="P10" target="T2"
	map<string, string> target; 
	// key: arc id, eg. "P10 to T2" source="P10" target="T2"
	map<string, int> weight; 
};

