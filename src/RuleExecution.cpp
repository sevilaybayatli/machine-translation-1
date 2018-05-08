/*
 * RuleExecution.cpp
 *
 *  Created on: May 5, 2018
 *      Author: aboelhamd
 */

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "../pugixml/pugixml.hpp"
#include "TranElemLiterals.h"

using namespace std;
using namespace pugi;
using namespace elem;

#include "RuleExecution.h"

void RuleExecution::ruleExe(xml_node rule,
		vector<vector<string> >* slAnalysisTokens,
		vector<vector<string> >* tlAnalysisTokens,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedTokens, vector<string> tlMatchedTokens,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag) {

	for (xml_node node = rule.child(ACTION).first_child(); node;
			node = node.next_sibling()) {

		string nodeName = node.name();
		if (nodeName == LET) {
			letAction(node, slAnalysisTokens, tlAnalysisTokens, attrs,
					slMatchedTokens, tlMatchedTokens, slTokenTag, tlTokenTag,
					map<int, int>());
		} else if (nodeName == CHOOSE) {
			chooseAction(node, slAnalysisTokens, tlAnalysisTokens, attrs,
					slMatchedTokens, tlMatchedTokens, slTokenTag, tlTokenTag,
					map<int, int>());
		} else if (nodeName == CALL_MACRO) {
			macroAction(node, slAnalysisTokens, tlAnalysisTokens, attrs,
					slMatchedTokens, tlMatchedTokens, slTokenTag, tlTokenTag);
		}
	}
}

void RuleExecution::macroAction(xml_node callMacro,
		vector<vector<string> >* slAnalysisTokens,
		vector<vector<string> >* tlAnalysisTokens,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedTokens, vector<string> tlMatchedTokens,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag) {

	string macroName = callMacro.attribute(N).value();

	map<int, int> paramToPattern;
	int i = 1;
	for (xml_node with_param = callMacro.child(WITH_PARAM); with_param;
			with_param = with_param.next_sibling()) {
		paramToPattern[i] = with_param.attribute(POS).as_int();
	}

	xml_node transfer = callMacro.parent().parent().parent();

	xml_node macros = transfer.child(SECTION_DEF_MACROS);

	for (xml_node macro = macros.child(DEF_MACRO); macro;
			macro = macro.next_sibling()) {
		if (string(macro.name()) == macroName) {
			chooseAction(macro.child(CHOOSE), slAnalysisTokens,
					tlAnalysisTokens, attrs, slMatchedTokens, tlMatchedTokens,
					slTokenTag, tlTokenTag, paramToPattern);
			break;
		}
	}
}

vector<string> RuleExecution::findAttrPart(vector<string> tokenTags,
		vector<vector<string> > attrTags) {

	vector<string> matchedTags;
	for (unsigned i = 0; i < tokenTags.size(); i++) {
		for (unsigned j = 0; j < attrTags.size(); j++) {

			if (tokenTags[i] == attrTags[j][0]) {
				matchedTags.push_back("<" + tokenTags[i] + ">");
				for (unsigned k = 1;
						k < attrTags[j].size() && (k + i) < tokenTags.size();
						k++) {

					if (tokenTags[i + k] == attrTags[j][k])
						matchedTags.push_back("<" + tokenTags[i + k] + ">");
					else
						break;
				}
				if (matchedTags.size() == attrTags[j].size())
					return matchedTags;
				else
					matchedTags.clear();
			}
		}
	}
	return matchedTags;
}

// equal has only 2 childs
// they are always in this transfer file
// clip and lit-tag only, but we will make it general
bool RuleExecution::equal(xml_node equal,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedTokens, vector<string> tlMatchedTokens,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag, map<int, int> paramToPattern) {

	xml_node firstChild = equal.first_child();
	vector<string> firstResult;

	string firstName = firstChild.name();
	if (firstName == CLIP) {
		firstResult = clipAction(firstChild, attrs, slMatchedTokens,
				tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);
	} else if (firstName == CONCAT) {
		firstResult = concat(firstChild, attrs, slMatchedTokens,
				tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);
	} else if (firstName == LIT_TAG) {
		string tag = firstChild.attribute(V).value();
		firstResult.push_back("<" + tag + ">");
	} else if (firstName == LIT) {
		firstResult.push_back(firstChild.attribute(V).value());
	}

	xml_node secondChild = firstChild.next_sibling();
	vector<string> secondResult;

	string secondName = secondChild.name();
	if (secondName == CLIP) {
		secondResult = clipAction(secondChild, attrs, slMatchedTokens,
				tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);
	} else if (secondName == CONCAT) {
		secondResult = concat(secondChild, attrs, slMatchedTokens,
				tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);
	} else if (secondName == LIT_TAG) {
		string tag = secondChild.attribute(V).value();
		secondResult.push_back("<" + tag + ">");
	} else if (secondName == LIT) {
		secondResult.push_back(secondChild.attribute(V).value());
	}

	if (firstResult.size() != secondResult.size())
		return false;

	for (unsigned i = 0; i < firstResult.size(); i++) {
		if (firstResult[i] != secondResult[i])
			return false;
	}

	return true;
}

void RuleExecution::chooseAction(xml_node choose,
		vector<vector<string> >* slAnalysisTokens,
		vector<vector<string> >* tlAnalysisTokens,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedTokens, vector<string> tlMatchedTokens,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag, map<int, int> paramToPattern) {

	xml_node when = choose.child(WHEN);

	xml_node test = when.child(TEST);

	xml_node node = test.first_child();
	string nodeName = node.name();

	bool result;

	if (nodeName == EQUAL) {
		result = equal(node, attrs, slMatchedTokens, tlMatchedTokens,
				slTokenTag, tlTokenTag, paramToPattern);
	} else if (nodeName == AND) {
		for (xml_node equalNode = node.first_child(); equalNode; equalNode =
				equalNode.next_sibling()) {

			result = equal(equalNode, attrs, slMatchedTokens, tlMatchedTokens,
					slTokenTag, tlTokenTag, paramToPattern);
			if (!result)
				break;
		}
	} else if (nodeName == OR) {
		for (xml_node equalNode = node.first_child(); equalNode; equalNode =
				equalNode.next_sibling()) {

			result = equal(equalNode, attrs, slMatchedTokens, tlMatchedTokens,
					slTokenTag, tlTokenTag, paramToPattern);
			if (result)
				break;
		}
	}

	// we assume that let only comes after test
	if (result) {
		for (xml_node let = when.child(LET); let; let = let.next_sibling(LET)) {
			letAction(let, slAnalysisTokens, tlAnalysisTokens, attrs,
					slMatchedTokens, tlMatchedTokens, slTokenTag, tlTokenTag,
					paramToPattern);
		}
	}
}

void RuleExecution::letAction(xml_node let,
		vector<vector<string> >* slAnalysisTokens,
		vector<vector<string> >* tlAnalysisTokens,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedTokens, vector<string> tlMatchedTokens,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag, map<int, int> paramToPattern) {

	// it is always a clip
	xml_node firstChild = let.first_child();
	vector<string> firstResult = clipAction(firstChild, attrs, slMatchedTokens,
			tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);

	xml_node secondChild = firstChild.next_sibling();
	string secondName = secondChild.name();
	vector<string> secondResult;
	if (secondName == CLIP) {
		secondResult = clipAction(secondChild, attrs, slMatchedTokens,
				tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);
	} else if (secondName == CONCAT) {
		secondResult = concat(secondChild, attrs, slMatchedTokens,
				tlMatchedTokens, slTokenTag, tlTokenTag, paramToPattern);
	} else if (secondName == LIT_TAG) {
		string tag = secondChild.attribute(V).value();
		secondResult.push_back("<" + tag + ">");
	} else if (secondName == LIT) {
		secondResult.push_back(secondChild.attribute(V).value());
	}

	int position = firstChild.attribute(POS).as_int() - 1;
	if (paramToPattern.size())
		position = paramToPattern[firstChild.attribute(POS).as_int()] - 1;

	if (firstChild.attribute(SIDE).value() == SL) {
		for (unsigned i = 0; i < (*slAnalysisTokens)[position].size(); i++) {
			if ((*slAnalysisTokens)[position][i] == firstResult[0]) {
				(*slAnalysisTokens)[position].erase(
						(*slAnalysisTokens)[position].begin() + i,
						(*slAnalysisTokens)[position].begin() + i
								+ firstResult.size());
				(*slAnalysisTokens)[position].insert(
						(*slAnalysisTokens)[position].begin() + i,
						secondResult.begin(), secondResult.end());
				break;
			}
		}
	} else {
		for (unsigned i = 0; i < (*tlAnalysisTokens)[position].size(); i++) {
			if ((*tlAnalysisTokens)[position][i] == firstResult[0]) {
				(*tlAnalysisTokens)[position].erase(
						(*tlAnalysisTokens)[position].begin() + i,
						(*tlAnalysisTokens)[position].begin() + i
								+ firstResult.size());
				(*tlAnalysisTokens)[position].insert(
						(*tlAnalysisTokens)[position].begin() + i,
						secondResult.begin(), secondResult.end());
				break;
			}
		}
//		cout << "HERE" << endl;
	}
}

// put the token and its tags in one vector and put tags between "<" , ">"
// the analysis will be done on this vector , "<>" to differ between tags and non-tags
// and the token for the lemma
vector<string> RuleExecution::formatTokenTags(string token,
		vector<string> tags) {

	vector<string> analysisToken;
	analysisToken.push_back(token);

	for (unsigned i = 0; i < tags.size(); i++) {
		analysisToken.push_back("<" + tags[i] + ">");
	}

	return analysisToken;
}

vector<string> RuleExecution::clipAction(xml_node clip,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedTokens, vector<string> tlMatchedTokens,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag, map<int, int> paramToPattern) {

	vector<string> result;

	int position = clip.attribute(POS).as_int() - 1;
	if (paramToPattern.size())
		position = paramToPattern[clip.attribute(POS).as_int()] - 1;

	string langSide = clip.attribute(SIDE).value();
	string part = clip.attribute(PART).value();

	string token;
	vector<string> tags;
	if (langSide == TL) {
		token = tlMatchedTokens[position];
		tags = tlTokenTag[token];
	} else if (langSide == SL) {
		token = slMatchedTokens[position];
		tags = slTokenTag[token];
	}
	if (part == WHOLE) {
		result = formatTokenTags(token, tags);
	} else if (part == LEM) {
		result.push_back(token);
	}
	// part == "attr"
	else {
		result = RuleExecution::findAttrPart(tags, attrs[part]);
	}

	return result;
}

vector<string> RuleExecution::concat(xml_node concat,
		map<string, vector<vector<string> > > attrs,
		vector<string> slMatchedToken, vector<string> tlMatchedToken,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag, map<int, int> paramToPattern) {

	vector<string> result;

	for (xml_node node = concat.first_child(); node; node =
			node.next_sibling()) {

		string nodeName = node.name();
		if (nodeName == CLIP) {
			vector<string> clipResult = clipAction(node, attrs, slMatchedToken,
					tlMatchedToken, slTokenTag, tlTokenTag, paramToPattern);
			result.insert(result.end(), clipResult.begin(), clipResult.end());
		} else if (nodeName == LIT_TAG) {
			string tag = node.attribute(V).value();
			result.push_back("<" + tag + ">");
		} else if (nodeName == LIT) {
			result.push_back(node.attribute(V).value());
		}
	}

	return result;
}

//vector<string> RuleExecution::clipAction(int* position, string* langSide,
//		string* part, map<string, vector<string> > slTokenTag,
//		map<string, vector<string> > tlTokenTag,
//		map<string, vector<vector<string> > > attrs, xml_node clip) {
//
//	vector<string> matchedTags;
//
//	if ((*part) == ATTR) {
//		matchedTags = findAttrPart();
//	}
//
//	return matchedTags;
//}

//int main(int argc, char **argv) {
//	vector<string> tokenTags;
//	tokenTags.push_back("gna_cond");
//	tokenTags.push_back("px2pl");
//	tokenTags.push_back("frm");
//	tokenTags.push_back("p2");
//	tokenTags.push_back("frm");
//	tokenTags.push_back("pl");
//
//	vector<string> t1;
//	t1.push_back("p1");
//	t1.push_back("sg");
//	vector<string> t2;
//	t2.push_back("p2");
//	t2.push_back("sg");
//	vector<string> t3;
//	t3.push_back("p3");
//	t3.push_back("sg");
//	vector<string> t4;
//	t4.push_back("p1");
//	t4.push_back("pl");
//	vector<string> t5;
//	t5.push_back("p2");
//	t5.push_back("pl");
//	vector<string> t6;
//	t6.push_back("p3");
//	t6.push_back("pl");
//	vector<string> t7;
//	t7.push_back("p2");
//	t7.push_back("frm");
//	t7.push_back("sg");
//	vector<string> t8;
//	t8.push_back("p2");
//	t8.push_back("frm");
//	t8.push_back("pl");//
//	vector<vector<string> > attrTags;
//	attrTags.push_back(t5);
//	attrTags.push_back(t6);
//	attrTags.push_back(t1);
//	attrTags.push_back(t2);
//	attrTags.push_back(t3);
//	attrTags.push_back(t4);
//	attrTags.push_back(t7);
//	attrTags.push_back(t8);
//
//	vector<string> matchedTags = RuleExecution::findAttrPart(tokenTags,
//			attrTags);
//
//	for (unsigned i = 0; i < matchedTags.size(); i++) {
//		cout << matchedTags[i] << endl;
//	}
//
//	return 0;
//}

