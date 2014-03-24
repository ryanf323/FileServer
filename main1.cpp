/*************************
* Edward Alvarez Mercedes
* Ryan Flynn
* ISTE-101
* Project 2
* File Client
**************************/

#include <iostream>
#include <string>
#include <cstdlib>
#include "Winsock2.h"
#include "Socket.h"

using namespace std;

bool checkName(string, string);
char* stringToCharArray(string);
bool checkCommand (string, Socket&);
void receiveDir(Socket&);
void receiveFile(Socket&);

int main()
{
    //Variables definitions:
    string ipAddress;
    int port = 54321;
    char recMessage[STRLEN];
    string userId;
    string command;

    cout << "Enter the Server's ip address: ";
    cin  >> ipAddress;

    ClientSocket sockClient;
    sockClient.ConnectToServer(ipAddress.c_str(), port);
    sockClient.RecvData( recMessage, STRLEN );

    if (checkName("LOGIN", recMessage)== false){
        sockClient.CloseConnection();
    }else{
        cout << "Please enter the user-identifier: ";
        cin  >> userId;
        sockClient.SendData(stringToCharArray(userId));
        cin.ignore();  //Removing the /n character for the next getline.
    }

    sockClient.RecvData( recMessage, STRLEN );

    if (checkName("WELCOME", recMessage)== false){
        sockClient.CloseConnection();
        return 0;
    }else{

        while(true){
            cout << "Please enter the command: ";
            getline(cin, command);
            sockClient.SendData(stringToCharArray(command));
            if (checkCommand(command, sockClient) == false) break;
        }

    }


    return 0;
}


/********************
* Name: checkName
* Purpose: Verify the server keywords "LOGIN" and "WELCOME".
* Arguments: required keyword, keyword send by the server.
* Returns: true if matched, false otherwise
********************/
bool checkName(string keyword, string message){

    if(keyword == message){
        return true;
    }else{
        return false;
    }
}

/********************
* Name: stringToCharArray
* Purpose: Converts a string in a character array to send
via the socket.
* Arguments: String to be converted
* Returns: New Cstring.
********************/

char* stringToCharArray(string oldStr){

    char *newCStr = new char[oldStr.size()+1];
    strcpy(newCStr, oldStr.c_str());

    return newCStr;
}

/********************
* Name: checkCommand
* Purpose: Verify the command sent to the server
to be prepare for the response.
* Arguments: Command send to the server. Socket to receive data.
* Returns: true to exit keep the client running.
False to finish the program.
********************/

bool checkCommand (string command, Socket& sockClient){

    const int STRLEN = 256;
    char recMessage[STRLEN];

    if (command == "LIST"){
        receiveDir(sockClient);
        return true;
    }

    else if (command.substr(0,4) == "SEND"){
        receiveFile(sockClient);
        return true;
    }

    else if (command.substr(0,4) == "QUIT"){
        sockClient.CloseConnection();
        cout << "Connection close by the server. Exiting the program" << endl;
        return false;
    }
    else {
        sockClient.RecvData( recMessage, STRLEN );
            if (checkName("ERROR", recMessage)== true) {
                cout << "Command not found" << endl;
                return true;
            }

    }

}

/********************
* Name: receiveDir
* Purpose: Receive the contents of the
server's current directory.
* Arguments: Socket to receive data.
* Returns: Nothing.
********************/
void receiveDir(Socket& sockClient){

    char recMessage[STRLEN];

    string output;

    sockClient.RecvData( recMessage, STRLEN );
    string format(recMessage);
    output=format.substr(3); //Eliminates character at the beginning of the string.
    cout << output;
}

/********************
* Name: receiveFile
* Purpose: Receive requested file
from the server.
* Arguments: Socket to receive data.
* Returns: Nothing.
********************/
void receiveFile(Socket& sockClient){

    char filename[256];
    char recMessage[256];
    sockClient.ReceiveFile(filename);
    sockClient.RecvData( recMessage, STRLEN );

    if (checkName("EOFEOFEOFEOFEOFEOF", recMessage)== false){
        sockClient.CloseConnection();
    }else{
        sockClient.SendData(stringToCharArray("EOF OK"));
        sockClient.RecvData( recMessage, STRLEN );
            if (checkName("OK", recMessage)== false) sockClient.CloseConnection();
    }
}
