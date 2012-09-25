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

    frameType_ = IO_RX_SAMPLE;
}

// Copy constructor
API_IO_Sample::API_IO_Sample(const API_IO_Sample& other)
{
    //copy ctor
}

// Specific constructor
// I should consider removing this constructor since the detailed information here initialise will most probably be always parsed from the API_Receive_Packet::
API_IO_Sample::API_IO_Sample(unsigned short numberOfSamples, unsigned int digitalChannelMask, unsigned short analogChannelMask, vector<unsigned short> digitalSamples, vector< pair <unsigned short, string> > analogSamples)
: API_Receive_Packet(), numberOfSamples_(numberOfSamples), digitalChannelMask_(digitalChannelMask), analogChannelMask_(analogChannelMask), digitalSamples_(digitalSamples), analogSamples_(analogSamples)
{
    // Implement here
    frameType_ = IO_RX_SAMPLE;
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

/*API_IO_Sample& API_IO_Sample::operator=(const API_IO_Sample& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}*/

// parseFrame method
bool API_IO_Sample::parseFrame(string frame)
{
    // Implementation here
    unsigned int digitalSamples = 0;
    unsigned int masked = 0;
    unsigned char MSB = (unsigned char)frame[1], LSB = (unsigned char)frame[2];

    length_ = (MSB * 0x100) + LSB;
    sourceAddress_ = frame.substr(4, 8);
    sourceNetworkAddress_ = frame.substr(12, 2);//[12] + frame[13];
    receiveOptions_ = ReceiveOptions((unsigned char)frame[14]);
    numberOfSamples_ = (unsigned short)(unsigned  char)frame[15];

    if(numberOfSamples_ > 1)
        return false;

    // 13 is the fixed length without received data
    receivedData_ =  frame.substr(16, length_ - 13);

    digitalChannelMask_ = (unsigned char)frame[16] + (unsigned char)frame[17];
    analogChannelMask_ = (unsigned char)frame[18];

    // If the sample set includes any digital I/O lines (digital channel mask > 0)
    // these two bytes contain samples for all enabled digital I/O lines.
    if((unsigned int)digitalChannelMask_ > 0)
        digitalSamples = (unsigned char)frame[19] + (unsigned char)frame[20];

    // By doing a bitwise AND between the channel mask and the digital Samples, we get the actual pins that are set high.
    // In this particular case the AND works much like a intersection between two groups, i.e,
    // only the bits position that are both set to 1 are high.
    masked = digitalChannelMask_ & digitalSamples;
    // If the bitwise AND result in a value larger than 0, than it means that at least one of the pins
    // configured to received digital samples is reading high.
    if (masked > 0){

        unsigned short shift = 0;

        // Browse the the 16 bits mask for the all the pins reading high.
        while(shift < 16){

            // For each one reading high save its position in the digital samples vector.
            if((masked & (1 << shift)) != 0)
                digitalSamples_.push_back(shift);

            shift++;
        }
        //digitalSamples_ = (unsigned char)frame[19] + (unsigned char)frame[20];
    }

    // Doing the same thing for the analogMasks.
    masked = analogChannelMask_;
    // If the bitwise AND result in a value larger than 0, than it means that at least one of the pins
    // configured to received analog samples is getting them.
    if (masked > 0){

        unsigned short numberOfSensors = 0, shift = 0;

        // For each of the pins receiving analog samples, save the pin position actually receiving it
        // and the corresping sample.
        while(shift < 8){

            if((masked & (1 << shift)) != 0){

                if(digitalSamples_.size() > 0)
                    analogSamples_.push_back(make_pair(shift, frame.substr(21+(numberOfSensors*2), 2)));
                else
                    analogSamples_.push_back(make_pair(shift, frame.substr(19+(numberOfSensors*2), 2)));

                numberOfSensors++;
            }

            shift++;
        }
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

// getFrame method
string API_IO_Sample::getFrame()
{
    // It should be verified the validity of each value

    checksum_ = frameType_;

    for(unsigned int i = 0; i < sourceAddress_.length(); i++){
        checksum_ += (unsigned char)sourceAddress_[i];
    }

    checksum_ += (unsigned char)sourceNetworkAddress_[0] + (unsigned char)sourceNetworkAddress_[1] + receiveOptions_ + numberOfSamples_;

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
    + (char)numberOfSamples_
    + receivedData_
    + (char)checksum_;
}
