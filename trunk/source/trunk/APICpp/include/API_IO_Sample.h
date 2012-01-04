///
/// The API_IO_Sample class.
/// This class holds the structure for a ZigBee IO Data Sample Rx Indicator frame.
/// When the module receives an IO sample frame from a remote device, it sends the sample out the UART using
/// this frame type (when AO=0). Only modules running API firmware will send IO samples out the UART.
/// The ZigBee I/O Sample Rx Indicator is really just an extension of the ZigBee Receive Packet. The main
/// difference is that instead of the payload having an  arbitrary or unconstrained format, it is organized
/// in a highly structured way.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_IO_SAMPLE_H
#define API_IO_SAMPLE_H

#include <vector>
#include <utility>

#include <API_Receive_Packet.h>


class API_IO_Sample : public API_Receive_Packet
{
    public:

        /** Default constructor */
        API_IO_Sample();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_IO_Sample(const API_IO_Sample& other);

        /** Default destructor */
        virtual ~API_IO_Sample();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        //API_IO_Sample& operator=(const API_IO_Sample& other);

        /** Constructor for instantiation of an API_IO_Sample object with only specific information regarding this API Frame.
         * \param numberOfSamples An unsigned short holding the number of samples.
         * \param digitalChannelMask An unsigned integer holding the digital channel mask.
         * \param analogChannelMask An unsigned short holding the analog channel mask.
         * \param digitalSamples A string holding the digital samples.
         * \param analogSamples A string holding the analog samples.
         * \note This constructor  might be removed since the initialisation of all these member variables will only (probably) happen
         *       from parsing the API_Receive_Packet::receivedMessage.
         */
        API_IO_Sample(unsigned short numberOfSamples, unsigned int digitalChannelMask, unsigned short analogChannelMask,
                      std::vector<unsigned short> digitalSamples, std::vector< std::pair<unsigned short, std::string> > analogSamples);

        /** Constructor for full instantiation of an API_IO_Sample object with all relevant information for this API Frame.
         * \param length An unsigned integer holding the length for this API Frame.
         * \param framType A FrameType enumerated value holding the Frame type for this API message.
         * \param checksum An unsigned char holding the value of the calculated checksum
         * \param sourceAddress A string holding the value of the message 64 bits source address.
         * \param sourceNetworkAddress A string holding the value of the message 16 bits source network address.
         * \param receiveOptions A ReceiveOptions enumerated type holding the message receive options value.
         * \param receivedMessage A string holding the received message itself, i.e, the intended data to be delivered.
         */
        API_IO_Sample(unsigned int length, FrameType frameType, unsigned char checksum,
                      std::string sourceAddress, std::string sourceNetworAddress, ReceiveOptions receiveOptions, std::string receivedMessage);

        /** Get method to access number of samples member variable.
         * \return An unsigned short holding the number of samples on this message.
         */
        inline unsigned short getNumberOfSamples(){
            return numberOfSamples_;
        }

        /** Set method for updating the number of samples.
         * \param numberOfSamples An unsigned short holding the new value to set.
         */
        inline void setNumberOfSamples(unsigned int numberOfSamples){
            numberOfSamples_ = numberOfSamples;
        }

        /** Get method to access digital channel mask member variable.
         * \return An unsigned integer holdin the current value of the digital channel mask.
         */
        inline unsigned int getDigitalChannelMask(){
            return digitalChannelMask_;
        }

        /** Set method for updating the digital channel mask.
         * \param digitalChannelMask An unsigned integer holding the new value to set.
         */
        inline void setDigitalChannelMask(unsigned int digitalChannelMask){
            digitalChannelMask_ = digitalChannelMask;
        }

        /** Get method to access the analog channel mask member variable.
         * \return An unsigned short holding the current value of analog channel mask.
         */
        inline unsigned short getAnalogChannelMask(){
            return analogChannelMask_;
        }

        /** Set method for updating the analog channel mask.
         * \param analogChannelMask An unsigned short holding the new value to set.
         */
        inline void setAnalogChannelMask(unsigned short analogChannelMask){
            analogChannelMask_ = analogChannelMask;
        }

        /** Get method to access the digital samples member variable.
         * \return A vector of unsigned short holding the current value of digitalSamples_
         */
        inline std::vector<unsigned short> getDigitalSamples(){
            return digitalSamples_;
        }

        /** Set method for updating the digital samples
         * \param digitalSampes A vector of unsigned short holding the new value to set
         */
        inline void setDigitalSamples(std::vector<unsigned short> digitalSamples){
            digitalSamples_ = digitalSamples;
        }

        /** Get method to access analog samples member variable.
         * \return A vector of pairs holding the current value of analogSamples.
         */
        inline std::vector< std::pair<unsigned short, std::string> >& getAnalogSamples(){
            return analogSamples_;
        }

        /** Set method for updating the analog samples.
         * \param analogSamples A vector of pairs holding the new value to set.
         */
        void setAnalogSamples(std::vector< std::pair<unsigned short, std::string> > analogSamples){
            analogSamples_ = analogSamples;
        }

        /**
         * The main purpose of this pure virtual method is to parse an ZigBee API frame into the apropriate classes.
         * Moreover this method also validates the message consistency by the checking its checksum.
         * To verify checksum: Add all bytes (include checksum, but not the delimiter and length bytes).
         * If the checksum is correct, the sum will equal 0xFF.
         * \param frame String holding an entire API frame (from start delimiter to checksum).
         * \return Returns a boolean value. "true" if frame was successfuly parse and false if otherwise.
         * \note One of the reasons for the frame not being successfuly parse is if the checksum isn't valid.
         */
        bool parseFrame(std::string frame);

        /**
         * This method returns the frame based on the current object.
         * \return Returns a string holding the frame for the current object.
         */
        std::string getFrame();

    protected:
    private:

        static unsigned int digitalPinsMask_; //!< A static unsigned integer holding the digital pins Mask. Unless the XBee physically changes, this value will never change.
        static unsigned short analogPinsMask_; //!< A static unsigned short holding the analog pins mask. Unless the XBee physically changes, this value will never change.

        unsigned short numberOfSamples_; //!< An unsigned integer holding the number of samples for this Frame. for ZBv2 it will always be 1.
        unsigned int digitalChannelMask_; //!< An umsigned integer holding the digital channel mask.
        unsigned short analogChannelMask_; //!< An unsigned short holding the analog channel mask.
        std::vector<unsigned short> digitalSamples_; //!< A vector holding the digital pins set high by the means of unsigned shorts.
        std::vector< std::pair<unsigned short, std::string> > analogSamples_; /** A vector of pairs holding the pins receiving analog samples.
                                                                               *  The pair contains the pin (as un unsigned short) and a string with the analog sample itself.
                                                                               */
};

#endif // API_IO_SAMPLE_H
