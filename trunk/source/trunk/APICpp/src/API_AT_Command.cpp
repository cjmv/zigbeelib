///
/// Definition of API_AT_Command class.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "API_AT_Command.h"

using namespace std;

// Default Constructor
API_AT_Command::API_AT_Command(): API_Frame()
{
    //ctor
}

// Copy constructor
API_AT_Command::API_AT_Command(const API_AT_Command& other): API_Frame()
{
    //copy ctor
}

// Constructor
API_AT_Command::API_AT_Command(unsigned char frameId, std::string atCommand, std::string parameterValue)
: API_Frame(), frameId_(frameId), atCommand_(atCommand), parameterValue_(parameterValue)
{

}

// Constructor
API_AT_Command::API_AT_Command(unsigned int length, FrameType frameType, unsigned char frameId, std::string atCommand, std::string parameterValue)
: API_Frame(length, frameType), frameId_(frameId), atCommand_(atCommand), parameterValue_(parameterValue)
{

}

API_AT_Command::~API_AT_Command()
{
    //dtor
}

API_AT_Command& API_AT_Command::operator=(const API_AT_Command& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void API_AT_Command::parseFrame(string frame)
{
}
