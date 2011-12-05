///
/// Definition of API_Receive_Packet class.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "API_Receive_Packet.h"

using namespace std;

// Default constructor
API_Receive_Packet::API_Receive_Packet(): API_Frame()
{
    //ctor
    frameType_ = RX_PACKET;
}

// Copy constructor
API_Receive_Packet::API_Receive_Packet(const API_Receive_Packet& other)
{
    //copy ctor
}

// Specific constructor
API_Receive_Packet::API_Receive_Packet(string sourceAddress, string sourceNetworkAddress, ReceiveOptions receiveOptions, string receivedData)
: API_Frame(), sourceAddress_(sourceAddress), sourceNetworkAddress_(sourceNetworkAddress), receiveOptions_(receiveOptions), receivedData_(receivedData)
{
    // Implement here
    frameType_ = RX_PACKET;
}

// Full constructor
API_Receive_Packet::API_Receive_Packet(unsigned int length, FrameType frameType, unsigned char checksum,
                                       string sourceAddress, string sourceNetworkAddress, ReceiveOptions receiveOptions, string receivedData)
: API_Frame(length, frameType, checksum), sourceAddress_(sourceAddress), sourceNetworkAddress_(sourceNetworkAddress), receiveOptions_(receiveOptions), receivedData_(receivedData)
{
    // Implement here
}

// Default destructor
API_Receive_Packet::~API_Receive_Packet()
{
    //dtor
}

API_Receive_Packet& API_Receive_Packet::operator=(const API_Receive_Packet& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

bool API_Receive_Packet::parseFrame(string frame)
{
    // Specific implementation here.
    unsigned char MSB = (unsigned char)frame[1], LSB = (unsigned char)frame[2];

    length_ = (MSB * 0x100) + LSB;
    sourceAddress_ = frame.substr(4, 8);
    sourceNetworkAddress_ = frame[12] + frame[13];
    receiveOptions_ = ReceiveOptions((unsigned char)frame[14]);

    // 12 is the fixed length without received data
    receivedData_ =  frame.substr(15, length_ - 12);

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

string API_Receive_Packet::getFrame()
{
    // It should be verified the validity of each value

    checksum_ = frameType_;

    for(unsigned int i = 0; i < sourceAddress_.length(); i++){
        checksum_ += (unsigned char)sourceAddress_[i];
    }

    checksum_ += (unsigned char)sourceNetworkAddress_[0] + (unsigned char)sourceNetworkAddress_[1] + receiveOptions_;

    for (unsigned int i = 0; i < receivedData_.length(); i++){
        checksum_ += (unsigned char)receivedData_[i];
    }

    // (checksum_ & 0xFF) to select the lowest 8 bits
    checksum_ = 0xFF - (checksum_ & 0xFF);

    return string("")
    + (char)startDelimiter_
    + (char)(((length_ & 0xFF00) >> 8) & 0xFF) // length MSB
    + (char)(length_ & 0xFF) // length LSB
    + (char)frameType_
    + sourceAddress_
    + sourceNetworkAddress_
    + (char)receiveOptions_
    + receivedData_
    + (char)checksum_;
}
