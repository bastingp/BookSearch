#include "WORD_INDEXER_H.h"

using namespace std;

Index::Index()
{
	dirRoot = "";
	lastWordSearchedFor = "";			//make search that this won't match user's first search
	cout << "\n\nBuilding book map....\n\n";
	BuildBookPathsMap();						//store book paths in memory, to reduce file opening	
	cout << "\n\nBuilding book info map...\n\n";
	BuildBookInfo();							//store info for each book file in memory (title, author)
	cout << "\n\nDone building\n\n";
}

Index::Index(string rootDirectory)
{
	dirRoot = rootDirectory;
	lastWordSearchedFor = "";			//make search that this won't match user's first search
	cout << "\n\nBuilding book map....\n\n";
	BuildBookPathsMap();						//store book paths in memory, to reduce file opening
	cout << "\n\nBuilding book info map...\n\n";
	BuildBookInfo();							//store info for each book file in memory (title, author)
	cout << "\n\nDone building\n\n";
}

string Index::GetDirRoot()
{
	return dirRoot;
}

void Index::Reset()
{
	dirRoot = "";
	lastWordSearchedFor = "";
	wordBuildMap.clear();
	bookPaths.clear();
	bookInfo.clear();
	wordMap.clear();
	
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
	
	path = dirRoot;			//begin the path at the directory root
	BuildStopWords();
	ProcessDirectory("");		//process every file beginning at directory root
	WriteMapToFile();		//any words leftover from buffer are written to file
	wordBuildMap.clear();
	BuildBookPathsMap();		//rebuild bookPath map now that there are files in the directories
	BuildBookInfo();			//rebuild bookInfo map now that there are files in the directories
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
		fileCount++;			//we're reading a new file, so increment fileCount
		cout << "processing file# "<<fileCount<<":"<<path << file <<endl;
		
		ofstream bookFile(bookPathsFile.c_str(), ios::out | ios::app); //opens file to output file paths to
		long int booknum = bookFile.tellp();			//gets start location of path in file
		bookFile << (path + file);
		bookFile << endl;		
		bookFile.close();
		
		unsigned short int bookPathIndex = fileCount - 1;
		AddWordsToMap(bookPathIndex, (path+file));					//add words to buffer
		if (fileCount % 2000 == 0) 		//every 2000 files we read, write map to files and clear the buffer
		{			   //until written to file
			WriteMapToFile();
			wordBuildMap.clear();
		}
	}
}

vector<string> Index::GetInstancesOf(string word)
{	
	vector<string> instancesOfWord;
	MakeLower(word);					//normalize word
	if(lastWordSearchedFor != word)		//don't rebuild the wordMap if it's the same word as last time
	{
		BuildWordMap(word);
		lastWordSearchedFor = word;
	}
	
	map<unsigned short int, vector<int> >::iterator it;
	string line;
	for(it = wordMap.begin(); it != wordMap.end(); it++)		//read through every book the word appears in
	{
		instancesOfWord.push_back(GetBookInfo(it->first));		//get info for book (title, author)
		ifstream bookfile;
		if(CarefulOpenIn(bookfile, bookPaths.at(it->first)))		//open each book file
		{
			for(int j = 0; j < it->second.size(); j++)		//and get each line in the book the word appears in
			{
				bookfile.seekg(it->second.at(j), bookfile.beg);					//and move to the offset
				getline(bookfile, line);
				instancesOfWord.push_back(line);				//add the line to instancesOfWord
			}
		}
	}
	
	return instancesOfWord;
}

vector<string> Index::GetInstancesOf(string word, int startingIndex, int numBooks)
{
	vector<string> instancesOfWord;
	MakeLower(word);					//normalize word
	if(lastWordSearchedFor != word)		//don't rebuild the wordMap if it's the same word as last time
	{
		BuildWordMap(word);
		lastWordSearchedFor = word;
	}
	
	map<unsigned short int, vector<int> >::iterator it;
	map<unsigned short int, vector<int> >::iterator endPos;
	string line;
	
	if(startingIndex >= wordMap.size())		//if startingIndex is out of map range, just return empty vector
	{
		return instancesOfWord;
	}
	
	it = wordMap.begin();		//set it to beginning of wordMap
	endPos = wordMap.begin();
	advance(endPos, (startingIndex + numBooks));		//move endPos numBooks beyond the startingIndex
	for(advance(it, startingIndex); (it != wordMap.end()) && (it != endPos); it++)		//read through numBooks number of books the word appears in,
	{																					//starting at startingIndex
		instancesOfWord.push_back(GetBookInfo(it->first));		//get info for book (title, author)
		ifstream bookfile;
		if(CarefulOpenIn(bookfile, bookPaths.at(it->first)))		//open each book file
		{
			for(int j = 0; j < it->second.size(); j++)		//and get each line in the book the word appears in
			{
				bookfile.seekg(it->second.at(j), bookfile.beg);					//and move to the offset
				getline(bookfile, line);
				instancesOfWord.push_back(line);				//add the line to instancesOfWord
			}
		}
	}
	
	return instancesOfWord;
}

void Index::AddWordsToMap(unsigned short int bookIndex, string bookPath)
{
	pair <unsigned short int, int> wordPair;
	ifstream bookFile;
	string line, word;
	if(CarefulOpenIn(bookFile, bookPath))
	{
		int pos = 0;
		while(!bookFile.fail())			//read through the whole file
		{
			getline(bookFile, line);			//get each line
			istringstream iss(line);
			while(iss >> word)				//read each word
			{
				MakeLower(word);			//normalize word
				if (!IsStopWord(word)) 	//don't store stop words
				{			
					wordPair = make_pair(bookIndex,pos);
					wordBuildMap[word].push_back(wordPair);		//store pair in pairIndex
				}
			}
			
			pos = bookFile.tellg();		//update pos to new filePosition
		}
		bookFile.close();
	}
}

void Index::WriteMapToFile()
{
	cout << "writing words to word file...." << endl;
	map<string, vector<pair<unsigned short int, int> > >::iterator it;
	
	for (it = wordBuildMap.begin(); it != wordBuildMap.end(); it++) //for every key in the map
	{
		for (int i = 0; i < it->second.size(); i++) 			//for every value in the vector of pairs
		{
			string fileName = wordsDir + (it->first) + wordsFileType;
			unsigned short int bookPathIndex = (it->second)[i].first; 		//first value from pair: book index in vector of book positions
			int pos = (it->second)[i].second;						//secont value from pair: position of word in file
			ofstream wordFileOut(fileName.c_str(), ios::out | ios::binary | ios::app); //file to store book locations and word locations
			wordFileOut.write((char*)&bookPathIndex, sizeof(unsigned short int));
			wordFileOut.write((char*)&pos, sizeof(int));
			wordFileOut.close();
		}
	}
}

bool Index::CarefulOpenIn(ifstream& infile, string name)
{
      infile.open(name.c_str(), ios::binary);			//was getting wrong positions without opening binary
      if(infile.fail())							//return false if there's a problem opening the file
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
	
	
	if (fullString.length() >= ending.length()) 				//if the full string has fewer characters than ending, 
	{															//full string doesn't contain the ending
		string fullStringEnding = fullString.substr(fullString.length() - ending.length(), ending.length());		//store the ending of fullString
		return (0 == fullStringEnding.compare (ending));			//compare ending of full string with ending
	} 
	else 
	{
		return false;
	}
}

void Index::BuildStopWords()
{
    string line, word; 
	string filename = "NewStop.txt";
	ifstream infile(filename); // open the file
    int position = 0; 
    while (!infile.fail()) 
	{
        getline(infile, line); // get each line in file
        istringstream lineStream(line); 
        while (lineStream >> word) 			//get each word in line
		{ 
            stopWords[word];		// push the word and the line position on the vector for this word
        }
        position = infile.tellg(); // get the poistion of the next line
    }
}

bool Index::IsStopWord(string word)
{
	map<string, short int>::iterator it;	
	it = stopWords.find(word);
	
    if (it == stopWords.end() || word == "") 		//check if word is found in stopWords
	{
         return false;
    } 
	else 
	{
        return true;
    }
}

void Index::BuildBookPathsMap()
{
	ifstream infile;
	if(CarefulOpenIn(infile, bookPathsFile))		//open bookPaths file
	{
		bookPaths.clear();				//get rid of any garbage in bookPaths vector
		string line;
		while(!infile.eof())			//read through bookPaths file
		{
			getline(infile, line);
			cout << line << endl;
			bookPaths.push_back(line);			//push each line to bookPaths vector
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

void Index::BuildWordMap(string word)
{	
	MakeLower(word);		//normalize word
	string newFileName = wordsDir+word+wordsFileType;			//get the word file name
	ifstream wordFile(newFileName.c_str(), ios::in | ios::binary);		//open it to read in binary
	
	if(wordFile.fail())		//if word doesn't exist, just return
	{
		cout << "\nNo file for word...\n";
		return;
	}
	
	wordMap.clear();			//clear out word map from last word searched for
	
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

void Index::BuildBookInfo()
{
	const int MAX_LINE_SEARCH = 30;			//number of lines to search for info at beginning of file
	const string infoIntro = "The Project Gutenberg EBook of ";			//string that precedes the book info
	ifstream infile;
	
	for(int i = 0; i < bookPaths.size(); i++)		//for each book
	{
		if(CarefulOpenIn(infile, bookPaths.at(i)))		//open the file
		{
			cout << "\nGetting info for " << bookPaths.at(i) << endl;
			string line;
			for(int j = 0; j < MAX_LINE_SEARCH; j++)		//check the first few lines for a title
			{
				getline(infile, line);
				size_t pos = line.find(infoIntro);
				if(pos != string::npos)					//check if the line contains the infoIntro: if it does, store it
				{
					pos += infoIntro.size();		//move pos past infoIntro
					bookInfo[i] = line.substr(pos);		//store the info in bookInfos
				}
			}
			infile.close();
		}
	}
}

string Index::GetBookInfo(int bookIndex)
{
	cout << "\nGetting book " << bookPaths.at(bookIndex) << endl;
	
	map<int, string>::iterator it;	
	
	it = bookInfo.find(bookIndex);
    if (it != bookInfo.end())					//if there is info for that book, send it back
	{
		string info = "<br><i><b>";
		info += bookInfo[bookIndex] + ": ";
		info += "</i></b>";
        return info;
    } 
	else 						//otherwise, return an empty string
	{
        return "";
    }
}