#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <string.h>

#include "../pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

#include "RuleParser.h"

void RuleParser::sentenceTokenizer(map<string, vector<string> >* slTokenTag,
		map<string, vector<string> >* tlTokenTag, char* tokenizedSentence) {
	vector<string> taggedTokens;

	char * taggedToken;
	taggedToken = strtok(tokenizedSentence, "^$/ ");
	while (taggedToken != NULL) {
		taggedTokens.push_back(taggedToken);
		taggedToken = strtok(NULL, "^$/ ");
	}

	for (unsigned i = 0; i < taggedTokens.size(); i++) {
		char taggedToken[taggedTokens[i].size()];
		strcpy(taggedToken, taggedTokens[i].c_str());
		char* split;
		split = strtok(taggedToken, "<>");
		string token = split;

		if (i % 2 == 0)
			(*slTokenTag)[token] = vector<string>();
		else
			(*tlTokenTag)[token] = vector<string>();

		split = strtok(NULL, "<>");
		while (split != NULL) {
			string tag = split;

			if (i % 2 == 0)
				(*slTokenTag)[token].push_back(tag);
			else
				(*tlTokenTag)[token].push_back(tag);

			split = strtok(NULL, "<>");
		}
	}

}

void RuleParser::matchCats(map<string, vector<string> >* slTokenCat,
		map<string, vector<string> >* tlTokenCat, xml_node transfer,
		map<string, vector<string> > slTokenTag,
		map<string, vector<string> > tlTokenTag) {

	xml_node section_def_cats = transfer.child("section-def-cats");

	for (xml_node def_cat = section_def_cats.first_child(); def_cat; def_cat =
			def_cat.next_sibling()) {
		for (xml_node cat_item = def_cat.first_child(); cat_item; cat_item =
				cat_item.next_sibling()) {

			string tagsString = cat_item.first_attribute().value();

			char taggedToken[tagsString.size()];
			strcpy(taggedToken, tagsString.c_str());
			char* split;
			split = strtok(taggedToken, ".");

			vector<string> itemTags;

			while (split != NULL) {
				string tag = split;
				itemTags.push_back(tag);

				split = strtok(NULL, ".");
			}

			for (map<string, vector<string> >::iterator slIt =
					slTokenTag.begin(), tlIt = tlTokenTag.begin();
					slIt != slTokenTag.end(); ++slIt, ++tlIt) {
				vector<string> tags = slIt->second;

				unsigned i = 0, j = 0;
				for (; i < tags.size() && j < itemTags.size(); i++) {
					if (itemTags[j] == "*") {
						if (itemTags[j + 1] == tags[i]) {
							j += 2;
						}
					} else if (itemTags[j] == tags[i]) {
						j++;
					} else {
						break;
					}
				}

				if (i == tags.size()
						&& (j == itemTags.size()
								|| (j + 1 == itemTags.size()
										&& itemTags[j] == "*"
										&& itemTags[j - 1] != tags[i - 1]))) {

					(*slTokenCat)[slIt->first].push_back(
							def_cat.first_attribute().value());

					(*tlTokenCat)[tlIt->first].push_back(
							def_cat.first_attribute().value());
				}
			}
		}
	}

}

void RuleParser::matchRules(map<string, vector<xml_node> >* slTokenRule,
		map<string, vector<xml_node> >* tlTokenRule, xml_node transfer,
		map<string, vector<string> > slTokenCat,
		map<string, vector<string> > tlTokenCat) {

	xml_node section_rules = transfer.child("section-rules");

	for (xml_node rule = section_rules.first_child(); rule;
			rule = rule.next_sibling()) {
		xml_node pattern = rule.child("pattern");

		for (map<string, vector<string> >::iterator slIt = slTokenCat.begin(),
				tlIt = tlTokenCat.begin(); slIt != slTokenCat.end();
				++slIt, ++tlIt) {

			bool applyRule = false;
			for (xml_node pattern_item = pattern.first_child(); pattern_item;
					pattern_item = pattern_item.next_sibling()) {

				for (unsigned i = 0; i < slIt->second.size(); i++) {
					// if cat name equals pattern item name
					if (pattern_item.first_attribute().value()
							== slIt->second[i]) {
						applyRule = true;
					}
				}
				if (applyRule)
					break;
			}
			if (applyRule) {
				(*slTokenRule)[slIt->first].push_back(rule);
				(*tlTokenRule)[tlIt->first].push_back(rule);
			}
		}
	}
}
