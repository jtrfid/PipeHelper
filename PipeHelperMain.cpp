#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include "HtmlHelper.h"
#include "XmlHelper.h"
#include "Controler.h"

using namespace std;

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
	string::size_type pos;
	char select = '0';
	while (1) {
		if (select != '\n') {
			cout << "\n1: pnt to xml.\n"
				<< "2: Invariant Analysis.html to txt.\n"
				<< "3: Incidence Matrix.html to txt.\n"
				<< "4: Invariant Analysis.html and Incidence Matrix.html to LINGO.\n"
				<< "5: xml to txt.\n"
				<< "6: comSiphons(need SESSION.ina).\n"
				<< "7: controler(need SESSION.ina).\n"
				<< "0: exit!\n";
			cout << "\nPlease select: ";
		}
		cin.get(select);
		if (select == '0') break;
		switch (select)
		{
		case '1':
			cout << "input pnt file: ";
			pnt.clear();
			while(pnt.empty()) getline(cin, pnt);
			if (pnt.find_last_of('.') == string::npos) pnt.append(".pnt");
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
			outFile = "LINGO.txt";
			pipe1.InvariantsIncidences("Invariant Analysis.html", "Incidence Matrix.html", outFile);
			break;
		case '5':
			cout << "input xml file: ";
			xml= "";
			xml.clear();
			while (xml.empty()) getline(cin, xml);
			if (xml.find_last_of('.') == string::npos) xml.append(".xml");
			outFile = xml + ".txt";
			pipe.readXml(xml, outFile);
			break;
		case '6':
			cout << "input pnt file(include PR and PA): ";
			//pnt = "INA2.pnt"; 
			pnt.clear();
			while (pnt.empty()) getline(cin, pnt);
			if (pnt.find_last_of('.') == string::npos) pnt.append(".pnt");
			outFile = pnt + ".txt";
			controler.comSiphons(pnt,"SESSION.ina",outFile);
			break;
			// 由.pnt和SESSION.ina文件计算控制器，生成新的含有控制器的pnt文件
		case '7':
			cout << "input pnt file(include PR and PA): ";
			//pnt = "INA2.pnt"; 
			pnt.clear();
			while (pnt.empty()) getline(cin, pnt);
			if (pnt.find_last_of('.') == string::npos) pnt.append(".pnt");
			outFile = pnt + ".txt";
			pos = pnt.find_last_of('.');
			xml = pnt.substr(0,pos);
			xml.append("CP").append(".pnt");
			controler.pntCP(pnt, "SESSION.ina", outFile, xml);
			break;
		default:
			break;
		}
	}

	system("pause");
	return 0;
}