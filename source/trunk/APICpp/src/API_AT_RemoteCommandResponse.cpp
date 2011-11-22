///
/// Definition of API_AT_RemoteCommandResponse class.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "API_AT_RemoteCommandResponse.h"

using namespace std;

// Default constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(): API_AT_CommandResponse()
{
    //ctor
}

// Copy constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(const API_AT_RemoteCommandResponse& other)
{
    //copy ctor
}

// Specific constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(string destinationAddress, string destinationNetworkAddress)
: API_AT_CommandResponse(), destinationAddress_(destinationAddress), destinationNetworkAddress_(destinationNetworkAddress)
{
    // Implement here
}

// Full constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(unsigned int length,
                                                           FrameType frameType,
                                                           unsigned char frameId,
                                                           string atCommand,
                                                           string parameterValue,
                                                           CommandStatus commandStatus,
                                                           string destinationAddress,
                                                           string destinationNetworkAddress)
: API_AT_CommandResponse(length, frameType, frameId, atCommand, parameterValue, commandStatus),
destinationAddress_(destinationAddress), destinationNetworkAddress_(destinationNetworkAddress)
{
    // Implement here
}

// Default destructor
API_AT_RemoteCommandResponse::~API_AT_RemoteCommandResponse()
{
    //dtor
}

API_AT_RemoteCommandResponse& API_AT_RemoteCommandResponse::operator=(const API_AT_RemoteCommandResponse& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void API_AT_RemoteCommandResponse::parseFrame(string frame)
{
    // Implement method.
}
