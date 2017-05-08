#MakeFile to build and deploy the Sample US CENSUS Name Data using ajax
# For CSC3004 Software Development

# Put your user name below:
USER= twitchelln

CC= g++ -std=c++11

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

all: searchWordFiles searchbooksAjax PutCGI PutHTML


searchWordFiles.o: searchWordFiles.cpp fifo.h WORD_INDEXER_H.h
	$(CC) -c $(CFLAGS) searchWordFiles.cpp
	
WORD_INDEXER.o: WORD_INDEXER.cpp WORD_INDEXER_H.h
	$(CC) -c $(CFLAGS) WORD_INDEXER.cpp
	
searchbooksAJAX.o: searchbooksAjax.cpp fifo.h
	$(CC) -c $(CFLAGS) searchbooksAjax.cpp


searchWordFiles: searchWordFiles.o fifo.o WORD_INDEXER.o 
	$(CC) searchWordFiles.o WORD_INDEXER.o fifo.o -o searchWordFiles
	
fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp
		
searchbooksAjax: searchbooksAjax.o  fifo.h
	$(CC) searchbooksAjax.o  fifo.o -o searchbooksAjax -L/usr/local/lib -lcgicc

PutCGI: searchbooksAjax
	chmod 757 searchbooksAjax
	cp searchbooksAjax /usr/lib/cgi-bin/$(USER)_searchbooksAjax.cgi 

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp book_search.html /var/www/html/class/softdev/$(USER)
	cp book_search.css /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o bookSearch_readouts_ajax searchWordFiles 
