///
/// The API_AT_RemoteCommand class.
/// This class purpose, as a child of API_AT_Command class is to hold all relevant information as part of an AT command remotely sent,
/// either for querying a given registry or to set it with an updated value.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_AT_REMOTECOMMAND_H
#define API_AT_REMOTECOMMAND_H

#include <API_AT_Command.h>

/** Enumerated type to hold all possible remote command options. */
enum RemoteCommandOption {
    DISABLE_ACK = 0x01,
    APPLY_CH = 0x02,
    EXT_TX_TIMEOUT = 0x40
};

class API_AT_RemoteCommand : public API_AT_Command
{
    public:
        /** Default constructor */
        API_AT_RemoteCommand();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_AT_RemoteCommand(const API_AT_RemoteCommand& other);

        /** Specific constructor for instantiation of a API_AT_RemoteCommand object.
         * \param destinationAddress A string holding the destination address to where this AT command shall be issued.
         * \param destinationNetworkAddress A string holding the destination network address to where this AT command shall be issued.
         * \param remoteCommandOption A RemoteCommandOption enumeration holding the remote command option that shall be applied to this AT command.
         */
        API_AT_RemoteCommand(std::string destinationAddress, std::string destinationNetworkAddress, RemoteCommandOption remoteCommandOption);

        /** Full Constructor for instantiation of a API_AT_RemoteCommand object with all relevant data.
         * \param length An unsigned integer holding the length of the API Frame.
         * \param frameType A FrameType enumerated type holding the API frame type.
         * \param atCommand A string holding the AT command.
         * \param parameterValue A string holding the parameter value to send. If empty the AT command shall act as a query.
         * \param destinationAddress A string holding the destination address to where this AT command shall be issued.
         * \param destinationNetworkAddress A string holding the destination network address to where this AT command shall be issued.
         * \sa API_Frame::length
         */
        API_AT_RemoteCommand(unsigned int length,
                             FrameType frameType,
                             unsigned char frameId,
                             std::string atCommand,
                             std::string parameterValue,
                             std::string destinationAddress,
                             std::string destinationNetworkAddress,
                             RemoteCommandOption remoteCommandOption);

        /** Default destructor */
        virtual ~API_AT_RemoteCommand();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        API_AT_RemoteCommand& operator=(const API_AT_RemoteCommand& other);

        /** Get method to access destination address.
         * \return Returns a string value holding the destination address.
         */
        inline std::string getDestinationAddress(){
            return destinationAddress_;
        }
        /** Set method for the destination address.
         * \param destinationAddress String value holding the new destination address to set.
         */
        inline void setDestinationAddress(std::string destinationAddress){
            destinationAddress_ = destinationAddress;
        }

        /** Get method for the destination network address.
         * \return Returns a string value holding the destination network address.
         */
        inline std::string getDestinationNetworkAddress(){
            return destinationNetworkAddress_;
        }
        /** Set method for the destination network address.
         * \param destinationNetworkAddress String value holding the new destination network address to be set.
         */
        inline void setDestinationNetworkAddress(std::string destinationNetworkAddress){
            destinationNetworkAddress_ = destinationNetworkAddress;
        }

        /** Get method for the remote command option.
         * \return Returns a RemoteCommandOption enumeration type holding the remote command option.
         */
        inline RemoteCommandOption getRemoteCommandOption(){
            return remoteCommandOption_;
        }
        /** Set method for the remote command options.
         * \param remoteCommandOption RemoteCommandOption unumerated type holding the new value for the remote command option.
         */
        inline void setRemoteCommandOption(RemoteCommandOption remoteCommandOption){
            remoteCommandOption_ = remoteCommandOption;
        }

        /**
        * The main purpose of this method is to parse an ZigBee API frame into the apropriate classes.
        * Moreover this method also validates the message consistency by the checking its checksum.
        * To verify checksum: Add all bytes (include checksum, but not the delimiter and length bytes).
        * If the checksum is correct, the sum will equal 0xFF.
        * \param frame String holding an entire API frame (from start delimiter to checksum).
        */
        void parseFrame(std::string frame);


    protected:
    private:
        std::string destinationAddress_; //!< String member variable holding the value for the destination address
        std::string destinationNetworkAddress_; //!< String member variable holding the value for the destination network address.
        RemoteCommandOption remoteCommandOption_; /**< RemoteCommandOption member member variable holding the value for the remote command option.
                                             * Possible values are:
                                             *                      0x01 - Disable Ack
                                             *                      0x02 - Apply changes on remote
                                             *                      0x40 - Use the extended transmission timeout for this destination
                                             */
};

#endif // API_AT_REMOTECOMMAND_H
