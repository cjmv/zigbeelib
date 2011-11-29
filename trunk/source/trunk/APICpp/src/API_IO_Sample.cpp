///
/// Definition of API_IO_Sample class.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "API_IO_Sample.h"

using namespace std;

// Default constructor
API_IO_Sample::API_IO_Sample(): API_Receive_Packet()
{
    //ctor
}

// Copy constructor
API_IO_Sample::API_IO_Sample(const API_IO_Sample& other)
{
    //copy ctor
}

// Specific constructor
// I should consider removing this constructor since the detailed information here initialise will most probably be always parsed from the API_Receive_Packet::
API_IO_Sample::API_IO_Sample(unsigned short numberOfSamples, string digitalChannelMask, unsigned char analogChannelMask, string digitalSamples, string analogSamples)
: API_Receive_Packet(), numberOfSamples_(numberOfSamples), digitalChannelMask_(digitalChannelMask), analogChannelMask_(analogChannelMask), digitalSamples_(digitalSamples), analogSamples_(analogSamples)
{
    // Implement here
}

// Full constructor
API_IO_Sample::API_IO_Sample(unsigned int length, FrameType frameType, unsigned char checksum,
                             string sourceAddress, string sourceNetworkAddress, ReceiveOptions receiveOptions, string receivedMessage)
: API_Receive_Packet(length, frameType, checksum, sourceAddress, sourceNetworkAddress, receiveOptions, receivedMessage)
{
    // Implement here
    // I should consider parsing the receivedMessage here and initialize all specific structure of the IO Sample Frame.
}

// default destructor
API_IO_Sample::~API_IO_Sample()
{
    //dtor
}

API_IO_Sample& API_IO_Sample::operator=(const API_IO_Sample& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void API_IO_Sample::parseFrame(string frame)
{
    // Implementation here
}
