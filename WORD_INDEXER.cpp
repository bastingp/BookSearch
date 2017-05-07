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
	
	bookIDRef.clear();
	path = dirRoot;			//begin the path at the directory root
	BuildStopWords();
	ProcessDirectory("");		//process every file beginning at directory root
	WriteMapToFile();		//ensures remaining map values ge written to file
	pairIndex.clear();
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
		int booknum = bookFile.tellp();			//gets start location of path in file
		bookIDRef.push_back(booknum);
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

string Index::GetInstancesOf(string word, int nextWordNum)
{	
	MakeLower(word);
	string newFileName = wordsDir+word+wordsFileType;
	ifstream wordFile(newFileName.c_str(), ios::in | ios::binary);
	int i = 0;
	int j = 0;
	int value;
	unsigned short int book;
	int position;
	string bookPath="";
	string line;
	string instanceOfWord="";
	if (wordFile.is_open())
	{
		while (!wordFile.eof())
		{
			if (i % 2 == 0) //alternates how the values are handled
			{
				wordFile.read((char*)&value, sizeof(unsigned short int));
				if (!wordFile.eof())
				{
					book = value;
				}
			} 
			else
			{
				wordFile.read((char*)&value, sizeof(int));
				if (!wordFile.eof())
				{
					position = value;
					ifstream bookFile, bookPathIndex;
					bookPathIndex.open(bookDir, ios::in); //file where paths are stored
					bookPathIndex.seekg(bookIDRef[book], bookPathIndex.beg);
					getline(bookPathIndex, bookPath);
					if(CarefulOpenIn(bookFile, bookPath) && j == nextWordNum)
					{
						bookFile.seekg(position, bookFile.beg);
						getline(bookFile, line);
						//get title
						//string bookFileName = bookPath.erase(0,bookPath.length()-12);
						//instancesOfWord.push_back(bookFileName+":");
						instanceOfWord = line;
					}
					j++;
				}
			}
			i++;
		}
		return instanceOfWord;
	}
	else //couldnt find word file aka there were no instances stored of that word
	{
		instanceOfWord = "";
		return instanceOfWord;
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

