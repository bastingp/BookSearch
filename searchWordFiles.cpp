
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

/* Fifo names */
string receive_fifo = "GutenbergRequest";
string send_fifo = "GutenbergReply";

int i; 

int main() 
{ 
	// create the FIFOs for communication
    Fifo recfifo(receive_fifo);
    Fifo sendfifo(send_fifo);
	
	string inMessage, outMessage;
	
	
	while(1){//this will keep running 
	string matchLine;
	string word;
		
		//gets the input from CGI
		recfifo.openread();
        inMessage = recfifo.recv();
		
		cout << "Word: " << endl; 
		
		
		//initiates write lock 
		
		sendfifo.openwrite();
        sendfifo.send("</p>");
        i = 0; 
		do{
			matchLine = index.GetInstancesOf(word, i);
			if(i==0 && matchLine =="")
			{
				cout << "No Matches found for: "<<word<<endl;
			}
			else
			{
				cout << matchLine << endl;
				outMessage = matchLine; 
				sendfifo.send(outMessage); 
				sendfifo.send("</p>");
				 //ships out lines to website. 
				i++;
			}
		}while(matchLine != "");
	

		

		//closes up
		cout << "$END"; 
		sendfifo.send("$END");
        sendfifo.fifoclose();
       	recfifo.fifoclose();

	}

}
    
