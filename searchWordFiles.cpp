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
#include "fifo.h"

using namespace std; 


int main() 
{ 
	/* Fifo names */
	string receive_fifo = "GutenbergRequest";
	string send_fifo = "GutenbergReply";

	const string wordNotFoundMessage = "No matches found.";
	
	Index index;	
	
	// create the FIFOs for communication
    Fifo recfifo(receive_fifo);
    Fifo sendfifo(send_fifo);	
	
	while(1)		//main server loop
	{				
		recfifo.openread();
        string word = recfifo.recv();		//get word to search for from CGI
		
		cout << "Word: " << word << endl;		
		sendfifo.openwrite();		//initiates write lock 
		
		
		// const int BOOK_SEARCH_INTERVAL = 10000;
		// vector<string> lineInstances = index.GetInstancesOf(word, 0, BOOK_SEARCH_INTERVAL);
		// if(lineInstances.empty())
		// {
			// cout << "No Matches found for: "<<word<<endl;
			// sendfifo.send(wordNotFoundMessage);	
		// }
		// else
		// {
			// for(int i = BOOK_SEARCH_INTERVAL; !lineInstances.empty(); i+= BOOK_SEARCH_INTERVAL)
			// {
				// for(int j = 0; j < lineInstances.size(); j++)
				// {
					// cout << lineInstances[j] << endl;
					// sendfifo.send(lineInstances[j]);
				// }
				// lineInstances = index.GetInstancesOf(word, i, BOOK_SEARCH_INTERVAL);
			// }
		// }
		
		
		
		
		vector<string> lineInstances = index.GetInstancesOf(word);			//get the matching lines
		if(lineInstances.empty())				//if no matching line is returned, then the word does not exist in the index
		{
			cout << "No Matches found for: "<<word<<endl;
			sendfifo.send(wordNotFoundMessage);					
		}
		else
		{
			for(int i = 0; i < lineInstances.size(); i++)		//otherwise, send CGI every line
			{
				cout << lineInstances[i] << endl;
				sendfifo.send(lineInstances[i]); 
			}
		}

		cout << "$END\n"; 
		sendfifo.send("$END");				//let CGI know that loop is over
        sendfifo.fifoclose();
       	recfifo.fifoclose();			//close FIFOs
	}
}
    
