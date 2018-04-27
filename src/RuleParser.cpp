#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <string.h>

#include "pugixml.hpp"

using namespace std;
using namespace pugi;

#include "RuleParser.h"

int main(int argc, char **argv) {
	// Tokenize sentence part
	char* tokenizedSentence;

	if (argc == 2) {
		tokenizedSentence = argv[1];
	}

	vector<string> taggedTokens;

	char * taggedToken;
	taggedToken = strtok(tokenizedSentence, "^$ ");
	while (taggedToken != NULL) {
		//		printf("%s\n", taggedToken);
		taggedTokens.push_back(taggedToken);
		taggedToken = strtok(NULL, "^$ ");
	}

	map<string, vector<string> > tokenTag;

	for (unsigned i = 0; i < taggedTokens.size(); i++) {
		char taggedToken[taggedTokens[i].size()];
		strcpy(taggedToken, taggedTokens[i].c_str());
		char* split;
		split = strtok(taggedToken, "<>");
		string token = split;
		tokenTag[token] = vector<string>();

//		printf("Token : %s, Tags : ", token.c_str());
		cout << "Token : " << token.c_str() << " , Tags : ";

		split = strtok(NULL, "<>");
		while (split != NULL) {
			string tag = split;
			tokenTag[token].push_back(tag);
//			printf(" %s , ", tag.c_str());
			cout << " " << tag.c_str() << " , ";
			split = strtok(NULL, "<>");
		}
		cout << endl;
//		printf("\n");
	}

	// rules parsing part

	map<string, vector<string> > tokenCat;

	xml_document doc;
	xml_parse_result result = doc.load_file("rules.t1x");

//	printf("%s\n", result.description());
	cout << result.description() << endl;

	xml_node transfer = doc.child("transfer");

	xml_node section_def_cats = transfer.child("section-def-cats");

//	cout << section_def_cats.name() << endl;
//	printf("%s\n", section_def_cats.name());

	for (xml_node def_cat = section_def_cats.first_child(); def_cat; def_cat =
			def_cat.next_sibling()) {
		for (xml_node cat_item = def_cat.first_child(); cat_item; cat_item =
				cat_item.next_sibling()) {
			string tagsString = cat_item.first_attribute().value();

//			cout << tagsString << endl;

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

			for (map<string, vector<string> >::iterator it = tokenTag.begin();
					it != tokenTag.end(); ++it) {
				vector<string> tags = it->second;

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

					tokenCat[it->first].push_back(
							def_cat.first_attribute().value());
				}
			}
		}
	}

	map<string, vector<xml_node> > tokenRule;

	xml_node section_rules = transfer.child("section-rules");

	for (xml_node rule = section_rules.first_child(); rule;
			rule = rule.next_sibling()) {
		xml_node pattern = rule.child("pattern");

		for (map<string, vector<string> >::iterator it = tokenCat.begin();
				it != tokenCat.end(); ++it) {

			bool applyRule = false;
			for (xml_node pattern_item = pattern.first_child(); pattern_item;
					pattern_item = pattern_item.next_sibling()) {

				for (unsigned i = 0; i < it->second.size(); i++) {
					// if cat name equals pattern item name
					if (pattern_item.first_attribute().value()
							== it->second[i]) {
						applyRule = true;
					}
				}
				if (applyRule)
					break;
			}
			if (applyRule) {
				tokenRule[it->first].push_back(rule);
			}
		}
	}

//	for (map<string, vector<string> >::iterator it = tokenCat.begin();
//			it != tokenCat.end(); ++it) {
//		cout << "Token: " << it->first << " ,cat names: ";
//		vector<string> cats = it->second;
//		for (unsigned i = 0; i < cats.size(); i++) {
//			cout << cats[i] << " , ";
//		}
//		cout << endl;
//	}
//
	for (map<string, vector<xml_node> >::iterator it = tokenRule.begin();
			it != tokenRule.end(); ++it) {
		cout << "Token: " << it->first << " ,rule comments: " << endl;
		vector<xml_node> rules = it->second;
		for (unsigned i = 0; i < rules.size(); i++) {
			cout << rules[i].first_attribute().value() << endl;
			cout << "pattern item names : " << endl;

			xml_node pattern = rules[i].child("pattern");
			for (xml_node pattern_item = pattern.first_child(); pattern_item;
					pattern_item = pattern_item.next_sibling()) {
				cout << "  " << pattern_item.first_attribute().value();
			}
			cout << endl;
		}
		cout << endl << endl;
	}

	return 0;
}
