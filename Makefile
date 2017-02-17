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

all: shakeserver testclient shakeajax PutCGI PutHTML
#all: shakeserver testclient 

testclient.o: testclient.cpp fifo.h
	$(CC) -c $(CFLAGS) testclient.cpp

shakeserver.o: shakeserver.cpp fifo.h
	$(CC) -c $(CFLAGS) shakeserver.cpp

shakeajax.o: shakeajax.cpp fifo.h
	$(CC) -c $(CFLAGS) shakeajax.cpp

testclient: testclient.o fifo.o
	$(CC) testclient.o fifo.o -o testclient

shakeserver: shakeserver.o fifo.o
	$(CC) shakeserver.o  fifo.o -o shakeserver

fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp
shakeajax: shakeajax.o  fifo.h
	$(CC) shakeajax.o  fifo.o -o shakeajax -L/usr/local/lib -lcgicc

PutCGI: shakeajax
	chmod 757 shakeajax
	cp shakeajax /usr/lib/cgi-bin/$(USER)_shakeajax.cgi 

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/

PutHTML:
	cp shake_ajax.html /var/www/html/class/softdev/$(USER)
	cp shake_ajax.css /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o shakespeare_readouts_ajax shakeserver testclient
