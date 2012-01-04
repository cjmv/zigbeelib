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

    /**
     * Enumerated type holding all AT commands described as possible
     * in the XBee documentation.
     * For clearer identification the enumeration has the following format:
     * <command type>_<node type>_<command>.
     *
     * The "command type" field is identified as follow:
     * ADDR -> Addressing commands.
     * NETW -> Networking commands.
     * SERIAL -> Serial interface commands
     * SLEEP -> Sleep commands.
     * EXEC -> Execution commands.
     *
     * The "node type" field is identified as C=Coordinator, R=Router and E=End Device.
     */
     // All Addressing and sleep commands are enumerated.
    enum ATCommands {
        UNKNOWN_COMMAND = 0x00,
        ADDR_CRE_DESTINATION_ADDRESS_HIGH = 0x4448,
        ADDR_CRE_DESTINATION_ADDRESS_LOW = 0x444C,
        ADDR_CRE_16BIT_NETWORK_ADDRESS = 0x4D59,
        ADDR_E_16BIT_PARENT_NETWORK_ADDRESS = 0x4D50,
        ADDR_CR_NUMBER_REMAINING_CHILDREN = 0x4E43,
        ADDR_CRE_SERIAL_NUMBER_HIGH = 0x5348,
        ADDR_CRE_SERIAL_NUMBER_LOW = 0x534C,
        ADDR_CRE_NODE_IDENTIFIER = 0x4E49,
        ADDR_CRE_SOURCE_ENDPOINT = 0x5345,
        ADDR_CRE_DESTINATION_ENDPOINT = 0x4445,
        ADDR_CRE_CLUSTER_IDENTIFIER = 0x4349,
        ADDR_CRE_MAX_RF_PAYLOAD_BYTES = 0x4E50,
        ADDR_CRE_DEVICE_TYPE_IDENTIFIER = 0x4444,
        NETW_CRE_EXTENDED_PAN_ID = 0x4944,
        NETW_CRE_NODE_DISCOVERY_TIMEOUT = 0x4E54,
        NETW_CR_NODE_JOIN_TIME = 0x4E4A,
        NETW_R_NETWORK_WATCHDOG_TIMEOUT = 0x4E57,
        SERIAL_CRE_INTERFACE_DATA_RATE = 0x4244,
        SLEEP_RE_SLEEP_MODE = 0x534D,
        SLEEP_CRE_NUMBER_SLEEP_PERIODS = 0x534E,
        SLEEP_CRE_SLEEP_PERIOD = 0x5350,
        SLEEP_E_TIME_BEFORE_SLEEP = 0x5354,
        SLEEP_E_SLEEP_OPTIONS = 0x534F,
        SLEEP_E_WAKE_HOST = 0x5748,
        SLEEP_E_SLEEP_IMMEDIATELY = 0x5349,
        SLEEP_E_POLLING_RATE = 0x504F,
        EXEC_CRE_NODE_DISCOVERY = 0x4E44,
        EXEC_CRE_DESTINATION_NODE = 0x444E,
        EXEC_CRE_FORCE_SAMPLE = 0x4953,
        EXEC_RE_XBEE_SENSOR_SAMPLE = 0x3153
    };

    enum SleepMode {
        SLEEP_DISABLED = 0x0,
        PIN_SLEEP_ENABLED = 0x1,
        CYCLIC_SLEEP_ENABLED = 0x4,
        CYCLIC_SLEEP_PIN_WAKE = 0x5
    };

    enum SleepOptions {
        ALWAYS_WAKE = 0x02,
        SLEEP_ENTIRE_TIME = 0x04
    };

        static const unsigned int BDParameterRange[];


        /** Default constructor */
        API_AT_Command();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_AT_Command(const API_AT_Command& other);

        /** Constructor for instantiation of an API AT Frame with common relevant parameters.
         * \param frameId An unsigned char holding the frame ID for this message.
         * \param atCommand An enumerated ATCommands type holding the AT command to be sent.
         * \param parameterValue A string holding the parameter value associated with the AT Command.
         *                       If empty, this AT command shall query the module.
         */
        API_AT_Command(unsigned char frameId, ATCommands atCommand, std::string parameterValue);

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
        //API_AT_Command& operator=(const API_AT_Command& other);

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
        ///@return Returns a boolean value. "true" if frame was successfuly parse and false if otherwise.
        ///@note One of the reasons for the frame not being successfuly parse is if the checksum isn't valid.
        ///
        virtual bool parseFrame(std::string frame);

        /**
         * This method returns the frame based on the current object.
         * \return Returns a string holding the frame for the current object.
         */
        virtual std::string getFrame();

    protected:


        unsigned char frameId_;
        std::string atCommand_;
        std::string parameterValue_; /// A string holding the AT command parameter value.

    private:

};

#endif // API_AT_COMMAND_H
