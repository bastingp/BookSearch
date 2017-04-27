#include "WORD_INDEXER_H.h"

using namespace std;

Index::Index()
{
	dirRoot = "";
}

Index::Index(string rootDirectory)
{
	dirRoot = rootDirectory;
}

string Index::GetDirRoot()
{
	return dirRoot;
}

void Index::Reset()
{
	dirRoot = "";
	index.clear();
	bookIDRef.clear();
}

void Index::ChangeDirRoot(string newRoot)
{
	dirRoot = newRoot;
}

void Index::Build()
{	
	ifstream infile;
	string word, line;
	
	if(dirRoot.empty())		//if filename has not been provided, return
	{
		cout << "\n\nERROR: No file given.\n\n";
		return;
	}
	
	bookIDRef.push_back(dirRoot);
	CarefulOpenIn(infile, dirRoot);
	int pos= 0;
	while(!infile.fail())
	{
		getline(infile, line);
		istringstream iss(line);
		while(iss >> word)
		{
			MakeLower(word);			//store word as lower
			if(!index[word].empty())			//add to current WordLocations
			{
				index[word][0].locations.push_back(pos);			
			}
			else			//otherwise, no WordLocations stored, so one must be added 
			{
				WordLocations loc;
				loc.locations.push_back(pos);
				index[word].push_back(loc);
			}
			
		}
		
		pos = infile.tellg();
	}
	
	infile.close();
}

vector<string> Index::GetInstancesOf(string word)
{	
	vector<WordLocations> locations;

	MakeLower(word);		//make sure word is normalized
	locations = GetLocations(word);

	vector<string> lines;
	for(int i = 0; i < locations.size(); i++)
	{
		vector<string> temp = GetInstancesOfWordInFile(dirRoot, locations[i].locations);		//get every instance of word in each file
		for(int i = 0; i < temp.size(); i++)		
		{
			lines.push_back(temp.at(i));			//and store it in lines
		}
	}
	
	return lines;
}

vector<string> Index::GetInstancesOfWordInFile(string filePath, vector<int> positions)
{
	vector<string> lines;
	ifstream infile;
	CarefulOpenIn(infile, filePath);
	
	string line, word;
	int last_pos = -1;
	for(int i = 0; i < positions.size(); i++)			//output lines starting at each position
	{
		infile.seekg(positions[i], infile.beg);
		getline(infile, line);
		if(positions[i] != last_pos)							//don't output same line multiple times
		{
			istringstream iss(line);
			line = "";
			while(iss >> word)					//add each word into line
			{
				line += word + " ";
			}
		}
		last_pos = positions[i];
		lines.push_back(line);
	}
	
	infile.close();
	return lines;
}

vector<WordLocations> Index::GetLocations(string word)
{
	map<string, vector<WordLocations> >::iterator it;
	vector<WordLocations> empty_locations;							//returned if word is not found in index
	
	it = index.find(word);
	if(it == index.end())					//word is not found
		return empty_locations;
	else
		return index[word];
}

void Index::CarefulOpenIn(ifstream& infile, string name)
{
      infile.open(name.c_str(), ios::binary);			//was getting wrong positions without opening binary
      if(infile.fail())
      {
         cout << "\n\n\nThere was an error opening the input file " << name << ".\n\n";
      }
      return;
}

void Index::MakeLower(string& word)
{
	for(int i = 0; i < word.length(); i++)
	{
		word[i] = tolower(word[i]);
	}
}