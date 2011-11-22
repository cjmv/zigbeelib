///
/// Definition of API_AT_RemoteCommand class.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

 #include "API_AT_RemoteCommand.h"

 using namespace std;

// Default constructor
API_AT_RemoteCommand::API_AT_RemoteCommand(): API_AT_Command()
{
    //ctor
}

// Copy constructor
API_AT_RemoteCommand::API_AT_RemoteCommand(const API_AT_RemoteCommand& other)
{
    //copy ctor
}

// Specific constructor
API_AT_RemoteCommand::API_AT_RemoteCommand(string destinationAddress, string destinationNetworkAddress, RemoteCommandOption remoteCommandOption)
: API_AT_Command(), destinationAddress_(destinationAddress), destinationNetworkAddress_(destinationNetworkAddress), remoteCommandOption_(remoteCommandOption)
{
    // Implementation here.
}

API_AT_RemoteCommand::API_AT_RemoteCommand(unsigned int length, FrameType frameType, unsigned char frameId, string atCommand, string parameterValue,
                                           string destinationAddress, string destinationNetworkAddress, RemoteCommandOption remoteCommandOption)
: API_AT_Command(length, frameType, frameId, atCommand, parameterValue),
destinationAddress_(destinationAddress), destinationNetworkAddress_(destinationNetworkAddress), remoteCommandOption_(remoteCommandOption)
{
    // Implementation here
}

// Destructor
API_AT_RemoteCommand::~API_AT_RemoteCommand()
{
    //dtor
}

API_AT_RemoteCommand& API_AT_RemoteCommand::operator=(const API_AT_RemoteCommand& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void API_AT_RemoteCommand::parseFrame(string frame)
{
    // Implement parse frame.
}
