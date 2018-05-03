/*
 * ruleParser.h
 *
 *  Created on: Apr 25, 2018
 *      Author: aboelhamd
 */

#ifndef SRC_RULEPARSER_H_
#define SRC_RULEPARSER_H_

#include "../pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

class RuleParser {
public:
	static void sentenceTokenizer(map<string, vector<string> >* slTokenTag,
			map<string, vector<string> >* tlTokenTag, vector<string>* slTokens,
			vector<string>* tlTokens, char* tokenizedSentence);

	static void matchCats(map<string, vector<string> >* slTokenCat,
			map<string, vector<string> >* tlTokenCat, xml_node transfer,
			map<string, vector<string> > slTokenTag,
			map<string, vector<string> > tlTokenTag);

	static void matchRules(map<xml_node, vector<vector<string> > >* slTokenRule,
			map<xml_node, vector<vector<string> > >* tlTokenRule,
			vector<string> slTokens, vector<string> tlTokens, xml_node transfer,
			map<string, vector<string> > slTokenCat,
			map<string, vector<string> > tlTokenCat);

};

#endif /* SRC_RULEPARSER_H_ */
