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

	// tokens in the sentence order
	vector<string> slTokens, tlTokens;
	// map of tokens and their tags
	map<string, vector<string> > slTokenTag, tlTokenTag;

	RuleParser::sentenceTokenizer(&slTokenTag, &tlTokenTag, &slTokens,
			&tlTokens, tokenizedSentence);

	// load transfer file in an xml document object
	xml_document transferDoc;
	xml_parse_result result = transferDoc.load_file("transferFile.t1x");

	if (string(result.description()) != "No error") {
		cout << "ERROR : " << result.description() << endl;
		return -1;
	}
	// xml node of the parent node (transfer) in the transfer file
	xml_node transfer = transferDoc.child("transfer");

	// map of tokens and their matched categories
	map<string, vector<string> > slTokenCat, tlTokenCat;

	RuleParser::matchCats(&slTokenCat, &tlTokenCat, transfer, slTokenTag,
			tlTokenTag);

	// map of tokens and their matched rules
	map<xml_node, vector<vector<string> > > slTokenRule, tlTokenRule;

	RuleParser::matchRules(&slTokenRule, &tlTokenRule, slTokens, tlTokens,
			transfer, slTokenTag, tlTokenTag);

	for (map<xml_node, vector<vector<string> > >::iterator it =
			slTokenRule.begin(); it != slTokenRule.end(); ++it) {
		cout << "The applied rule : " << it->first.first_attribute().value()
				<< endl;

		cout << "The pattern-items :  ";
		xml_node pattern = it->first.child("pattern");
		for (xml_node pattern_item = pattern.first_child(); pattern_item;
				pattern_item = pattern_item.next_sibling()) {
			cout << pattern_item.first_attribute().value() << "  ";
		}
		cout << endl << endl;

		cout << "The applied tokens : " << endl;

		for (unsigned i = 0; i < it->second.size(); i++) {
			for (unsigned j = 0; j < it->second[i].size(); j++) {
				cout << it->second[i][j] << "  ";
			}
			cout << endl;
		}
		cout << endl;
	}

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
//	for (unsigned i = 0; i < slTokens.size(); i++) {
//		cout << slTokens[i] << endl;
//		cout << tlTokens[i] << endl;
//	}

//	for (map<string, vector<pair<xml_node, int> > >::iterator it =
//			tlTokenRule.begin(); it != tlTokenRule.end(); ++it) {
//		cout << "Token: " << it->first << " ,rule comments: " << endl;
//		vector<pair<xml_node, int> > rules = it->second;
//		for (unsigned i = 0; i < rules.size(); i++) {
//			cout << rules[i].first.first_attribute().value()
//					<< "  , pattern-item : " << rules[i].second << endl;
//			cout << "pattern item names : " << endl;
//
//			xml_node pattern = rules[i].first.child("pattern");
//			for (xml_node pattern_item = pattern.first_child(); pattern_item;
//					pattern_item = pattern_item.next_sibling()) {
//				cout << "  " << pattern_item.first_attribute().value();
//			}
//			cout << endl;
//		}
//		cout << endl << endl;
//	}
}
