

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
	string rootDirectory = "/home/students/schutzj/FinalProject/Test/Books"; 
	Index index(rootDirectory);
	index.Build();
	char again;
	do 
	{
		string word;

		//word = getInput(); 
		cout << "Enter word: " ; 
		cin >> word; 
		cout << endl; 
		
		stringstream iss;

		vector<string>instancesSet = index.GetInstancesOf(word);
	
		iss << instancesSet.size();
		
		cout << "Number of entries: " << iss.str() << endl;

		for (int i=0; i < instancesSet.size(); i++)
		{
			cout << instancesSet[i] << endl; 
		}

		cout << "$END"; 

		cout << "Hit A to go again" << endl;
		//checks loop to go again
		cin >> again;

	} while (again == 'A' || again == 'a');

}
    
