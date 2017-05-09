#include "WORD_INDEXER_H.h"

using namespace std;

Index::Index()
{
	dirRoot = "";
	BuildBookPathsMap();
	
}

Index::Index(string rootDirectory)
{
	dirRoot = rootDirectory;
	BuildBookPathsMap();
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
	
	bookIDRef.clear();
	path = dirRoot;			//begin the path at the directory root
	BuildStopWords();
	ProcessDirectory("");		//process every file beginning at directory root
	WriteMapToFile();		//ensures remaining map values ge written to file
	pairIndex.clear();
	BuildBookPathsMap();
}

void Index::ProcessDirectory(string directory)
{
	string dirToOpen = path + directory;
	DIR *dir;
	dir = opendir(dirToOpen.c_str());

	//set the new path for the content of the directory
	path = dirToOpen + "/";
	if(NULL == dir)
	{
		cout << "could not open directory: " << dirToOpen.c_str() <<\
		endl;
		return;
	}
	struct dirent *entity;
	entity = readdir(dir);

	while(entity != NULL)
	{
		ProcessEntity(entity);
		entity = readdir(dir);
	}

	//we finished with the directory so remove it from the path
	path.resize(path.length() - 1 - directory.length());
	closedir(dir);
}

void Index::ProcessEntity(struct dirent* entity)
{
	//find entity type
	if(entity->d_type == DT_DIR) 	//it's a directory
	{
		if(entity->d_name[0] == '.')		//don't process meta files!
		{
			return;
		}
		else	//it's a directory--send it to be processed
		{
			ProcessDirectory(string(entity->d_name));
			return;
		}
	}

	if(entity->d_type == DT_REG)		//it's a regular file
	{
		ProcessFile(string(entity->d_name));		//so send it to be processed
		return;
	}
	
	cout << "Not a file or directory: " << entity->d_name << endl;
}

void Index::ProcessFile(string file)
{
	string fileType = ".txt";
	ifstream infile;
	string line, word;
	if (hasEnding(file,fileType)) 		//Make sure it's a text file
	{  
		fileCount++;
		cout << "processing file# "<<fileCount<<":"<<path << file <<endl;
		string bookFileName = bookDir;
		ofstream bookFile(bookFileName.c_str(), ios::out | ios::app); //opens file to output file paths to
		long int booknum = bookFile.tellp();			//gets start location of path in file
		bookIDRef.push_back(booknum);
		ofstream bookVectorStorage(bookPathPos.c_str(), ios::out | ios::app); //stores book vector as file
		bookVectorStorage << booknum << endl;
		bookFile << (path + file);
		bookFile << endl;
		
		bookFile.close();
		unsigned short int bookPathIndex = bookIDRef.size() - 1;
		AddWordsToMap(bookPathIndex, (path+file));
		if (fileCount % 2000 == 0) //how many files stored in map
		{			   //until written to file
			WriteMapToFile();
			pairIndex.clear();
		}
	}
}

vector<string> Index::GetInstancesOf(string word)
{	
	vector<string> instancesOfWord;
	map<unsigned short int, vector<int> > wordMap;	
	BuildWordMap(word, wordMap);
	
	map<unsigned short int, vector<int> >::iterator it;
	string line;
	for(it = wordMap.begin(); it != wordMap.end(); it++)		//read through every book the word appears in
	{
		ifstream bookfile;
		if(CarefulOpenIn(bookfile, bookPaths.at(it->first)))		//open each book file
		{
			for(int j = 0; j < it->second.size(); j++)		//and get each line in the book the word appears in
			{
				bookfile.seekg(it->second.at(j), bookfile.beg);					//and move to the offset
				getline(bookfile, line);
				instancesOfWord.push_back(line);				//add the line to instancesOfWord
				cout << line << endl;
			}
		}
	}
	
	return instancesOfWord;
}

void Index::BuildBookIDVector()
{
	string value;
	bookIDRef.clear();
	ifstream bookPathPositions;
	bookPathPositions.open(bookPathPos, ios::in); //file containing positions of paths in bookpath file
	bookPathPositions.seekg(0,bookPathPositions.beg);
	while(!bookPathPositions.eof())
	{
		getline(bookPathPositions,value);
		if(value.length() >0)
		{
			bookIDRef.push_back(stol(value)); //populates vector with values from file
		}
	}
}

void Index::AddWordsToMap(unsigned short int bookIndex, string bookPath){
	ifstream bookFile;
	string line, word;
	if(CarefulOpenIn(bookFile, bookPath)){
		int pos = 0;
		while(!bookFile.fail())			//read through the whole file
		{
			getline(bookFile, line);			//get each line
			istringstream iss(line);
			while(iss >> word)				//read each word
			{
				MakeLower(word);
				if (!IsStopWord(word)) //checks if it is a stop word
				{			//store word as lower				
					wordPair = make_pair(bookIndex,pos);
					pairIndex[word].push_back(wordPair);
				}
			}
			
			pos = bookFile.tellg();
		}
		bookFile.close();
	}
}

void Index::WriteMapToFile(){
	cout << "writing words to word file...." << endl;
	map<string, vector<pair<unsigned short int, int> > >::iterator it;
	string fileName;
	unsigned short int bookPathIndex;
	int pos;
	for (it = pairIndex.begin(); it != pairIndex.end(); it++) //for every key in the map
	{
		for (int i = 0; i < it->second.size(); i++) 			//for every value in the vector of pairs
		{
			fileName = wordsDir + (it->first) + wordsFileType;
			bookPathIndex = (it->second)[i].first; 				//first value from pair: book index in vector of book positions
			pos = (it->second)[i].second;						//secont value from pair: position of word in file
			ofstream wordFileOut(fileName.c_str(), ios::out | ios::binary | ios::app); //file to store book locations and word locations
			wordFileOut.write((char*)&bookPathIndex, sizeof(unsigned short int));
			wordFileOut.write((char*)&pos, sizeof(int));
			wordFileOut.close();
		}
	}
}

/////////////////////////////////////
//NOT CURRENTLY USING THIS FUNCTION//
/////////////////////////////////////
vector<string> Index::GetInstancesOfWordInFile(string filePath, vector<int> positions)
{
	vector<string> lines;
	ifstream infile;
	if(CarefulOpenIn(infile, filePath))
	{
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
	}
	
	return lines;
}

BookMap Index::GetLocations(string word)
{
	map<string, BookMap>::iterator it;
	BookMap emptyBookMap;							//returned if word is not found in index
	
	it = index.find(word);
	if(it == index.end())					//word is not found
		return emptyBookMap;
	else
		return index[word];
}

bool Index::CarefulOpenIn(ifstream& infile, string name)
{
      infile.open(name.c_str(), ios::binary);			//was getting wrong positions without opening binary
      if(infile.fail())
      {
         cout << "\n\n\nThere was an error opening the input file " << name << ".\n\n";
		 return false;
      }
      return true;
}

void Index::MakeLower(string& word)
{
	if(!word.empty())
	{
		while(!isalpha(word.front()) && !word.empty())			//if the word begins with punctuation (i.e., if it's in quotes),
		{
			word.erase(word.begin());					//don't store the punctuation
		}
	}
			
	if(!word.empty())
	{
		while(!isalpha(word.back()))		//do the same if it ends with punctuation
		{
			word.pop_back();
		}
	}

	if (word.size() > 0)
	{
		for(int i = 0; i < word.size(); i++)
		{
			word[i] = tolower(word[i]);		//make word lowercase
			if (!isalpha(word[i]))			//if any internal char is punctiation, erase word
			{
				word = "";
			}
		}
	}
}

bool Index::hasEnding(string const &fullString, string const &ending) 
{
	if (fullString.length() >= ending.length()) 
	{
		return (0 == fullString.compare (fullString.length() - ending.\
						 length(), ending.length(), ending));
	} 
	else 
	{
		return false;
	}
}

void Index::BuildStopWords()
{
    string line, word; 
	ifstream infile("NewStop.txt"); // open the file
    int position = 0; 
    while (!infile.fail()) 
	{
        getline(infile, line); // get the next line of code
        istringstream lineStream(line); // Create a string stream of the line
        while (lineStream >> word) 
		{ // get the next word
            stopWords[word];
            // push the word and the line position on the vector for this word
        }
        position = infile.tellg(); // get the poistion of the next line
    }
}

bool Index::IsStopWord(string word)
{
	map<string, short int>::iterator it;	
	it = stopWords.find(word);
    if (it == stopWords.end() || word == "") 
	{
         return false;
    } 
	else 
	{
        return true;
    }
}


string Index::GetTitle(ifstream& infile)
{
	///////NOT DONE YET////////////////
	int pos = 0;
	string line;
	infile.seekg(pos, infile.beg);
	getline(infile, line);
}

void Index::BuildBookPathsMap()
{
	ifstream infile;
	if(CarefulOpenIn(infile, bookDir))
	{
		bookPaths.clear();
		string line;
		while(!infile.eof())
		{
			getline(infile, line);
			cout << line << endl;
			bookPaths.push_back(line);
		}
		
		infile.close();
	}
	else
	{
		cout << "\n\nERROR: Couldn't open book paths file\n\n";
	}
	
	stringstream iss;
	iss << bookPaths.size();
	cout << "\n\nTotal book paths: " << iss.str();
}

void Index::BuildWordMap(string word, map<unsigned short int, vector<int> >& wordMap)
{
	MakeLower(word);		//normalize word
	string newFileName = wordsDir+word+wordsFileType;			//get the word file name
	ifstream wordFile(newFileName.c_str(), ios::in | ios::binary);		//open it to read in binary
	
	if(wordFile.fail())		//word doesn't exist, just return
	{
		cout << "\nNo file for word...\n";
		return;
	}
	
	wordMap.clear();			//clear out word map just in case, to ensure there's no garbage
	
	unsigned short int bookIndex;
	int offset;
	if (wordFile.is_open())
	{
		while (!wordFile.eof())		//read through whole file
		{
			wordFile.read((char*)&bookIndex, sizeof(unsigned short int));		//check which book to look in
			if(wordFile.eof())		//don't add extra junk at the end of the file
			{
				break;
			}
			wordFile.read((char*)&offset, sizeof(int));			//check where in the book to look
			
			wordMap[bookIndex].push_back(offset);		//add entry to our map
		}
	}
}

