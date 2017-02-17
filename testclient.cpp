#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "fifo.h"

//this is a test client program so that the server and fifo can be tested before CGI and HTML are used
using namespace std;
string receive_fifo = "Namereply";
string send_fifo = "Namerequest";

int main() {
    string message, word;
    string reply;

    while (1) {
        cout << "Enter a last name:";
        cin >> word;
        //transform(name.begin(), name.end(), name.begin(), ::toupper);


        // create the FIFOs for communication
        Fifo recfifo(receive_fifo);
        Fifo sendfifo(send_fifo);

        message = word;
        cout << "Send:" << message << endl;
        sendfifo.openwrite();
        sendfifo.send(message);

        recfifo.openread();

        do {
            /* Get a message from a server */
            reply = recfifo.recv();

            cout << "Server sent: " << reply << endl;

        } while (reply.find("$END") == string::npos);

        recfifo.fifoclose();
        sendfifo.fifoclose();

    }
}

