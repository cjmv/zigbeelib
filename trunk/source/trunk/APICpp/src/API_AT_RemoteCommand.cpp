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
    frameType_ = AT_COMMAND;
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
    frameType_ = AT_COMMAND;
}

// Full constructor
API_AT_RemoteCommand::API_AT_RemoteCommand(unsigned int length, FrameType frameType, unsigned char frameId, string atCommand, string parameterValue,
                                           string destinationAddress, string destinationNetworkAddress, RemoteCommandOption remoteCommandOption)
: API_AT_Command(length, frameType, frameId, atCommand, parameterValue),
destinationAddress_(destinationAddress), destinationNetworkAddress_(destinationNetworkAddress), remoteCommandOption_(remoteCommandOption)
{
    // Implementation here
}

// default Destructor
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

// parseFrame method
bool API_AT_RemoteCommand::parseFrame(string frame)
{
    unsigned char MSB = (unsigned char)frame[1], LSB = (unsigned char)frame[2];

    length_ = (MSB * 0x100) + LSB;
    frameId_ = (unsigned char) frame[4];
    destinationAddress_ = frame.substr(5, 8);
    destinationNetworkAddress_ = frame[13] + frame[14];
    remoteCommandOption_ = RemoteCommandOption((unsigned char)frame[15]);
    atCommand_ =  frame[16] + frame[17];

    // 15 is the fixed value without the parameter value
    if(length_ > 15){

        parameterValue_ = frame.substr(18, length_ - 15 );
    }

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

string API_AT_RemoteCommand::getFrame()
{
    // It should be verified the validity of each value

    checksum_ =
        frameType_ +
        frameId_ +
        (unsigned char)atCommand_[0] +
        (unsigned char)atCommand_[1] +
        (unsigned char)destinationNetworkAddress_[0] +
        (unsigned char)destinationNetworkAddress_[1] +
        (unsigned char)remoteCommandOption_;

    for (unsigned int i = 0; i < destinationAddress_.length(); i++){
        checksum_ += (unsigned char)destinationAddress_[i];
    }

    for (unsigned int i = 0; i < parameterValue_.length(); i++){
        checksum_ += (unsigned char)parameterValue_[i];
    }

    // (checksum_ & 0xFF) to select the lowest 8 bits
    checksum_ = 0xFF - (checksum_ & 0xFF);

    return string("")
    + (char)startDelimiter_
    + (char)(((length_ & 0xFF00) >> 8) & 0xFF) // length (MSB)
    + (char)(length_ & 0xFF) // length (LSB)
    + (char)frameType_
    + (char)frameId_
    + destinationAddress_
    + destinationNetworkAddress_
    + (char)remoteCommandOption_
    + atCommand_
    + parameterValue_
    + (char)checksum_;
}
