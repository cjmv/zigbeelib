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

#include <string>


class API_Frame
{
    public:

        API_Frame();
        virtual ~API_Frame();
        API_Frame(const API_Frame& other);
        API_Frame& operator=(const API_Frame& other);

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
        ///@return Returns the ZigBee standart API frame type.
        ///@note It's probably best if this attribute is actually an enum type that converted into hex value whenever necessary.
        ///
        inline unsigned char getFrameType() {
            return frameType_;
        }
        ///
        /// Set method for the ZigBee standart API Frame Type.
        ///@param frameType An unsigned char holding the standart ZigBee API frame type value to be taken.
        ///@note It's probably best if this attribute is actually an enum type that converted into hex value whenever necessary.
        ///
        inline void setFrameType(unsigned char frameType) {
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
        ///
        virtual void parseFrame(std::string frame) = 0;

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
        unsigned char frameType_; /// Unsigned char to hold the standart ZigBee API Frame type.
        unsigned char checksum_; /// Unsigned char to hold the checksum for the current API Frame.
                                 ///@sa calculateChecksum

    private:


};

#endif // API_FRAME_H
