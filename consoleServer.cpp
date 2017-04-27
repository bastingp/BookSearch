

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
#include <WORD_INDEX_H.h>

using namespace std;


int main() 
{ 
	string rootDirectory = "rootdirectory"; 
	Index index(rootDirectory);
	index.Build();
	do 
	{

		//word = getInput(); 
		cout << "Enter word: " ; 
		cin >> word; 
		cout << endl; 

		vector<string>instancesSet = index.GetInstancesOf(word);


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
    
