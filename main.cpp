/*************************
* Edward Alvarez Mercedes
* Ryan Flynn
* ISTE-101
* Project 2
* File Server
**************************/
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "Winsock2.h"
#include <dirent.h>
#include "Socket.h"

using namespace std;

void getFolderContents(Socket&);
bool checkName(string, string);
void getFile(char*, Socket&);
void checkMsg(char*, Socket&);
char* stringToCharArray(string);

int main(int argc, char * argv[])
{

    int port = 54321;
    string ipAddress;
    string id = "user";
    bool done = false;
    char recMessage[STRLEN];

        //Server
        ServerSocket sockServer;
        cout<<"Server started..."<<endl;
        sockServer.StartHosting( port );

        while ( true )
        {
        cout << "Prompting for login..." <<endl;
        sockServer.SendData(stringToCharArray("LOGIN"));
        sockServer.RecvData( recMessage, STRLEN );

            if(checkName(recMessage, id) == false){
                cerr << "Invalid User ID!" << endl;
                sockServer.SendData(stringToCharArray("UNWELCOME"));
                sockServer.CloseConnection();
                sockServer.Listen();

            }else{
                sockServer.SendData(stringToCharArray("WELCOME"));

                while(!done){
                    sockServer.RecvData( recMessage, STRLEN );
                    cout<<"Received: > "<<recMessage<<endl;

                        if ( strcmp( recMessage, "QUIT")==0 ){
                            sockServer.CloseConnection();
                            sockServer.Listen();
                            done = true;
                        }else{
                            checkMsg(recMessage, sockServer);
                        }
                }

            }

        }

    return 0;
}


/********************
* Name: checkName
* Purpose: Verify user login
* Arguments: required name, user name
* Returns: true if matched, false otherwise
********************/
bool checkName(string name, string user){

    if(name == user){
        return true;
    }else{
        return false;
    }
}

/********************
* Name: checkMsg
* Purpose: Parse message
*    and execute operations
* Arguments: message and socket ptr
* Returns: nothing
********************/
void checkMsg(char* recMessage, Socket& sockServer){

 if ( strncmp( recMessage, "LIST", 4 )==0){

            getFolderContents(sockServer);

        }
    else if ( strncmp( recMessage, "SEND", 4 )==0){

            getFile(recMessage, sockServer);

        }
    else if ( strncmp( recMessage, "EOF OK", 6 )==0){

        cout << "File Transmitted Successfully." << endl;
        sockServer.SendData(stringToCharArray("OK"));

        }
    else{

        sockServer.SendData(stringToCharArray("ERROR"));

    }

}
/********************
* Name: getFolderContents
* Purpose: reads file contents
*     of program directory and sends
*     a list of file to the client
* Arguments: socket
* Returns: nothing
********************/
void getFolderContents(Socket& sockServer){
        char dirList [STRLEN];
        char num [STRLEN];
        char newline [2];
        char space [2];
        newline[0]='\n';
        newline[1]='\0';
        space[0]='\t';
        space[1]='\0';

        //strcat(dirList, newline);
        DIR *pdir = NULL; // remember, it's good practice to initialize a pointer to NULL!
	    pdir = opendir ("."); // "." will refer to the current directory
	    struct dirent *pent = NULL;

	    if (pdir == NULL) // if pdir wasn't initialized correctly
	    { // print an error message and exit the program
	        cerr << "Error: Directory could not be initialized correctly." <<endl;
	        exit (3);
	    } // end if

        int i = 1;
	    while (pent = readdir (pdir)) // while there is still something in the directory to list
        {
	        if (pent == NULL) // if pent has not been initialized correctly
	        { // print an error message, and exit the program
	            cerr << "Error: Directory could not be initialized correctly." <<endl;
                exit (4);
	        }

	        //Build String with Folder Contents
	        strcat(dirList, itoa(i,num,10));
	        strcat(dirList, space);
	        strcat(dirList,pent->d_name);
	        strcat(dirList, newline);

	        i++;
	    }
        sockServer.SendData(dirList);
	    // finally, let's close the directory
	    closedir (pdir);

	     for (int i=0; i<STRLEN; i++)
            dirList[i]=NULL;
}
/********************
* Name: getFile
* Purpose: fetches file name
*  and passes it to the send
*  method
* Arguments: filename, socket
* Returns: nothing
********************/
void getFile(char *message, Socket& sockServer){

        //parse SEND message for file number
       string toParse(message);
       string fileName;
       vector<string> dirContents;
       int space = toParse.find(' ');
       int fileNum;

       if (space == -1){
            cerr << "Invalid File Request" << endl;
       }else{
           string number = toParse.substr((space+1), toParse.length()-space+1);
            fileNum = atoi(number.c_str());
            cout << "File Number "<< fileNum << "Requested." << endl;
       }

        DIR *pdir = NULL;
	    pdir = opendir ("."); // "." current directory
	    struct dirent *pent = NULL;

	    if (pdir == NULL)
	    {
	        cerr << "Error: Directory could not be initialized correctly." <<endl;
	        exit (3);
	    }

        while (pent = readdir(pdir)) // while there is still something in the directory to list
        {
	        if (pent == NULL)
            {
	            cerr << "Error: Directory could not be initialized correctly." <<endl;
                exit (4);
	        }

	        dirContents.push_back(pent->d_name);

	    }
	    closedir (pdir);

	    //Check for number in Range!
	    if(fileNum < 0 || fileNum > dirContents.size() -1){

          sockServer.SendData(stringToCharArray("ERROR"));

	    }else{

            fileName = dirContents[fileNum-1];

            //send file

            sockServer.SendFile(stringToCharArray(fileName));
            sockServer.SendData(stringToCharArray("EOFEOFEOFEOFEOFEOF"));
	    }
}

char* stringToCharArray(string oldStr){

    char *newCStr = new char[oldStr.size()+1];
    strcpy(newCStr, oldStr.c_str());

    return newCStr;
}
