///
/// The API_Frame abstract class. This class cannot be directly instantiated.
/// This class holds the common structure to all ZigBee API frames.
/// Beside the structure this class also presents several common functions that should be implemented by its children.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_FRAME_H
#define API_FRAME_H

//#define START_DELIMITER 0x7E

#include <string>

class API_Frame
{
    public:


        /** API Frame type enumeration for all known (for the time being) messages.
         * \note For the sake of documentation, maybe I should include a small description for each message.
         */
         enum FrameType {
             AT_COMMAND = 0x08,
             AT_COMMAND_QUEUED = 0x09,
             TX_REQUEST = 0x10,
             EADDRESS_TX_REQUEST = 0x11,
             AT_REMOTE_COMMAND = 0x17,
             CREATE_SRC_ROUTE = 0x21,
             AT_COMMAND_RESPONSE = 0x88,
             MODEM_STATUS = 0x8A,
             TX_STATUS = 0x8B,
             RX_PACKET = 0x90,
             EADDRESS_RX_PACKET = 0x91,
             IO_RX_SAMPLE = 0x92,
             SENSOR_READ = 0x94, // From Digi 1-wire sensor adapter.
             NODE_IDENT = 0x95, // see ND command.
             AT_REMOTE_RESPONSE = 0X97,
             OTA_FIRM_UPDATE = 0xA0,
             ROUTE_RECORD = 0xA1,
             MTO_ROUTE_REQUEST = 0xA3
        };

        /** Default constructor */
        API_Frame();

        /** Copy constructor
         *  \param other Object to copy from
         */
        //API_Frame(const API_Frame& other);

        /** Constructor to initialise an API Frame (through its children), holding common items to all frame types.
         * \param length An unsigned integer holding the length for this API Frame.
         * \param frameType A FrameType enumerated value holding the Frame type for this API message.
         */
        API_Frame(unsigned int length, FrameType frameType);

        /** Constructor to initialise an API Frame (through its children), holding common items to all frame types.
         * \param length An unsigned integer holding the length for this API Frame.
         * \param frameType A FrameType enumerated value holding the Frame type for this API message.
         * \param checksum An unsigned char holding the value of the calculated checksum
         */
        API_Frame(unsigned int length, FrameType frameType, unsigned char checksum);

        /** Default destructor */
        virtual ~API_Frame();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        //API_Frame& operator=(const API_Frame& other);

        ///
        /// Get method for API frame start delimiter.
        ///@return Returns a unsigned char with the value (in hexadecimal) of the standard ZigBee Frame start delimiter.
        ///
        inline unsigned char getStartDelimiter() {
            return startDelimiter_;
        }
        ///
        /// Set method for API frame start delimiter.
        ///@param startDelimiter An unsigned char holding the hexadecimal value.
        ///@todo Maybe this method should be removed, since the start delimiter must always be the same.
        ///
        inline void setStartDelimiter(unsigned char startDelimiter) {
            startDelimiter_ = startDelimiter;
        }

        ///
        /// Get method for the API frame length.
        ///@return Returns an unsigned integer with the length value.
        ///
        inline unsigned int getLength() {
            return length_;
        }
        ///
        /// Set method for the API frame length.
        ///@param length An unsigned integer holding the API length value to be taken.
        ///
        inline void setLength(unsigned int length) {
            length_ = length;
        }

        ///
        /// Get method for the ZigBee standart API Frame Type.
        ///@return Returns FrameType enumerated value holding the ZigBee standart API frame type.
        ///
        inline FrameType getFrameType() {
            return frameType_;
        }
        ///
        /// Set method for the ZigBee standart API Frame Type.
        ///@param frameType A FrameType enumerated value holding the standart ZigBee API frame type to be set.
        ///
        inline void setFrameType(FrameType frameType) {
            frameType_ = frameType;
        }

        ///
        /// Get method for the API frame checksum.
        ///@return Returns an unsigned char with the API Frame checksum value.
        ///
        inline unsigned char getChecksum() {
            return checksum_;
        }
        ///
        /// Set method for the API frame checksum.
        ///@param checksum An unsigned char that holds the respective checksum for the current API Frame.
        ///
        inline void setChecksum(unsigned char checksum) {
            checksum_ = checksum;
        }

        ///
        /// The main purpose of this pure virtual method is to parse an ZigBee API frame into the apropriate classes.
        /// Moreover this method also validates the message consistency by the checking its checksum.
        /// To verify checksum: Add all bytes (include checksum, but not the delimiter and length bytes).
        /// If the checksum is correct, the sum will equal 0xFF.
        ///@param frame String holding an entire API frame (from start delimiter to checksum).
        ///@return Returns a boolean value. "true" if frame was successfuly parse and false if otherwise.
        ///@note One of the reasons for the frame not being successfuly parse is if the checksum isn't valid.
        ///
        virtual bool parseFrame(std::string frame) = 0;

        /**
         * This method returns the frame based on the current object, and must be implemented by every
         * class inhereting from API_Frame.
         * \return Returns a string holding the frame for the current object.
         */
        virtual std::string getFrame() = 0;

        ///
        /// This method calculates the checksum for a given frame.
        /// To calculate: Not including frame delimiters and length, add all
        /// bytes, keeping only the lowest 8 bits of the result, and subtract the result from 0xFF.
        ///@param frame String holding the API frame from which the checksum shall be calculated.
        ///@return Returns an unsigned char holding the calculated checksum.
        ///
        unsigned char calculateChecksum(std::string frame);

    protected:

        unsigned char startDelimiter_; /// Unsigned char to hold the standart ZigBee API Frame start delimiter.
        unsigned int length_; /// Unsigned integer to hold the API Frame length.
        FrameType frameType_; /// FrameType enum to hold the standart ZigBee API Frame type.
        unsigned char checksum_; /// Unsigned char to hold the checksum for the current API Frame.
                                 ///@sa calculateChecksum

    private:


};

#endif // API_FRAME_H
