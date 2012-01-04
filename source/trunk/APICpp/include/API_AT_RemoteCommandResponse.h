///
/// The API_AT_RemoteCommandResponse class.
/// This class purpose, as a child of API_AT_CommandResponse class is to hold all relevant information regarding AT remnote command responses.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_AT_REMOTECOMMANDRESPONSE_H
#define API_AT_REMOTECOMMANDRESPONSE_H

#include <API_AT_CommandResponse.h>


class API_AT_RemoteCommandResponse : public API_AT_CommandResponse
{
    public:
        /** Default constructor */
        API_AT_RemoteCommandResponse();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_AT_RemoteCommandResponse(const API_AT_RemoteCommandResponse& other);

        /** Constructor for instantiation of specific API_AT_RemoteCommandResponse information.
         * \param sourceAddress A string holding the source address from where this frame came from.
         * \param sourceNetworkAddress A string holding the source network address from where this frame came from.
         */
        API_AT_RemoteCommandResponse(std::string destinationAddress, std::string destinationNetworkAddress);

        /** Constructor for instantiation of a API_AT_RemoteCommandResponse object with full relevant information.
         * \param length An unsigned integer holding the API frame length.
         * \param frameType A FrameType enumerated type holding the API Frame Type.
         * \param frameId An unsigned char holding the frame ID concerning the AT remote coomand.
         * \param parameterValue A string holding the parameter value. Empty if the AT remote command was to set / update a given parameter.
         * \param commandStatus A CommandStatus enumerated type to hold the command status referent to the issued AT command.
         * \param sourceAddress A string holding the source address from where this frame came from.
         * \param sourceNetworAddress A string holding the source network address from there this frame came from.
         */
        API_AT_RemoteCommandResponse(unsigned int length,
                                     FrameType frameType,
                                     unsigned char frameId,
                                     std::string atCommand,
                                     std::string parameterValue,
                                     CommandStatus commandStatus,
                                     std::string sourceAddress,
                                     std::string sourceNetworkAddress);

        /** Default destructor */
        virtual ~API_AT_RemoteCommandResponse();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        //API_AT_RemoteCommandResponse& operator=(const API_AT_RemoteCommandResponse& other);

        /** Get method to access the source address member variable.
         * \return Returns a string holding the value of the source address.
         */
        inline std::string getSourceAddress(){
            return sourceAddress_;
        }
        /** Set method for updating the source address member variable.
         * \param destinationAddress String value holding the new source address to be set.
         */
        inline void setSourceAddress(std::string sourceAddress){
            sourceAddress_ = sourceAddress;
        }

        /** Get method to access the source network address member variable.
         * \return Returns a string holding the current value of the source network address.
         */
        inline std::string getSourceNetworkAddress(){
            return sourceNetworkAddress_;
        }
        /** Set method for updating the source network address member variable.
         * \param destinationNetworAddress String value holding the new source network address to be set.
         */
        inline void setSourceNetworkAddress(std::string sourceNetworkAddress){
            sourceNetworkAddress_ = sourceNetworkAddress;
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
        bool parseFrame(std::string frame);

        /**
         * This method returns the frame based on the current object.
         * \return Returns a string holding the frame for the current object.
         */
        std::string getFrame();

    protected:

    private:

        std::string sourceAddress_; //!< String member variable holding the value for the destination address
        std::string sourceNetworkAddress_; //!< String member variable holding the value for the destination network address.
};

#endif // API_AT_REMOTECOMMANDRESPONSE_H
