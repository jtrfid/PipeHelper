// xmlTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "tinyxml2.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

using namespace std;
using namespace tinyxml2;

bool XMLTest(const char* testString, const char* expected, const char* found, bool echo = true, bool extraNL = false);
bool XMLTest(const char* testString, XMLError expected, XMLError found, bool echo = true, bool extraNL = false);
bool XMLTest(const char* testString, bool expected, bool found, bool echo = true, bool extraNL = false);
template< class T > bool XMLTest(const char* testString, T expected, T found, bool echo = true);

int main222222()
{
	XMLDocument doc;
	doc.LoadFile("resources/dream.xml");
	if (doc.Error())
	{
		doc.PrintError();
		return -1;
	}

	// xml version="1.0"
	cout<<doc.FirstChild()->ToDeclaration()->Value()<<endl;
	// DOCTYPE PLAY SYSTEM "play.dtd"
	cout<<doc.FirstChild()->NextSibling()->ToUnknown()->Value()<<endl;
	// And Robin shall restore amends.
	cout<<doc.LastChild()->LastChild()->LastChild()->LastChild()->LastChildElement()->GetText()<<endl;
	
	//doc.Parse(doc);
	//XMLTest("Load dream.xml", false, doc.Error());

	// Element: "PLAY"
	XMLElement* titleElement = doc.FirstChildElement("PLAY")->FirstChildElement("TITLE");
	const char* title = titleElement->GetText();
	printf("Name of play (1): %s\n", title); // Name of play (1): A Midsummer Night's Dream 

	XMLText* textNode = titleElement->FirstChild()->ToText();
	title = textNode->Value();
	printf("Name of play (2): %s\n", title); // Name of play (1): A Midsummer Night's Dream

	int count = 0;
	for (XMLNode* ele = doc.FirstChildElement("PLAY")->FirstChild();
		ele;
		ele = ele->NextSibling())
	{
		//cout<<ele->Accept()
		++count;
	}
	cout << "count=" << count << endl; // 10
	
	system("pause");
	return 0;
}

