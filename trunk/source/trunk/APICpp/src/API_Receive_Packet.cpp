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

void API_Receive_Packet::parseFrame(string frame)
{
    // Specific implementation here.
}
