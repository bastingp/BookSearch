
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
	
	string rootDirectory = "/home/preston/Software_Development/project6/books_archive"; 
	Index index(rootDirectory);
	//index.Build();
	
	while(1){//this will keep running 
	string matchLine;
		string word;
		
		//gets the input from CGI
	//	recfifo.openread();
    //    	inMessage = recfifo.recv();
		cout << "Search: ";
		cin >> word;	
		//cout << "Word: " << endl; 
		int i = 0;
		do{
			matchLine = index.GetInstancesOf(word, i);
			if(i==0 && matchLine =="")
			{
				cout << "No Matches found for: "<<word<<endl;
			}
			else
			{
				cout << matchLine << endl;
				i++;
			}
		}while(matchLine != "");
		//initiates write lock 
		
		// sendfifo.openwrite();
        //	sendfifo.send("</p>");

		// for (int i=0; i < matchLines.size(); i++)
		 //{
		// 	cout << matchLines[i] << endl; 
		//	outMessage = matchLines[i]; 
		//	sendfifo.send(outMessage); 
		//	sendfifo.send("</p>");
			 //ships out lines to website. 
		 //}

		//closes up
		//cout << "$END"; 
		//sendfifo.send("$END");
        //	sendfifo.fifoclose();
       	//	recfifo.fifoclose();

	}

}
    