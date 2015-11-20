//C++ includes
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <cstring>
#include <sstream>

//VIS-SENSE includes
#include "vsCSVFile.h"

using namespace std;

vsFilename::vsFilename(const char *filename)
{
	string fname(filename);
	string path(filename);

	this->FullName = new char[fname.size() + 1];
	strcpy(this->FullName, fname.c_str());

	size_t pos = path.find_last_of( "/\\" );

	fname = fname.substr( pos + 1 );
	path = path.substr(0, pos + 1);

	this->FileName = new char[fname.size() + 1];
	strcpy(this->FileName, fname.c_str());

	this->Path = new char[path.size() + 1];
	strcpy(this->Path, path.c_str());

	pos = fname.find_last_of( "." );

	string extension = fname;
	extension = extension.substr( pos + 1 ); 
	this->Extension = new char[extension.size() + 1];
	strcpy(this->Extension, extension.c_str());

	string bname = fname;
	bname = bname.substr(0, pos);
	this->BaseName = new char[bname.size() + 1];
	strcpy(this->BaseName, bname.c_str());
	int pos_of_ = fname.find_first_of( "_" );
	int pos_of_dot = fname.find_last_of( "." );
	int len=pos_of_dot-pos_of_-1;
	this->name_of_feature =new char[fname.size()];
	strcpy(this->name_of_feature,fname.substr(pos_of_+1, len).c_str());
	//string extension=files[i].substr(pos_of_dot+1);
/*
	fprintf(stderr, "%s\n", this->BaseName);fflush(stderr);
	fprintf(stderr, "%s\n", this->FullName);fflush(stderr);
	fprintf(stderr, "%s\n", this->Path);fflush(stderr);
	fprintf(stderr, "%s\n", this->Extension);fflush(stderr);
	fprintf(stderr, "%s\n", this->FileName);fflush(stderr);
*/
}

vsFilename::~vsFilename(){
	delete []this->FullName;
	delete []this->BaseName;
	delete []this->Extension;
	delete []this->Path;
}

vsLine::vsLine(){
	this->Word = NULL;
	this->NoWords = 0;
}

vsLine::~vsLine(){
	for (int i = 0; i < this->NoWords; i++)
		delete []this->Word[i];
	free(this->Word);
}

void vsLine::addWord(char* word){

	this->Word = (char**) realloc(this->Word, (this->NoWords + 1) * sizeof(char*));
	if (!this->Word){
		fprintf (stderr, "Cannot reallocate memory for array Word\n");
		exit(1);
	}

	this->Word[this->NoWords] = new char[strlen(word) + 1];
	strcpy(this->Word[this->NoWords], word);
	this->NoWords++;
}

vsCSVFile::vsCSVFile(vector<string> files,std::vector<int> numberof_time_periods,std::string name_of_feature){
	this->NoLines = 0;
	//this->Line = NULL;
	this->FileName = NULL;

	if (files.size()>0)
		this->getInput(files,numberof_time_periods,name_of_feature);
}

vsCSVFile::~vsCSVFile(){
	for (int i = 0; i < this->NoLines; i++){
		delete this->Line[i];
	}
	//free(this->Line);

	delete this->FileName;
}

int vsCSVFile::addLine(){
	
	/*this->Line = (vsLine**) realloc(this->Line, (this->NoLines + 1) * sizeof(vsLine*));
	if (!this->Line){
		fprintf (stderr, "Cannot reallocate memory for array Line\n");
		exit(1);
	}
	
	this->Line[this->NoLines] = new vsLine();
	this->NoLines++;

	return this->NoLines;*/
	return 1;
}

bool vsCSVFile::getInput(vector<string>& files,vector<int> numberof_time_periods,string name_of_feature){

	string String;
	string file_name;
	std::string nameofthefile;
	std::string path_of_feature;
	std::string name_of_feature_combination;
	int pos_of_backslash = files[0].find_last_of( "/" );
	path_of_feature = files[0].substr(0, pos_of_backslash+1);
	nameofthefile+=path_of_feature;
	
	int pos_of_parenthesis = files[0].find_last_of( ")" );
	if(pos_of_parenthesis<0) pos_of_parenthesis = files[0].find_last_of( "/" );
	int pos_of_dot = files[0].find_last_of( "." );
	int len=pos_of_dot-pos_of_parenthesis-1;
	file_name = files[0].substr(pos_of_parenthesis+1, len);

	nameofthefile+="_"+file_name+".csv";
	this->FileName = new vsFilename(nameofthefile.c_str());
	
	
	for (int i=0;i<files.size();i++)
	{
		char* filename=new char[100];
		strcpy(filename,files[i].c_str());
		ifstream inFile(filename);
		
		//inFile.open(filename);
		
		
		if ( inFile.is_open() ){

			fprintf(stderr, "Parsing file \"%s\" ... ", filename);fflush(stderr);
			
			int i=0;
			std::ofstream ofs("memory_problem.txt");
			//while ( !inFile.eof() ) // To get you all the lines.
			while (getline(inFile, String)) // Gets a new line and saves the line in STRING.
			{
				ofs<<i<<" "<<String<<" ";
				i++;
				String = this->eraseStrChar(String, ",[]");
				String = this->eraseStrChar(String, "[");
				String = this->eraseStrChar(String, "]");
				String = this->eraseStrChar(String, " ");
				String = this->eraseStrChar(String, ",\"\"");
				
				//char delims[] = ", \t";
				char delims[] = ",";
				
				if (String.empty() || String.find_first_of(delims) >= String.size())
					continue;
				ofs<<"1 ";
				int cL = this->addLine() - 1;
				
				ofs<<"2 ";
				char *line, *p;
				
				line = new char [String.size() + 1];
				strcpy(line, String.c_str());

				p = strtok(line, delims);
				ofs<<"3 ";
				//Parse the feature values
				vsLine* current_Line=new vsLine;
				while (p != NULL){

					//Add word to line
					current_Line->addWord(p);
					//this->Line[cL]->addWord(p);
					p = strtok(NULL, delims);
				}
				Line.push_back(current_Line);
				this->NoLines++;
				delete[] line;
				ofs<<"4 "<<"\n";
			}
			//delete[] filename;
			inFile.close();
			fprintf(stderr, "OK\n");fflush(stderr);

		}
		else{
			fprintf(stderr, "Cannot open file \"%s\". Exiting ...\n", filename);fflush(stderr);
			exit(1);
		}
	}
	return true;

}

string vsCSVFile::eraseStrChar(string str, const char *erase) {
	
	bool eraseString = false;
	if ( strlen(erase) > 1 )
		eraseString = true;

	//Set our locator equal to the first appearance of any character in replace
	size_t found;
	if (eraseString)
		found = str.find(erase);
	else
		found  = str.find_first_of(erase);

	while (found != string::npos) { // While our position in the sting is in range.
		str.erase(found, strlen(erase));

		if (eraseString)
			found = str.find(erase, found + 1);
		else
			found  = str.find_first_of(erase, found + 1);
			//found = str.find_first_of(erase, found + 1); // Relocate again.
	}

	return str; // return our new string.
}

void vsCSVFile::printSelf(){
	for (int i = 0; i < this->NoLines; i++){
		fprintf(stderr, "%s", this->Line[i]->Word[0]);fflush(stderr);

		for (int j = 1; j < this->Line[i]->NoWords; j++){
			fprintf(stderr, ",%s", this->Line[i]->Word[j]);fflush(stderr);
		}
		fprintf(stderr, "\n");fflush(stderr);
	}
}
