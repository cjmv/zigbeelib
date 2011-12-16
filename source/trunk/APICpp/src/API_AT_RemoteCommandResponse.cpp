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
    frameType_ = AT_COMMAND_RESPONSE;
}

// Copy constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(const API_AT_RemoteCommandResponse& other)
{
    //copy ctor
}

// Specific constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(string sourceAddress, string sourceNetworkAddress)
: API_AT_CommandResponse(), sourceAddress_(sourceAddress), sourceNetworkAddress_(sourceNetworkAddress)
{
    // Implement here
    frameType_ = AT_COMMAND_RESPONSE;
}

// Full constructor
API_AT_RemoteCommandResponse::API_AT_RemoteCommandResponse(unsigned int length,
                                                           FrameType frameType,
                                                           unsigned char frameId,
                                                           string atCommand,
                                                           string parameterValue,
                                                           CommandStatus commandStatus,
                                                           string sourceAddress,
                                                           string sourceNetworkAddress)
: API_AT_CommandResponse(length, frameType, frameId, atCommand, parameterValue, commandStatus),
sourceAddress_(sourceAddress), sourceNetworkAddress_(sourceNetworkAddress)
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

// parseFrame method
bool API_AT_RemoteCommandResponse::parseFrame(string frame)
{
    // Implement method.
    unsigned char MSB = (unsigned char)frame[1], LSB = (unsigned char)frame[2];

    length_ = (MSB * 0x100) + LSB;
    frameId_ = (unsigned char) frame[4];
    sourceAddress_ = frame.substr(5, 8);
    sourceNetworkAddress_ = frame.substr(13, 2); //frame[13] + frame[14];
    atCommand_ =  frame.substr(15, 2); //frame[15] + frame[16];
    commandStatus_ = CommandStatus(frame[17]);

    // 15 is fixed length without parameter value
    if(length_ > 15)
        parameterValue_ = frame.substr(18, length_ - 15);

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

string API_AT_RemoteCommandResponse::getFrame()
{
    // It should be verified the validity of each value

    checksum_ =
        frameType_ +
        frameId_ +
        (unsigned char)sourceNetworkAddress_[0] +
        (unsigned char)sourceNetworkAddress_[1] +
        (unsigned char)atCommand_[0] +
        (unsigned char)atCommand_[1] +
        (unsigned char)commandStatus_;

    for(unsigned int i = 0; i < sourceAddress_.length(); i++){
        checksum_ += (unsigned char)sourceAddress_[i];
    }

    for (unsigned int i = 0; i < parameterValue_.length(); i++){
        checksum_ += (unsigned char)parameterValue_[i];
    }

    // (checksum_ & 0xFF) to select the lowest 8 bits
    checksum_ = 0xFF - (checksum_ & 0xFF);

    return string("")
    + (char)startDelimiter_
    + (char)(((length_ & 0xFF00) >> 8) & 0xFF) // length MSB
    + (char)(length_ & 0xFF) // length LSB
    + (char)frameType_
    + (char)frameId_
    + sourceAddress_
    + sourceNetworkAddress_
    + atCommand_
    + (char)commandStatus_
    + parameterValue_
    + (char)checksum_;
}
