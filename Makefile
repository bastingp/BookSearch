#MakeFile to build and deploy the Sample US CENSUS Name Data using ajax
# For CSC3004 Software Development

# Put your user name below:
USER= twitchelln

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

all: searchWordFiles testclient booksearchAjax WORD_INDEXER PutCGI PutHTML
#all: shakeserver testclient 

testclient.o: testclient.cpp fifo.h
	$(CC) -c $(CFLAGS) testclient.cpp

searchWordFiles.o: searchWordFiles.cpp fifo.h WORD_INDEXER_H.h
	$(CC) -c $(CFLAGS) searchWordFiles.cpp
	
WORD_INDEXER.o: WORD_INDEXER_H.h
	$(CC) -c $(CFLAGS) WORD_INDEXER.cpp
	
booksearchAJAX.o: booksearchAjax.cpp fifo.h
	$(CC) -c $(CFLAGS) booksearchAjax.cpp

testclient: testclient.o fifo.o
	$(CC) testclient.o fifo.o -o testclient

searchWordFiles: searchWordFiles.o fifo.o
	$(CC) searchWordFiles.o  fifo.o -o searchWordFiles
	
WORD_INDEXER: WORD_INDEXER.o
	$(CC) WORD_INDEXER.o -o WORD_INDEXER
	
fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp
		
booksearchAjax: booksearchAjax.o  fifo.h
	$(CC) booksearchAjax.o  fifo.o -o bookSearchAjax -L/usr/local/lib -lcgicc

PutCGI: booksearchAjax
	chmod 757 booksearchajax
	cp shakeajax /usr/lib/cgi-bin/$(USER)_booksearchAjax.cgi 

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp book_search.html /var/www/html/class/softdev/$(USER)
	cp book_search.css /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o bookSearch_readouts_ajax searchWordFiles testclient
