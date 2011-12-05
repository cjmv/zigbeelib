///
/// The API_AT_CommandResponse class.
/// This class purpose, as a child of API_AT_Command class is to hold all relevant information regarding AT command responses.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_AT_COMMANDRESPONSE_H
#define API_AT_COMMANDRESPONSE_H

#include <API_AT_Command.h>

class API_AT_CommandResponse : public API_AT_Command
{
    public:

        /** An enumerated value to hold the Command Status possible values- */
        enum CommandStatus {
            OK = 0,
            ERROR,
            INVALID_COMMAND,
            INVALID_PARAMETER,
            TX_FAILURE
        };

        /** Default constructor */
        API_AT_CommandResponse();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_AT_CommandResponse(const API_AT_CommandResponse& other);

        /** Constructor for instantiation of specific API AT command response object.
         * \param commandStatus A CommandStatus enumerated type to hold the command status of the issued command.
         */
        API_AT_CommandResponse(CommandStatus commandStatus);

        /** Constructor for instantiation of full API AT command response object.
         * \param length An unsigned integer holding the API frame length.
         * \param frameType A FrameType enumerated type holding the API Frame Type.
         * \param frameId An unsigned integer holding the frame ID, regarding the AT command sent.
         * \param atCommand A string holding the AT command to which this answer referes to.
         * \param parameterValue A string holding the parameter value referent to the AT command sent. Empty if the AT command issued was to set / update a given parameter.
         * \param commandStatus A CommandStatus enumerated type holding the command status for the AT command issued.
         * \sa CommandStatus
         * \note Indicating the FrameType on this classes is redundant, since the simple instantiation of this class should already indicate the FrameType.
         *       It would probably be best to initialise the frameType_ member variable to the relevant value.
         */
        API_AT_CommandResponse(unsigned int length, FrameType frameType, unsigned char frameId, std::string atCommand, std::string parameterValue, CommandStatus commandStatus);

        /** Default destructor */
        virtual ~API_AT_CommandResponse();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        API_AT_CommandResponse& operator=(const API_AT_CommandResponse& other);

        /** Get method to access commandStatus_
         * \return Unsigned char holding the current value of AT response command status.
         */
        inline unsigned char getCommandStatus(){
            return commandStatus_;
        }
        /** Set method for AT Command Status.
         * \param commandStatus Unsigned char holding the AT response command status to set.
         * \note This set merhod might not make sense since this value will (probably) never be manually set.
         */
        inline void setCommandStatus(unsigned char commandStatus){
            commandStatus_ = commandStatus;
        }

        /**
        * The main purpose of this method is to parse an ZigBee API frame into the apropriate classes.
        * Moreover this method also validates the message consistency by the checking its checksum.
        * To verify checksum: Add all bytes (include checksum, but not the delimiter and length bytes).
        * If the checksum is correct, the sum will equal 0xFF.
        * \param frame String holding an entire API frame (from start delimiter to checksum).
        * \return Returns a boolean value. "true" if frame was successfuly parse and false if otherwise.
        * \note One of the reasons for the frame not being successfuly parse is if the checksum isn't valid.
        */
        virtual bool parseFrame(std::string frame);

        /**
         * This method returns the frame based on the current object.
         * \return Returns a string holding the frame for the current object.
         */
        virtual std::string getFrame();


    protected:

        unsigned char commandStatus_; /** Unsigned char member variable holding the AT response command status. Possible values are:
                                       * 0 - OK
                                       * 1 - ERROR
                                       * 2 - Invalid Command
                                       * 3 - Invalid Parameter
                                       * 4 - Tx failure
                                       */
    private:

};

#endif // API_AT_COMMANDRESPONSE_H
