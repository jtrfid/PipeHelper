#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "Tools.h"

using namespace std;

/*******************************************************************
 * ����PIPE�����html�ļ�: Invariant Analysis.html,Incidence Matrix.html
 * ǰ�ߵ��ã�bool InvariantsHtml(const string& htmlFile, const string &outFile);
 * ���ߵ���: bool IncidencesHtml(const string& htmlFile, const string &outFile);
 * ���������ļ���
 * HtmlParser.h
 * HtmlParse.cpp
 * Note: HtmlParse.cpp �������ã�fopen����ʹ�ã���
 * ��Ŀ->����->��������->C/C++ ->Ԥ������->Ԥ������������ӣ�_CRT_SECURE_NO_WARNINGS
 ********************************************************************/
class HtmlHelper
{
public:
	HtmlHelper();
	~HtmlHelper();

///////////////////Invariant Analysis.html
public:
	/**************************************
	 * ��ȡPIPE�����Invariant Analysis.html�ļ��������������ı��ļ�
	 * ������
	 * const string& htmlFile: PIPE�����Invariant Analysis.html�ļ�
	 * const string& outFile:  �����������ı��ļ�
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool InvariantsHtml(const string& htmlFile, const string &outFile);
private:
	// ����Html�ļ���õ�T����
	vector<string> Tvector;
	// ����Html�ļ���õ�P����
	vector<string> Pvector;
	// ����Html�ļ���õ�T����ʽ
	map<int, vector<int>> Tinvariants;
	// ����Html�ļ���õ�P����ʽ
	map<int, vector<int>> Pinvariants;
	// ����Html�ļ�,����T��P����ʽ��������Ϣ
	vector<string> others;

	/**************************************
	 * ��ȡPIPE�����Invariant Analysis.html�ļ�
	 * ��������:
	 * const string& htmlFile: PIPE�����html�ļ�
	 * ��������:
	 * vector<string>& fromHtml: ȥ����ǩ��Ǻ����Ч��Ϣ
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool InvariantHtml(const string& htmlFile, vector<string>& fromHtml);

	/********************************************
	 * ��������Invariant Analysis.html���ļ�
	 * ������
	 * formHtmlȥ����ǩ��Ǻ����Ч��Ϣ
	 * �ɹ�����true�����򷵻�false
	 *********************************************/
	bool InvariantHtml(const vector<string> &fromHtml);

	// ��ʾ�ͼ�¼Invariant Analysis.html�ļ��Ľ��������Ϣ
	void viewInvariants(ofstream &fout);

	// ������ݳ�Ա
	void clear();

////////////////// Incidence Matrix.html
public:
	/**************************************
	 * ��ȡPIPE�����Incidence Matrix.html�ļ��������������ı��ļ�
	 * ������
	 * const string& htmlFile: PIPE�����Incidence Matrix.html�ļ�
	 * const string& outFile:  �����������ı��ļ�
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool IncidencesHtml(const string& htmlFile, const string &outFile);
private:
	// ����Html�ļ���õ�T����
	// vector<string> Tvector; // ����
	// ����Html�ļ���õ�P����
	// vector<string> Pvector; // ����
	// ����Html�ļ���õ�T����ʽ
	// Forwards incidence matrix I+
	map<string, vector<int>, cmpByKey> ForwardMatrix;
	// Backwards incidence matrix I-
	map<string, vector<int>, cmpByKey> BackwardMatrix;
	// Combined incidence matrix I
	map<string, vector<int>, cmpByKey> CombinedMatrix;
	// Inhibition matrix H
	map<string, vector<int>, cmpByKey> InhibitionMatrix;
	// Marking
	map<string, vector<int>, cmpByKey> Marking;
	// Enabled transitions
	vector<string> EnableTransitions;

	/**************************************
	 * ��ȡPIPE�����Invariant Analysis.html�ļ�
	 * ��������:
	 * const string& htmlFile: PIPE�����html�ļ�
	 * ��������:
	 * vector<string>& fromHtml: ȥ����ǩ��Ǻ����Ч��Ϣ
	 * �ɹ�����true�����򷵻�false
	 ***************************************/
	bool IncidencesHtml(const string& htmlFile, vector<string>& fromHtml);

	/********************************************
	 * ��������Incidence Matrix.html���ļ�
	 * ������
	 * formHtmlȥ����ǩ��Ǻ����Ч��Ϣ
	 * �ɹ�����true�����򷵻�false
	 *********************************************/
	bool IncidencesHtml(const vector<string> &fromHtml);

	// ��ʾ�ͼ�¼Incidence Matrix.html�ļ��Ľ��������Ϣ
	void viewIncidences(ofstream &fout);

};


