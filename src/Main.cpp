#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <string.h>

#include "../pugixml/pugixml.hpp"
#include "RuleParser.h"
#include "RuleExecution.h"
#include "TranElemLiterals.h"

using namespace std;
using namespace pugi;
using namespace elem;

int
main (int argc, char **argv)
{

  char* tokenizedSentence;

  if (argc == 2)
    {
      tokenizedSentence = argv[1];
    }

  // tokens in the sentence order
  vector<string> slTokens, tlTokens;
  // map of tokens and their tags
  map<string, vector<string> > slTokenTag, tlTokenTag;

  RuleParser::sentenceTokenizer (&slTokenTag, &tlTokenTag, &slTokens, &tlTokens,
				 tokenizedSentence);

  // load transfer file in an xml document object
  xml_document transferDoc;
  xml_parse_result result = transferDoc.load_file ("transferFile.t1x");

  if (string (result.description ()) != "No error")
    {
      cout << "ERROR : " << result.description () << endl;
      return -1;
    }
  // xml node of the parent node (transfer) in the transfer file
  xml_node transfer = transferDoc.child ("transfer");

  // map of tokens and their matched categories
  map<string, vector<string> > slTokenCat, tlTokenCat;

  RuleParser::matchCats (&slTokenCat, &tlTokenCat, transfer, slTokenTag, tlTokenTag);

  // map of tokens and their matched rules
  map<xml_node, vector<vector<string> > > slTokenRule, tlTokenRule;

//	for (unsigned int i = 0; i < slTokens.size(); i++) {
//		cout << slTokens[i] << " : ";
//		for (unsigned int j = 0; j < slTokenCat[slTokens[i]].size(); j++) {
//			cout << slTokenCat[slTokens[i]][j] << " ";
//		}
//		cout << endl;
//	}
//	cout << endl;
//	for (unsigned int i = 0; i < tlTokens.size(); i++) {
//		cout << tlTokens[i] << "  ";
//		for (unsigned int j = 0; j < tlTokenCat[tlTokens[i]].size(); j++) {
//			cout << tlTokenCat[tlTokens[i]][j] << " ";
//		}
//		cout << endl;
//	}
//	cout << endl;

  RuleParser::matchRules (&slTokenRule, &tlTokenRule, slTokens, tlTokens, transfer,
			  slTokenCat, tlTokenCat);

  map<string, vector<vector<string> > > attrs = RuleParser::getAttrs (transfer);

  for (map<xml_node, vector<vector<string> > >::iterator it = slTokenRule.begin ();
      it != slTokenRule.end (); ++it)
    {
      xml_node rule = it->first;
      for (unsigned i = 0; i < slTokenRule[rule].size (); i++)
	{
	  vector<vector<string> > slAnalysisTokens, tlAnalysisTokens;
	  vector<string> slMatchedTokens = slTokenRule[rule][i];
	  vector<string> tlMatchedTokens = tlTokenRule[rule][i];
	  for (unsigned j = 0; j < slMatchedTokens.size (); j++)
	    {
	      string slToken = slMatchedTokens[j];
	      vector<string> slAnalysisToken = RuleExecution::formatTokenTags (
		  slToken, slTokenTag[slToken]);
	      slAnalysisTokens.push_back (slAnalysisToken);

	      string tlToken = tlMatchedTokens[j];
	      vector<string> tlAnalysisToken = RuleExecution::formatTokenTags (
		  tlToken, tlTokenTag[tlToken]);
	      tlAnalysisTokens.push_back (tlAnalysisToken);
	    }
	  vector<string> output = RuleExecution::ruleExe (rule, &slAnalysisTokens,
							  &tlAnalysisTokens, attrs);

	  cout << endl << "RULE : " << rule.first_attribute ().value () << endl;
	  cout << "--------------------------" << endl;

	  cout << "sl analysis" << endl;
	  for (unsigned x = 0; x < slAnalysisTokens.size (); x++)
	    {
	      for (unsigned y = 0; y < slAnalysisTokens[x].size (); y++)
		{
		  cout << slAnalysisTokens[x][y] << "  ";
		}
	      cout << endl;
	    }

	  cout << "tl analysis" << endl;
	  for (unsigned x = 0; x < tlAnalysisTokens.size (); x++)
	    {
	      tlAnalysisTokens[x].size ();

	      for (unsigned y = 0; y < tlAnalysisTokens[x].size (); y++)
		{
		  cout << tlAnalysisTokens[x][y] << "  ";
		}
	      cout << endl;
	    }

	  // rule's output
	  cout << endl << endl << "OUTPUT" << endl;
	  cout << "---------------------" << endl << endl;
	  for (unsigned i = 0; i < output.size (); i++)
	    {
	      cout << output[i];
	    }
	  cout << endl << endl;
	}
    }

//	xml_node rule;
//	string comment = "REGLA: gpr_past - gpr_past id= gpr_past-gpr_past-44";
//	for (map<xml_node, vector<vector<string> > >::iterator it =
//			slTokenRule.begin(); it != slTokenRule.end(); ++it) {
//		if (it->first.first_attribute().value() == comment) {
//			rule = (it->first);
//		}
//	}
//
//	vector<string> concatResult = RuleExecution::concat(
//			rule.child(ACTION).child(LET).child(CONCAT), attrs,
//			slTokenRule[rule][0], tlTokenRule[rule][0], slTokenTag, tlTokenTag);
//
//	cout << concatResult.size() << endl;
//	for (unsigned i = 0; i < concatResult.size(); i++) {
//		cout << concatResult[i] << endl;
//	}

//  // printing tokens and their matched categories
//	for (map<string, vector<string> >::iterator it = slTokenCat.begin();
//			it != slTokenCat.end(); ++it) {
//		cout << "Token: " << it->first << " ,cat names: ";
//		vector<string> cats = it->second;
//		for (unsigned i = 0; i < cats.size(); i++) {
//			cout << cats[i] << " , ";
//		}
//		cout << endl;
//	}

//  // printing source languages tokens
//	for (unsigned i = 0; i < slTokens.size(); i++) {
//		cout << slTokens[i] << endl;
//		cout << tlTokens[i] << endl;
//	}

////printing rules and matched tokens
//	for (map<xml_node, vector<vector<string> > >::iterator it =
//			slTokenRule.begin(); it != slTokenRule.end(); ++it) {
//		cout << "The applied rule : " << it->first.first_attribute().value()
//				<< endl;
//
//		cout << "The pattern-items :  ";
//		xml_node pattern = it->first.child("pattern");
//		for (xml_node pattern_item = pattern.first_child(); pattern_item;
//				pattern_item = pattern_item.next_sibling()) {
//			cout << pattern_item.first_attribute().value() << "  ";
//		}
//		cout << endl << endl;
//
//		cout << "The applied tokens : " << endl;
//
//		for (unsigned i = 0; i < it->second.size(); i++) {
//			for (unsigned j = 0; j < it->second[i].size(); j++) {
//				cout << it->second[i][j] << "  ";
//			}
//			cout << endl;
//		}
//		cout << endl;
//	}

//// printing attributes
//	for (map<string, vector<vector<string> > >::iterator it = attrs.begin();
//			it != attrs.end(); ++it) {
//		cout << "The attr name : " << it->first << endl;
//
//		cout << "The attr-tags :  ";
//
//		for (unsigned int i = 0; i < it->second.size(); i++) {
//			for (unsigned int k = 0; k < it->second[i].size(); k++) {
//				cout << it->second[i][k] << "  ";
//			}
//			cout << endl << endl;
//		}
//		cout << endl << endl;
//	}

}
