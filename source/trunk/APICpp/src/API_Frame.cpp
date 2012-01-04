///
/// Definition of API_Frame class.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

 #include "API_Frame.h"

 using namespace std;

// Default constructor.
API_Frame::API_Frame()
{
    startDelimiter_ = 0x7E;
    length_ = 0;
}

// Copy constructor
API_Frame::API_Frame(const API_Frame& other)
{
}

// Constructor
API_Frame::API_Frame(unsigned int length, FrameType frameType): length_(length), frameType_(frameType)
{
    startDelimiter_ = 0x7E;
}

// Constructor
API_Frame::API_Frame(unsigned int length, FrameType frameType, unsigned char checksum): length_(length), frameType_(frameType), checksum_(checksum)
{
    startDelimiter_ = 0x7E;
}

API_Frame::~API_Frame()
{
    //dtor
}

/*API_Frame& API_Frame::operator=(const API_Frame& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}*/

unsigned char API_Frame::calculateChecksum(string frame)
{
    unsigned char checksum;

    return checksum;
}

