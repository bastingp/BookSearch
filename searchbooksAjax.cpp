#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

//Stuff for pipes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "fifo.h"

using namespace std;
using namespace cgicc; // Needed for AJAX functions.

// fifo for communication
string receive_fifo = "GutenbergReply";
string send_fifo = "GutenbergRequest";

int main() 
{

    Cgicc cgi; // Ajax object
    char *cstr;
    // Create AJAX objects to recieve information from web page.
    form_iterator word = cgi.getElement("word");

    // create the FIFOs for communication
    Fifo recfifo(receive_fifo);
    Fifo sendfifo(send_fifo);

    cout << "Content-Type: text/plain\n\n";

    string searchWord = **word;					//dereference pointer to get word from javascript

    sendfifo.openwrite();
    sendfifo.send(searchWord);			//send the word to the server

    recfifo.openread();
    
	string reply;
    do 					//get messages from server
	{
        reply = recfifo.recv();
		if(reply.find("$END") == string::npos)			//stops when it finds the end message
		{
			cout << "</p><br><br>" << reply;				//and send it back to the website
		}
		else
		{
			//then do not send the message
		}
    } while (reply.find("$END") == string::npos);
	
    recfifo.fifoclose();
    sendfifo.fifoclose();			//close FIFOs
	
    return 0;
}
