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

API_Frame::API_Frame()
{
    //ctor
}

API_Frame::~API_Frame()
{
    //dtor
}

API_Frame::API_Frame(const API_Frame& other)
{
    //copy ctor
}

API_Frame& API_Frame::operator=(const API_Frame& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

unsigned char API_Frame::calculateChecksum(string frame)
{
    unsigned char checksum;

    return checksum;
}

