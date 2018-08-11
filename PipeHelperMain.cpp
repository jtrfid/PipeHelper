#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include "HtmlHelper.h"
#include "XmlHelper.h"
#include "Controler.h"

using namespace std;

void invariantTest()
{
	HtmlHelper pipe;
	string htmlFile = "Invariant Analysis.html";
	string outFile = "Invariant Analysis.txt";
	pipe.InvariantsHtml(htmlFile, outFile);
}

void incidenceTest()
{
	HtmlHelper pipe;
	string htmlFile = "Incidence Matrix.html";
	string outFile = "Incidence Matrix.txt";
	pipe.IncidencesHtml(htmlFile, outFile);
}

void xmlTest()
{
	XmlHelper pipe;
	string xmlFile = "PIPE.xml";
	string outFile = "PIPE.txt";
	pipe.readXml(xmlFile, outFile);
}

void saveXml()
{
	XmlHelper pipe;
	string xmlFile = "PIPE.xml";
	string outFile = "PIPE.txt";
	pipe.readXml(xmlFile, outFile);

	string outXml = "myPIPE.xml";
	//pipe.saveXml(outXml);
}

void pnt2xml()
{
	XmlHelper pipe;
	string pntFile = "INA1.pnt";
	string xmlFile = "INA.xml";
	pipe.pnt2xml(pntFile, xmlFile);
}

int main()
{
	//invariantTest();
	//incidenceTest();
	//xmlTest();
	//saveXml();
	// pnt2xml();
	XmlHelper pipe;
	HtmlHelper pipe1;
	Controler controler;
	string pnt,xml,htmlFile,outFile;
	char select = '0';
	while (1) {
		if (select != '\n') {
			cout << "\n1: pnt to xml.\n"
				<< "2: Invariant Analysis.html to txt.\n"
				<< "3: Incidence Matrix.html to txt.\n"
				<< "4: xml to txt.\n"
				<< "5: Siphons.\n"
				<< "0: exit!\n";
			cout << "\nPlease select: ";
		}
		cin.get(select);
		if (select == '0') break;
		switch (select)
		{
		case '1':
			cout << "input pnt file: ";
			pnt = "";
			while(pnt.empty()) getline(cin, pnt);
			if (pnt.find_last_of('.pnt') == string::npos) pnt.append(".pnt");
			outFile = pnt + ".xml";
			pipe.pnt2xml(pnt, outFile);
			break;
		case '2':
			htmlFile = "Invariant Analysis.html";
			outFile = htmlFile + ".txt";
			pipe1.InvariantsHtml(htmlFile, outFile);
			break;
		case '3':
			htmlFile = "Incidence Matrix.html";
			outFile = htmlFile + ".txt";
			pipe1.IncidencesHtml(htmlFile, outFile);
			break;
		case '4':
			cout << "input xml file: ";
			xml= "";
			while (xml.empty()) getline(cin, xml);
			if (xml.find_last_of('.xml') == string::npos) xml.append(".xml");
			outFile = xml + ".txt";
			pipe.readXml(xml, outFile);
			break;
		case '5':
			cout << "input pnt file(include PR and PA): ";
			pnt = "INA.pnt"; // TODO:
			while (pnt.empty()) getline(cin, pnt);
			if (pnt.find_last_of('.pnt') == string::npos) pnt.append(".pnt");
			outFile = pnt + ".txt";
			controler.comSiphons(pnt,"SESSION.ina",outFile);
			break;
		default:
			break;
		}
	}

	system("pause");
	return 0;
}