
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
//#inlcude "fifo.h"

using namespace std;

/* Fifo names */
//string receive_fifo = "GutenbergRequest";
//string send_fifo = "GutenbergReply";


int main() 
{ 
	string inMessage, outMessage;
	
	string rootDirectory = "/home/skon/books"; 
	Index index(rootDirectory);
	index.Build();
}
    
