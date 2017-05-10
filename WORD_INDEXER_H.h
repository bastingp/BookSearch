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
#include<utility>

using namespace std;

class Index
{
public:
	Index();		//creates index with no reference to a directory
	
	Index(string rootDirectory);		//creates index to build from the directory root
	
	string GetDirRoot();		//for testing
	
	void ChangeDirRoot(string newRoot);	//changes directory root to newRoot, and clears out index and bookIDRef
	
	void Reset();			//resets all info to defaults
	
	void Build();			//builds index of every word in every file in the directory root
	
	vector<string> GetInstancesOf(string word);		//returns every line in every file in every subdirectory of dirRoot containing the word "word"
	
	vector<string> GetInstancesOf(string word, int startingIndex, int numBooks);		//returns every line in numBooks number of files, beginning at startingIndex for word word
	
private:
	void ProcessDirectory(string directory);		//looks at every entity in a directory
	
	void ProcessEntity(struct dirent* entity);		//checks if the entity is a file or a directory 
	
	void ProcessFile(string file);					//reads through the file, and adds it to our index

	bool CarefulOpenIn(ifstream& infile, string name);		//opens file, returns false if attempt fails
	
	void MakeLower(string& word);		//changes everything to lowercase, removes punctuation
	
	bool hasEnding (string const &fullString, string const &ending);			//returns true if fullString ends with "ending"
	
	void BuildStopWords(); //builds a stopWords map from stopwords.txt 
	
	bool IsStopWord (string word); // checks to see if the word is a stopWord

	void AddWordsToMap(unsigned short int bookIndex, string bookPath);		//builds pairIndex from bookPath file

	void WriteMapToFile();			//writes all book paths and offsets to each word file, clears buffer
	
	void BuildBookPathsMap();  //builds a vector of book path strings based on the bookPathsFile
	
	void BuildWordMap(string word);			//builds map from file associated with word, to store locally
	
	void BuildBookInfo();			//builds map of info for each book to store locally
	
	string GetBookInfo(int bookIndex);		//returns the book info for the book at bookIndex 
	
	/* Variables for searching index */
	vector<string> bookPaths;		//stores all book paths search word appears in
	map<int, string> bookInfo;		//stores all book info search word appears in
	string lastWordSearchedFor;		//remembers the last word user searched for, so that wordMap doesn't need to be rebuilt
	map<unsigned short int, vector<int> > wordMap;		//stores in memory all books and offsets word appears in

	/* Variables for building index */
	string dirRoot;				//root from which index is built
	string path;				//used recursively to traverse directories while building index
	const string bookPathsFile = "/home/students/bastingp/project6/books/book_paths.txt";		//dir containing file which contains all paths for all files
	const string wordsDir = "/home/students/bastingp/project6/words/";			//directory where word files are stored
	const string wordsFileType = ".bin";								//file type for word files
	map<string, vector<pair<unsigned short int, int> > > wordBuildMap;		//used to store word info in buffer before writing to files
	map<string, short int> stopWords;				//list of stop words for search
	int fileCount =0; 			//counts the number of files in storage
};

#endif
