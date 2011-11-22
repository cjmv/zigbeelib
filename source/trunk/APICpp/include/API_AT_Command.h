///
/// The API_AT_Command class.
/// This class purpose, as a child of API_Frame abstract class is to hold all relevant information to send AT Commands,
/// either for querying a given registry or to set it with an updated value.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_AT_COMMAND_H
#define API_AT_COMMAND_H

#include <API_Frame.h>


class API_AT_Command : public API_Frame
{
    public:

        /** Default constructor */
        API_AT_Command();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_AT_Command(const API_AT_Command& other);

        /** Constructor for instantiation of an API AT Frame with common relevant parameters.
         * \param frameId An unsigned char holding the frame ID for this message.
         * \param atCommand A string holding the AT command to be sent.
         * \param parameterValue A string holding the parameter value associated with the AT Command.
         *                       If empty, this AT command shall query the module.
         */
        API_AT_Command(unsigned char frameId, std::string atCommand, std::string parameterValue);

        /** Constructor for instantiation of an API AT Frame with all relevant parameters
         * \param length unsigned integer holding the API frame length.
         * \param frameType FrameType enumeration holding the API frame type specifie on the ZigBee standard.
         * \param atCommand A string value holding the AT command to be sent.
         * \param parameterValue A string value holding the AT command parameter value.
         */
        API_AT_Command(unsigned int length, FrameType frameType, unsigned char frameId, std::string atCommand, std::string parameterValue);

        /** Default destructor */
        virtual ~API_AT_Command();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        API_AT_Command& operator=(const API_AT_Command& other);

        /** Get method to access the current value of the frame ID.
        * \return Returns an unsigned char holding the frame id parameter value.
        */
        inline unsigned char getFrameId(){
            return frameId_;
        }

        /** Set method to update the frame ID value for this message.
        * \param frameId An unsigned char holding the frame ID value to be set.
        */
        inline void setFrameId(unsigned char frameId){
            frameId_ = frameId;
        }


        ///
        /// Get method for the AT command.
        ///@return Returns a string holding the AT command.
        ///
        inline std::string getATCommand() {
            return atCommand_;
        }

        ///
        /// Set method for the AT command.
        ///@param atCommand A string holding the AT command to be set.
        ///
        inline void setATCommand(std::string atCommand) {
            atCommand_ = atCommand;
        }

        ///
        /// Get method for the AT command parameter value.
        /// If no parameter value is provided the ZigBee radio shall be queried for it, instead of setting it.
        ///@return Returns a string with the AT command parameter value.
        inline std::string getParameterValue() {
            return parameterValue_;
        }

        ///
        /// Set method for the AT command parameter value.
        ///@param parameterValue A string holding the AT command parameter value to set the ZigBee radio.
        ///
        inline void setParameterValue(std::string parameterValue) {
            parameterValue_ = parameterValue;
        }

        ///
        /// The main purpose of this method is to parse an ZigBee API frame into the apropriate classes.
        /// Moreover this method also validates the message consistency by the checking its checksum.
        /// To verify checksum: Add all bytes (include checksum, but not the delimiter and length bytes).
        /// If the checksum is correct, the sum will equal 0xFF.
        ///@param frame String holding an entire API frame (from start delimiter to checksum).
        ///
        virtual void parseFrame(std::string frame);

    protected:

        unsigned char frameId_;
        std::string atCommand_;
        std::string parameterValue_; /// A string holding the AT command parameter value.

    private:

};

#endif // API_AT_COMMAND_H
