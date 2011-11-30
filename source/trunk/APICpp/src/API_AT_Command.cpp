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
    frameType_ = AT_COMMAND;
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
    frameType_ = AT_COMMAND;
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

bool API_AT_Command::parseFrame(string frame)
{
    unsigned char MSB = (unsigned char)frame[1], LSB = (unsigned char)frame[2];

    length_ = (MSB * 0x100) + LSB;
    frameId_ = (unsigned char) frame[4];
    atCommand_ =  frame[5] + frame[6];
    parameterValue_ = frame.substr(7, length_ - 4)

    // Check checksum validity
    checksum_ = 0;
    for (unsigned int i = 3; i < frame.length(); i++){
        checksum_ += (unsigned char) frame[i];
    }

    // If valid, the result must be 0xFF. Exception treatment might be a good idea.
    if (checksum_ == 0xFF)
        return true;
    else
        return false;

}
