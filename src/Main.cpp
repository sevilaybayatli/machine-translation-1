#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <string.h>

#include "../pugixml/pugixml.hpp"
#include "RuleParser.h"

using namespace std;
using namespace pugi;

int main(int argc, char **argv) {

	char* tokenizedSentence;

	if (argc == 2) {
		tokenizedSentence = argv[1];
	}

	// map of tokens and their tags
	map<string, vector<string> > slTokenTag, tlTokenTag;

	RuleParser::sentenceTokenizer(&slTokenTag, &tlTokenTag, tokenizedSentence);

	// load transfer file in an xml document object
	xml_document transferDoc;
	xml_parse_result result = transferDoc.load_file("transferFile.t1x");

	if (string(result.description()) != "No error") {
		cout << result.description() << endl;
		return -1;
	}
	// xml node of the parent node (transfer) in the transfer file
	xml_node transfer = transferDoc.child("transfer");

	// map of tokens and their matched categories
	map<string, vector<string> > slTokenCat, tlTokenCat;

	RuleParser::matchCats(&slTokenCat, &tlTokenCat, transfer, slTokenTag,
			tlTokenTag);

	// map of tokens and their matched rules
	map<string, vector<xml_node> > slTokenRule, tlTokenRule;

	RuleParser::matchRules(&slTokenRule, &tlTokenRule, transfer, slTokenTag,
			tlTokenTag);

//	for (map<string, vector<string> >::iterator it = tlTokenCat.begin();
//			it != tlTokenCat.end(); ++it) {
//		cout << "Token: " << it->first << " ,cat names: ";
//		vector<string> cats = it->second;
//		for (unsigned i = 0; i < cats.size(); i++) {
//			cout << cats[i] << " , ";
//		}
//		cout << endl;
//	}
//
//	for (map<string, vector<xml_node> >::iterator it = tlTokenRule.begin();
//			it != tlTokenRule.end(); ++it) {
//		cout << "Token: " << it->first << " ,rule comments: " << endl;
//		vector<xml_node> rules = it->second;
//		for (unsigned i = 0; i < rules.size(); i++) {
//			cout << rules[i].first_attribute().value() << endl;
//			cout << "pattern item names : " << endl;
//
//			xml_node pattern = rules[i].child("pattern");
//			for (xml_node pattern_item = pattern.first_child(); pattern_item;
//					pattern_item = pattern_item.next_sibling()) {
//				cout << "  " << pattern_item.first_attribute().value();
//			}
//			cout << endl;
//		}
//		cout << endl << endl;
//	}
}
