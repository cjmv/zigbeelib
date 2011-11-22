///
/// Definition of API_AT_CommandResponse class.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

 #include "API_AT_CommandResponse.h"

 using namespace std;

// Default constructor
API_AT_CommandResponse::API_AT_CommandResponse(): API_AT_Command()
{
    //ctor
}

// Copy constructor
API_AT_CommandResponse::API_AT_CommandResponse(const API_AT_CommandResponse& other)
{
    //copy ctor
}

// Specific constructor
API_AT_CommandResponse::API_AT_CommandResponse(CommandStatus commandStatus)
: API_AT_Command(), commandStatus_(commandStatus)
{
    // Implementation here
}

// Full constructor
API_AT_CommandResponse::API_AT_CommandResponse(unsigned int length, FrameType frameType, unsigned char frameId, string atCommand, string parameterValue, CommandStatus commandStatus)
: API_AT_Command(length, frameType, frameId, atCommand, parameterValue), commandStatus_(commandStatus)
{
    // Implementation here
}

API_AT_CommandResponse::~API_AT_CommandResponse()
{
    //dtor
}

API_AT_CommandResponse& API_AT_CommandResponse::operator=(const API_AT_CommandResponse& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void API_AT_CommandResponse::parseFrame(string frame)
{
}
