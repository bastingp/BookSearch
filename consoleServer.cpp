

#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <map>
#include <algorithm>
#include <string>
#include <vector>
#include "WORD_INDEXER_H.h"

using namespace std;


int main() 
{ 
	string rootDirectory = "/home/skon/books/"; 
	Index index(rootDirectory);
	index.Build();
	char again;
	vector<string> matchLines;
//	do 
//	{
//		string word;

		//word = getInput(); 
//		cout << "Enter word: " ; 
//		cin >> word; 
//		cout << endl; 

	//	matchLines = index.GetInstancesOf(word);

	//	 for (int i=0; i < matchLines.size(); i++)
	//	 {
	//	 	cout << matchLines[i] << endl; 
	//	 }

		// cout << "$END"; 

	//	cout << "Hit A to go again" << endl;
		//checks loop to go again
	//	cin >> again;

//	} while (again == 'A' || again == 'a');

}
    
