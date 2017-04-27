#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <dirent.h>
#include <map>
#include <vector>

using namespace std;

vector<string> bookVector;

void ProcessDirectory(string directory);
void ProcessEntity(struct dirent* entity);
void ProcessFile(string file);
bool hasEnding (string const &fullString, string const &ending);
string getPath(int fileIndex);

string path = "/home/skon/books";
int fileCount = 0;



int main() 
{
  int fileSearch=0;
  
  char again = 'y';
	ProcessDirectory("");
	cout << "processed "<< fileCount<<" files"<< endl;
  while (again == 'y')
  {

    cout << "search for path to file: ";
    cin >> fileSearch;
    cout << endl;
    cout << getPath(fileSearch) << endl;
    cout << "again? (y/n): ";
    cin >> again;
    cout << endl;

  }

}


void ProcessDirectory(string directory)
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

void ProcessEntity(struct dirent* entity)
{
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an directory
     //don't process the  '..' and the '.' directories
      if(entity->d_name[0] == '.')
	{
	  return;
	}
      //it's an directory so process it
      ProcessDirectory(string(entity->d_name));
      return;
    }

  if(entity->d_type == DT_REG)
    {//regular file
      ProcessFile(string(entity->d_name));
      return;
    }
  cout << "Not a file or directory: " << entity->d_name << endl;
}


// Only look at it if it has a ".txt" extension
void ProcessFile(string file)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) 
  {  
    cout <<"file: "<<path<<file<< endl;
    bookVector.push_back(path+file);
    fileCount++;
	}

}

bool hasEnding (string const &fullString, string const &ending) 
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


string getPath(int fileIndex)
{
  if (fileIndex >= bookVector.size())
  {
    return("File path not found");
  } 
  else 
  {
    return(bookVector[fileIndex]);
  }
}