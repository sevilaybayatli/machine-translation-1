#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string.h>

#include "../pugixml/pugixml.hpp"
#include "TranElemLiterals.h"

using namespace std;
using namespace pugi;
using namespace elem;

#include "RuleParser.h"

void RuleParser::sentenceTokenizer(map<string, vector<string> >* slTokenTag,
		map<string, vector<string> >* tlTokenTag, vector<string>* slTokens,
		vector<string>* tlTokens, char* tokenizedSentence) {
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

		if (i % 2 == 0) {
			(*slTokenTag)[token] = vector<string>();
			slTokens->push_back(token);
		} else {
			(*tlTokenTag)[token] = vector<string>();
			tlTokens->push_back(token);
		}

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

	xml_node section_def_cats = transfer.child(SECTION_DEF_CATS);

	for (xml_node def_cat = section_def_cats.child(DEF_CAT); def_cat; def_cat =
			def_cat.next_sibling()) {
		for (xml_node cat_item = def_cat.child(CAT_ITEM); cat_item; cat_item =
				cat_item.next_sibling()) {

			string tagsString = cat_item.attribute(TAGS).value();

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
							def_cat.attribute(N).value());

					(*tlTokenCat)[tlIt->first].push_back(
							def_cat.attribute(N).value());
				}
			}
		}
	}

}

void RuleParser::matchRules(
		map<xml_node, vector<vector<string> > >* slTokenRule,
		map<xml_node, vector<vector<string> > >* tlTokenRule,
		vector<string> slTokens, vector<string> tlTokens, xml_node transfer,
		map<string, vector<string> > slTokenCat,
		map<string, vector<string> > tlTokenCat) {

	xml_node section_rules = transfer.child(SECTION_RULES);

	for (xml_node rule = section_rules.child(RULE); rule;
			rule = rule.next_sibling()) {
		xml_node pattern = rule.child(PATTERN);

		// Put pattern items in vector for ease in processing
		vector<xml_node> pattern_items;
		for (xml_node pattern_item = pattern.child(PATTERN_ITEM); pattern_item;
				pattern_item = pattern_item.next_sibling()) {
			pattern_items.push_back(pattern_item);
		}

		for (unsigned i = 0;
				(tlTokens.size() >= pattern_items.size())
						&& i <= tlTokens.size() - pattern_items.size(); i++) {
//			cout << tlTokens.size() << "  " << pattern_items.size() << endl;

			vector<string> slMatchedTokens, tlMatchedTokens;
			for (unsigned j = 0; j < pattern_items.size(); j++) {

				// match cat-item with pattern-item
				string tlToken = tlTokens[i + j], slToken = slTokens[i + j];
				vector<string> cats = slTokenCat[slToken];
				for (unsigned k = 0; k < cats.size(); k++) {
					// if cat name equals pattern item name
					if (pattern_items[j].attribute(N).value() == cats[k]) {
						tlMatchedTokens.push_back(tlToken);
						slMatchedTokens.push_back(slToken);
						break;
					}
				}
			}
			// if matched tokens' size = pattern items' size
			// then this rule is matched
			if (tlMatchedTokens.size() == pattern_items.size()) {
				(*tlTokenRule)[rule].push_back(tlMatchedTokens);
				(*slTokenRule)[rule].push_back(slMatchedTokens);
			}

		}

	}
}

// to sort attribute tags descendingly
bool sortParameter(vector<string> a, vector<string> b) {
	return (a.size() > b.size());
}

map<string, vector<vector<string> > > RuleParser::getAttrs(xml_node transfer) {
	map<string, vector<vector<string> > > attrs;
	xml_node section_def_attrs = transfer.child(SECTION_DEF_ATTRS);

	for (xml_node def_attr = section_def_attrs.child(DEF_ATTR); def_attr;
			def_attr = def_attr.next_sibling()) {

		vector<vector<string> > allTags;
		for (xml_node attr_item = def_attr.child(ATTR_ITEM); attr_item;
				attr_item = attr_item.next_sibling()) {
			// splitting tags by '.'
			string tagsString = attr_item.attribute(TAGS).value();
			char tagsChars[tagsString.size()];
			strcpy(tagsChars, tagsString.c_str());

			vector<string> tags;

			char * tag;
			tag = strtok(tagsChars, ".");
			while (tag != NULL) {
				tags.push_back(tag);
				tag = strtok(NULL, ".");
			}

			allTags.push_back(tags);
		}
		// sort the tags , descendingly by their size
		sort(allTags.begin(), allTags.end(), sortParameter);
		attrs[def_attr.attribute(N).value()] = allTags;
	}

	return attrs;
}
