#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <string.h>

//#include "../pugixml/pugixml.hpp"

using namespace std;
//using namespace pugi;

//int main(int argc, char **argv) {
//
////	cout << argc << endl;
//
////	for (int i = 1; i < argc; i++) {
////		cout << argv[i] << endl;
////	}
//
//	char* tokenizedSentence;
//
//	if (argc == 2) {
//		tokenizedSentence = argv[1];
//	}
//
//	vector<string> taggedTokens;
//
//	char * taggedToken;
//	taggedToken = strtok(tokenizedSentence, "^$ ");
//	while (taggedToken != NULL) {
////		printf("%s\n", taggedToken);
//		taggedTokens.push_back(taggedToken);
//		taggedToken = strtok(NULL, "^$ ");
//	}
//
//	map<string, vector<string> > tokens;
//
//	for (unsigned i = 0; i < taggedTokens.size(); i++) {
//		char taggedToken[taggedTokens[i].size()];
//		strcpy(taggedToken, taggedTokens[i].c_str());
//		char* split;
//		split = strtok(taggedToken, "<>");
//		string token = split;
//		tokens[token] = vector<string>();
//
//		printf("Token : %s, Tags : ", token.c_str());
//
//		split = strtok(NULL, "<>");
//		while (split != NULL) {
//			string tag = split;
//			tokens[token].push_back(tag);
//			printf(" %s , ", tag.c_str());
//			split = strtok(NULL, "<>");
//		}
//		printf("\n");
//	}
//
//	return 0;
//}
