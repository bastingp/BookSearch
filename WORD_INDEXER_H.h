#ifndef WORD_INDEXER_H
#define WORD_INDEXER_H

#include<string>
#include<iostream>
#include<vector>
#include<map>
#include<fstream>
#include<cstdlib>
#include<ctype.h>
#include<stdio.h>
#include<cstdio>
#include<sstream>
#include<algorithm>
#include<dirent.h>

using namespace std;

struct WordLocations
{
	int bookID;
	vector<int> locations;
};

class Index
{
public:
	Index();		//creates index with no reference to a directory too
	
	Index(string rootDirectory);		//creates index to build from the directory root
	
	string GetDirRoot();		//for testing
	
	void ChangeDirRoot(string newRoot);	//changes directory root to newRoot, and clears out index and bookIDRef
	
	void Reset();			//sets dirRoot to empty string, clears index, clears bookIDRef
	
	void Build();			//builds index of every word in every file in the directory root
	
	vector<string> GetInstancesOf(string word);		//returns every line in every file in every subdirectory of dirRoot containing the word "word"
	
private:
	void ProcessDirectory(string directory);		//looks at every entity in a directory
	
	void ProcessEntity(struct dirent* entity);		//checks if the entity is a file or a directory 
	
	void ProcessFile(string file);					//reads through the file, and adds it to our index

	void CarefulOpenIn(ifstream& infile, string name);		//opens file, exits program if attempt fails
	
	vector<WordLocations> GetLocations(string word);				//returns WordLocations of word in index--returns an empty WordLocations if word not in index
	
	vector<string> GetInstancesOfWordInFile(string filePath, vector<int> positions);		//returns all instances of word in filePath
	
	void MakeLower(string& word);

	string dirRoot;
	string path;
	map<string, vector<WordLocations> > index;
	vector<string> bookIDRef;
};

#endif