/***************************************************************************
 * nameservercpp  -  Program to serve of last name statistics
 *
 * copyright : (C) 2009 by Jim Skon
 *
 * This program runs as a background server to a CGI program, providinging US Census
 * Data on the frequency of names in response to requestes.  The rational for this 
 * is to demonstrait how a background process can serve up information to a web program,
 * saving the web program from the work of intiallizing large data structures for every
 * call.
 * 
 * 
 *
 ***************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "fifo.h"


using namespace std;

/* Fifo names */
string receive_fifo = "Namerequest";
string send_fifo = "Namereply";

string filename = "/home/class/SoftDev/Shakespeare/Shakespeare.txt";

map<string, vector<int> >refs;
map<string, vector<int> >::iterator it;
ifstream shakespeare;

string line, word, type, name;


vector <int> indexSearch(string word);
//Precondition: a word is defined, uses the map defined above and an open file. 
//Postcondition: returns a vector of the line numbers where the word appears. 



/* Read the US Census data file "dist.all.last" and load it into an
 * in memory b+tree with name as the key.
 *
 * Return 1 if success, 0 if fail
 */
int createnamemap(map<string, vector<int> > &refs, string);
//precondition: file available, string and refs are defined. 
//postconditon: creates a hashmap connecting line numbers to word incidences. 

/* Server main line,create name MAP, wait for and serve requests */
int main() {

    string inMessage, outMessage;

    int pos;

    // Create the map of last name data
    if (createnamemap(refs, "/home/class/SoftDev/Shakespeare/Shakespeare.txt") != 1) {
        cout << "Error Loading Database" << endl;
        exit(0);
    }

    cout << "Name data loaded!" << endl;

    // create the FIFOs for communication
    Fifo recfifo(receive_fifo);
    Fifo sendfifo(send_fifo);

    map<string, string> *refs;
    while (1) {//on because it needs to stay open to always check. 

        /* Get a message from a client */
        /* Get a message from a client */
        recfifo.openread();
        inMessage = recfifo.recv();
        /* Parse the incoming message */
        /* Form:  $type*name  */

        name = inMessage;
        type = "WORD";
        pos = 0;
        cout << "Message: " << type << " : " << name << endl;
        //close receive before you close the send 
        //send and recieve are like parentheses-- always do send first recieve second. 

        vector<int> locations = indexSearch(name);
        //copies index into a local vector

        ifstream infile;
        infile.open("/home/class/SoftDev/Shakespeare/Shakespeare.txt");
        //reopens file

        sendfifo.openwrite();
        sendfifo.send("</p>"); //sends out paragraph symbol 

        for (int i = 0; i < locations.size(); i++) {
            //iterates through for each file. 
            cout << i;

            infile.seekg(0);

            int lineNum = locations[i];

            //gets line number

            infile.seekg(lineNum);
            //goes to line number


            getline(infile, outMessage);
            cout << outMessage << endl;

            sendfifo.send(outMessage);
            sendfifo.send("</p>"); //sends out paragraph symbol 

        }
        sendfifo.send("$END");
        sendfifo.fifoclose();
        recfifo.fifoclose();

    }
}

vector <int> indexSearch(string word) {
    map<string, vector<int> >::iterator it; // iterator for find                                                                
    vector<int> blank; // return for no matches                                                                                  
    /* find the word and get the vector of references */
    /* First use find, so as to NOT create a new entry */
    it = refs.find(word);
    if (it == refs.end()) {
        return (blank);
    } else {
        return (refs[word]);
    }
}

int createnamemap(map<string, vector<int> > &refs, string) {

    ifstream infile;
    infile.open("/home/class/SoftDev/Shakespeare/Shakespeare.txt"); // open the file
    int position = 0;
    while (!infile.fail()) {
        getline(infile, line); // get the next line of code
        istringstream lineStream(line); // Create a string stream of the line
        while (lineStream >> word) { // get the next word
            transform(word.begin(), word.end(), word.begin(), ::toupper);
            refs[word].push_back(position);
            // push the word and the line position on the vector for this word
        }
        position = infile.tellg(); // get the poistion of the next line
    }
    return (1);
}