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
			map<string, vector<string> >* tlTokenTag, char* tokenizedSentence);

	static void matchCats(map<string, vector<string> >* slTokenCat,
			map<string, vector<string> >* tlTokenCat, xml_node transfer,
			map<string, vector<string> > slTokenTag,
			map<string, vector<string> > tlTokenTag);

	static void matchRules(map<string, vector<xml_node> >* slTokenRule,
			map<string, vector<xml_node> >* tlTokenRule, xml_node transfer,
			map<string, vector<string> > slTokenCat,
			map<string, vector<string> > tlTokenCat);

};

#endif /* SRC_RULEPARSER_H_ */
