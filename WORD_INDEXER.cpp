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
	
	path = dirRoot;			//begin the path at the directory root
	BuildStopWords();
	ProcessDirectory("");		//process every file beginning at directory root
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
		string bookFileName = "books/book_file.txt";
		ofstream bookFile(bookFileName.c_str(), ios::out | ios::app); //opens file to output file paths to
		int booknum = bookFile.tellp();			//gets start location of path in file
		bookFile << (path + file);
		bookFile << endl;
		
		bookFile.close();

		if(CarefulOpenIn(infile, (path + file)))
		{
			int pos= 0;
			while(!infile.fail())			//read through the whole file
			{
				getline(infile, line);			//get each line
				istringstream iss(line);
				while(iss >> word)				//read each word
				{
					MakeLower(word);
					if (!IsStopWord(word)) //checks if it is a stop word
					{			//store word as lower				
						string newFileName = "words/"+word+".bin";
						ofstream wordFileOut(newFileName.c_str(), ios::out | ios::binary | ios::app); //file to store book locations and word locations
						wordFileOut.write((char*)&booknum, sizeof(int));
						wordFileOut.write((char*)&pos, sizeof(int));
						wordFileOut.close();
					}
				}
				
				pos = infile.tellg();
			}
			
			infile.close();
		}
	}
}

vector<string> Index::GetInstancesOf(string word)
{	
	MakeLower(word);
	string newFileName = "words/"+word+".bin";
	ifstream wordFile(newFileName.c_str(), ios::in | ios::binary);
	int i = 2;
	int value;
	int book;
	int position;
	string bookPath="";
	string line;
	vector<string> instancesOfWord;
	if (wordFile.is_open())
	{
		while (!wordFile.eof())
		{
			wordFile.read((char*)&value, sizeof(int));
			if (!wordFile.eof())
			{
				if (i % 2 == 0) //alternates how the values are handled
				{
					book = value;
				} 
				else
				{
					position = value;
					ifstream bookFile, bookPathIndex;
					bookPathIndex.open("books/book_file.txt", ios::in); //file where paths are stored
					bookPathIndex.seekg(book, bookPathIndex.beg);
					getline(bookPathIndex, bookPath);
					if(CarefulOpenIn(bookFile, bookPath))
					{
						bookFile.seekg(position, bookFile.beg);
						getline(bookFile, line);
						//get title
						instancesOfWord.push_back(line);
					}
				}
				i++;
			}
		}
		return instancesOfWord;
	}
	else //couldnt find word file aka there were no instances stored of that word
	{
		instancesOfWord.push_back("No matches found");
		return instancesOfWord;
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
	ifstream infile("stopwords.txt"); // open the file
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

