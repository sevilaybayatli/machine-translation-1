/*
 * RuleExecution.h
 *
 *  Created on: May 5, 2018
 *      Author: aboelhamd
 */

#ifndef SRC_RULEEXECUTION_H_
#define SRC_RULEEXECUTION_H_

#include "../pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

class RuleExecution {
public:
	static void ruleExe(xml_node rule,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<string, vector<vector<string> > > attrs);

	static void chooseAction(xml_node choose,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<string, vector<vector<string> > > attrs,
			map<int, int> paramToPattern);

	static void letAction(xml_node let,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<string, vector<vector<string> > > attrs,
			map<int, int> paramToPattern);

	static void macroAction(xml_node callMacro,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<string, vector<vector<string> > > attrs);
	static void outAction();

	static vector<string> formatTokenTags(string token, vector<string> tags);

	static vector<string> findAttrPart(vector<string> tokenTags,
			vector<vector<string> > attrTags);

	static vector<string> clipAction(xml_node clip,
			map<string, vector<vector<string> > > attrs,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<int, int> paramToPattern);

	static vector<string> concat(xml_node concat,
			map<string, vector<vector<string> > > attrs,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<int, int> paramToPattern);

	static bool equal(xml_node equal,
			map<string, vector<vector<string> > > attrs,
			vector<vector<string> >* slAnalysisTokens,
			vector<vector<string> >* tlAnalysisTokens,
			map<int, int> paramToPattern);

	static vector<string> litTagAction(xml_node litTag);
};

#endif /* SRC_RULEEXECUTION_H_ */
