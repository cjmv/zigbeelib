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

const unsigned int API_AT_Command::BDParameterRange_[8] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

// Default Constructor
API_AT_Command::API_AT_Command(): API_Frame()
{
    frameType_ = AT_COMMAND;
    atCommand_ = "";
    parameterValue_ = "";
    frameId_ = 0x0;
    //ctor
}

// Copy constructor
API_AT_Command::API_AT_Command(const API_AT_Command& other): API_Frame()
{
    //copy ctor
}

API_AT_Command::API_AT_Command(unsigned char frameId, ATCommands atCommand, string parameterValue)
: API_Frame(), frameId_(frameId), parameterValue_(parameterValue)
{
    frameType_ = AT_COMMAND;

    // Have to convert the atCommand to string.
}

// Constructor
API_AT_Command::API_AT_Command(unsigned char frameId, std::string atCommand, string parameterValue)
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
    atCommand_ =  frame.substr(5,2);

    // 4 is the fixed length without parameter
    if(length_ > 4)
        parameterValue_ = frame.substr(7, length_ - 4);

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

// getFrame method
string API_AT_Command::getFrame()
{
    // It should be verified the validity of each value

    checksum_ = frameType_ + frameId_ + (unsigned char)atCommand_[0] + (unsigned char)atCommand_[1];

    length_ = 4;
    for (unsigned int i = 0; i < parameterValue_.length(); i++){
        checksum_ += (unsigned char)parameterValue_[i];
        length_++;
    }

    // (checksum_ & 0xFF) to select the lowest 8 bits
    checksum_ = 0xFF - (checksum_ & 0xFF);

    return string("")
    + (char)startDelimiter_
    + (char)(((length_ & 0xFF00) >> 8) & 0xFF) // length MSB
    + (char)(length_ & 0xFF) // length LSB
    + (char)frameType_
    + (char)frameId_
    + atCommand_
    + parameterValue_
    + (char)checksum_;
}
