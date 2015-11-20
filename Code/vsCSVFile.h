#ifndef VSCSVFILE_H_
#define VSCSVFILE_H_

#include <string.h>
#include <string>
#include <vector>
/*
#include <cstring>
*/

class vsFilename{
public:
	vsFilename(const char *);
	~vsFilename();

	char* FullName;
	char* FileName;
	char* BaseName;
	char* Extension;
	char* Path;
	char* name_of_feature;
};

class vsLine{
public:
	vsLine();
	~vsLine();

	void addWord(char* word);

	char** Word;
	int NoWords;
};

class vsCSVFile{
public:
	vsCSVFile(std::vector<std::string> files = std::vector<std::string>(),std::vector<int> numberof_time_periods=std::vector<int>(),std::string name_of_features="");
	~vsCSVFile();

	vsFilename* FileName;

	int addLine();
	bool getInput(std::vector<std::string>& files,std::vector<int> numberof_time_periods=std::vector<int>(),std::string name_of_features="");
	std::string eraseStrChar(std::string str, const char *erase);
	void printSelf();
	std::vector<vsLine*> Line;
	//vsLine** Line;
	int NoLines;

};

#endif

