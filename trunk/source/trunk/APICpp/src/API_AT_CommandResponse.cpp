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
    frameType_ = AT_COMMAND_RESPONSE;
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
    frameType_ = AT_COMMAND_RESPONSE;
}

// Full constructor
API_AT_CommandResponse::API_AT_CommandResponse(unsigned int length, FrameType frameType, unsigned char frameId, string atCommand, string parameterValue, CommandStatus commandStatus)
: API_AT_Command(length, frameType, frameId, atCommand, parameterValue), commandStatus_(commandStatus)
{
    // Implementation here
}

// default destructor
API_AT_CommandResponse::~API_AT_CommandResponse()
{
    //dtor
}

/*API_AT_CommandResponse& API_AT_CommandResponse::operator=(const API_AT_CommandResponse& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}*/

// get_ATBD_Value method
unsigned int API_AT_CommandResponse::get_ATBD_Value()
{
    unsigned char bandwith = 0x0;

    for(unsigned int i = 0; i < parameterValue_.length(); i++){

        if(i == 0)
            bandwith = ((unsigned char)parameterValue_[i] * 0x100) + (unsigned char)parameterValue_[i+1];

        else if(i != parameterValue_.length()-1)
            bandwith = (bandwith * 0x100) + (unsigned char)parameterValue_[i+1];
    }

    //cout << dec << API_AT_CommandResponse::BDParameterRange_[bandwith] << endl;

    return API_AT_CommandResponse::BDParameterRange[bandwith];
}

// parseFrame method
bool API_AT_CommandResponse::parseFrame(string frame)
{
    unsigned char MSB = (unsigned char)frame[1], LSB = (unsigned char)frame[2];

    length_ = (MSB * 0x100) + LSB;
    frameId_ = (unsigned char) frame[4];
    atCommand_ = frame.substr(5, 2);
    commandStatus_ = CommandStatus(frame[7]);

    parameterValue_ = frame.substr(8, length_ - 5);

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

string API_AT_CommandResponse::getFrame()
{
    // It should be verified the validity of each value

    checksum_ = frameType_ + frameId_ + (unsigned char)atCommand_[0] + (unsigned char)atCommand_[1] + (unsigned char)commandStatus_;


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
    + atCommand_
    + (char)commandStatus_
    + parameterValue_
    + (char)checksum_;
}
