///
/// The ZB_MonitoringAndControl class.
/// This class purpose is to provide high level facilities for interpreting the output comming from the device interface
/// associated to the XBee radio, as well as to issue typical query AT commands to that same XBee. This class can also be
/// instantiated to enable automatic search for nodes. If this is the case, this class shall gather information for each
/// existent node in the network on a node list.
/// To enable continuous monitoring and control of a XBee controller and eventually a mesh network, the listening
/// process for new frames shall be done in a thread.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */


#ifndef ZB_MONITORINGANDCONTROL_H
#define ZB_MONITORINGANDCONTROL_H

#include <map>
#include <queue>
#include <utility>

//#include "Thread.h"
#include "ZB_Frame_TXRX.h"
#include "ZB_Node.h"
#include "API_IO_Sample.h"
#include "API_AT_RemoteCommandResponse.h"
#include "API_AT_RemoteCommand.h"


class ZB_MonitoringAndControl: public Thread
{
    public:

        /** An enumerated type to specify the parameter value Type received from an AT response. */
        enum AT_ParameterValue_Type
        {
            UNSIGNED_INT = 0, /// unsigned int
            UNSIGNED_CHAR, /// unsigned char
            STRING, /// const char *
            NO_TYPE /// Default type
        };

        /** A structure to hold a resumed version of an AT response
         *\sa commandsResponse_buffer_
         */
        struct Resumed_AT_Response
        {
            API_AT_CommandResponse::CommandStatus commandStatus; /**< An enumerated value of type CommandStatus to hold
                                                                  * the AT command status.
                                                                  *\sa API_AT_CommandResponse::CommandStatus
                                                                  */
            AT_ParameterValue_Type parameterValueType; /**< An enumerated value of type AT_ParameterValue_Type that will hold
                                                        * the type of value to be considered on the parameterValue Union.
                                                        *\sa AT_ParameterValue_Type
                                                        *\sa parameterValue
                                                        */

            union {
                unsigned int ui_value;
                unsigned char uc_value;
            } parameterValue; //!< An union holding the value of the parameter received from the AT response.

            std::string s_value; /**< A STL string holding the value (if it's type is STRING) of the parameter
+                                     received from the AT response */
        };

        /** This method purpose is to provide an already created instance to the ZB_MonitoringAndControl object.
         * \return A ZB_MonitoringAndControl instantiated pointer, or 0 if still to be instantiated.
         * \note If after calling this method a ZB_MonitoringAndControl pointer is returned with 0, you should first
         * call the parameterizable getInstance(s) methods.
         */
         inline static ZB_MonitoringAndControl* getInstance(){
             return instance_;
         }

        /** This method purpose is to return an instance to the ZB_MonitoringAndControl object.
         * It will create a new instance if not instantiated yet, or just return the current one. In other words, if a class instance
         * was already created somewhere in the code, it doesn't matter the values of the parameters you set.
         * By default API mode will be set to use non-escaped control characters (ATAP=1)
         *  The auto update node list feature is turned ON (set to "true") by default.
         * \param device A string holding the tty interface that shall be used to send and receive ZigBee Frames
         *        from / to  controller.
         * \param max_sample_queue_size An unsigned long holding the maximum size of the sample queue before the oldest samples
         *        start to be excluded.
         * \return A ZB_MonitoringAndControl pointer.
         */
         static ZB_MonitoringAndControl* getInstance(std::string device, unsigned long max_sample_queue_size);

        /** This method purpose is to return an instance to the ZB_MonitoringAndControl object.
         * It will create a new instance if not instantiated yet, or just return the current one. In other words, if a class instance
         * was already created somewhere in the code, it doesn't matter the values of the parameters you set.
         * The auto update node list feature is turned ON (set to "true") by default.
         * \param api_mode A enumerated type holding the API mode to use: ESCAPED (ATAP=2) or NON-ESCAPED (ATAP=1)
         * \param device A string holding the tty interface that shall be used to send and receive ZigBee Frames
         *        from / to  controller.
         * \param max_sample_queue_size An unsigned long holding the maximum size of the sample queue before the oldest samples
         *        start to be excluded.
         * \return A ZB_MonitoringAndControl pointer.
         */
        static ZB_MonitoringAndControl* getInstance(ZB_Frame_TXRX::API_MODE api_mode, std::string device, unsigned long max_sample_queue_size);

        /** Copy constructor
         *  \param other Object to copy from
         */
        //ZB_MonitoringAndControl(const ZB_MonitoringAndControl& other);

        /** Default destructor */
        virtual ~ZB_MonitoringAndControl();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        ZB_MonitoringAndControl& operator=(const ZB_MonitoringAndControl& other);

        /** This method lets know if the auto node list update is on.
         * \return A bolean value in indicating if the auto node list update is on (true) or off (false).
         */
        inline bool isAutoUpdateON(){
            return auto_update_nodes_;
        }

        /** Set method to update the auto_update_nodes_ member variable.
         * \param auto_update_nodes A boolean parameter indicating if the auto node list update should be
         *                          turned on (if true) of off (if false)
         */
        inline void setAutoUpdate(bool auto_update_nodes){
            auto_update_nodes_ = auto_update_nodes;
        }

        /** Get method to access the network node list.
         * \return A STL vector holding the current value of node list member variable.
         */
        inline std::vector<ZB_Node*> getNodeList() {

            lock();
                std::vector<ZB_Node*> nodeList = nodeList_;
            unlock();

            return nodeList;
        }

        /** Set method to update the nodeList_ member variable value
         * \param nodeList A STL vector holding the new value to set
         */
        inline void setNodeList(std::vector<ZB_Node*> nodeList) {
            nodeList_ = nodeList;
        }

        /** This method purpose is to access and retrieve a given AT command response
         * with the given frame ID. The AT command response searched, if found, will be returned
         * and removed from the commands response buffer.
         * \param frameID And unsigned char with the frame ID to search concerning the AT command response received.
         * \param resumedAnswer A Resumed_AT_Response structure type that shall be instantiated on this method if
         *                      any AT response has been found with the given frameID.
         * \return A boolean value indicating if any AT response with the given frameID was found.
         * \sa Resumed_AT_Response
         * \sa API_AT_CommandResponse
         * \sa API_AT_RemoteCommandResponse
         */
        bool retrieveCommandsResponseBuffer(unsigned char frameID, Resumed_AT_Response& resumedResponse);

        /** This method purpose is to retrieve the IO Sample from the sample queue for processing.
         * By calling this method the oldest IO Sample is actually removed from the queue and it's reference is passed
         * through the io_sample pointer.
         * \param io_sample An API_IO_Sample object with the oldest sample frame in the queue.
         * \return A boolean value indicating if any IO Sample has been found.
         * \note The reason why the whole API_IO_Sample is returned here and not only the sample itself
         *       is because the IO SAmple data comming from a given XBee depends very much on how many
         *       sensors are being used on that node. For those cases where more than 1 pin is being used
         *       the programmer has also to access the analog and / or digital pin information. Since all this
         *       information is already in the API_IO_Sample class, there's no advantage on creating a
         *       subset of this class.
         */
        bool retrieveIOSample(API_IO_Sample* io_sample);

        /** This method purpose is to access the IO Sample given any of the node addresses or node ident
         * without removing it from the sample queue.
         * \param node An STL string holding the value of any of the 16 or 64 bits addresses or the node ident
         *             from the node you whish to see the oldest IO Sample in queue.
         * \param io_sample An API_IO_Sample object with the oldest sample frame in the queue.
         * \return A boolean value indicating if the an io sample has been found for the requested node.
         * \sa retrieveIOSample
         */
        //bool accessNodeIOSample(std::string node, API_IO_Sample* io_sample);

        /**
         * This method has the purpose to collect the network node list
         * and set it up on the node list member variable.
         * \param NI An std string holding the value of NI (node identifier) or MY (local address).
         *               If supplied, only the Node that matches the provided identifier will answer to the
         *               Node discover command.
         * \return An unsigned char with the frame ID used while sending the AT ND command.
         * \sa nodeList_
         */
        unsigned char discoverNetworkNodes(std::string NI = "");

        /**
         * This method purpose is to set a XBee node sleep mode (SM).
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *        network address (MY). If an empty string is presented instead, this command shall be issued
         *        to the controller.
         * \param sleepMode An enumerated value of type SleepMode holding the sleep mode to set.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         * \sa API_AT_Command::SleepMode
         */
        unsigned char setSleepMode(std::string networkAddr, API_AT_Command::SleepMode sleepMode);

        /**
         * This method purpose is to set / get the number of sleep periods (SN) for a given node.
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *        network address (MY). If an empty string is presented instead, this command shall be issued
         *        to the controller.
         * \param numberOfPeriods An unsigne integer holding the number of Periods during which this node should sleep.
         *                        The default value for this parameter is 0. If no other value is passed to this method
         *                        the method shall be treated has a query to the node, i.e., the XBee node shall return
         *                        the recorded value of (SN).
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         */
        unsigned char setNumberOfSleepPeriods(std::string networkAddr, unsigned int numberOfPeriods = 0);

        /**
         * This method purpose is to set the Sleep period (SP) for a given XBee node.
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *        network address (MY). If an empty string is presented instead, this command shall be issued
         *        to the controller.
         * \param sleepPeriod An unsigned integer holding the value (miliseconds) of the node sleeping period.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         */
        unsigned char setSleepPeriod(std::string networkAddr, unsigned int sleepPeriod);

        /**
         * This method purpose is to set the time a XBee node should wait until getting back to sleep (ST).
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *        network address (MY). If an empty string is presented instead, this command shall be issued
         *        to the controller.
         * \param timeBeforeSleep An unsigned integer holding the value (in miliseconds) of time before sleep.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         */
        unsigned char setTimeBeforeSleep(std::string networkAddr, unsigned int timeBeforeSleep);

        /**
         * This method purpose is to set the Sleep Options (SO) in a given XBee node.
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *        network address (MY). A valid, non-controller address must be provided.
         * \param sleepOptions An enumerated value of  type SleepOptions to hold the sleep option to be set.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         * \sa API_AT_Command::SleepOptions
         */
        unsigned char setSleepOptions(std::string networkAddr, API_AT_Command::SleepOptions sleepOptions);

        /**
         * This method purpose is to set /get the Node Identifier (NI) value for a given XBee node.
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *        network address (MY). If an empty string is presented instead, this command shall be issued
         *        to the controller.
         * \param nodeIdent A STL string holding the value (human readable) of the node identification.
         *                  The default value is an empty string. If no other value is given the method will
         *                  behave as a query, and the XBee node shall return the recorded value of NI.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         */
        unsigned char setNodeIdentifier(std::string networkAddr, std::string nodeIdent = "");

        /**
         * This method purpose is to set an end point node cyclic sleep configuration just by providing the
         * network address / node identification and the sleep period (in miliseconds)
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *                    network address (MY). This parameter must have a valid address or node
         *                    identification (non-empty). If an empty string is presented instead, this command
         *                    shall not be issued.
         * \param sleepPeriod An unsigned integer holding the value of the sleeping period to set in miliseconds.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         * \todo Not implemented yet.
         */
        unsigned char endpointCyclicSleepConfiguration(std::string networkAddr, unsigned long sleepPeriod);

        /**
         * This method purpose is to set / get the IO sample rate (IR) to / from a given XBee node.
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *                    network address (MY). This parameter must have a valid address or node
         *                    identification (non-empty). If an empty string is presented instead, this command
         *                    shall not be issued.
         * \param ioSampleRate An unsigned integer holding the IO sample rate to be set in miliseconds.
         *                     The default value of this parameter is 0. If this parameter isn't provided different
         *                     than 0, this method will query the XBee node for the recorded value of IR.
         * \return An unsigned char holding the value of the frameID generated for this AT command.
         */
        unsigned char setIOSampleRate(std::string networkAddr, unsigned int ioSampleRate = 0);

        /** This method purpose is to provide an easy way to send any AT command.
         * \param networkAddr A STL string holding the value of either the Node Identification (NI) or the
         *                    network address (MY). If empty, the AT command shall be treated as a local command and
         *                    sent to the controller.
         * \param addr A STL string to hold the 64 bits address for this node. If set as an empty string, the method,
         *             will try to get this address from the network node list, if the networkAddr is provided.
         * \param atCommand A STL string holding the AT command to be sent.
         * \param parameter A STL string holding the AT command parameter to be sent. If empty (default value), this AT
         *                  command shall be used to query the recorded value on the XBee module.
         * \param sleeping A boolean value indicating if this AT command is to be sent to an END POINT node which is set
         *                 to sleep. The default value is false. If set to true, This AT command to be issued shall be qeued
         *                 until I/O frame is received from the sleeping node.
         * \param option An RemoteCommandOption enumerated type holding the value of the AT remote command option
         *               to be considered. By default this value is "APPLY_CH". This parameter is ignored if the
         *               AT command is to be sent locally.
         * \param generatedFrame A pointer to a STL string holding the generated frame string by calling this method. This
         *                       paramater has the default value of 0. So is always possible to ignore it when calling the method.
         * \return An unsigned char holding the value of the frame ID sent with this command.
         */
        unsigned char sendATCommand(std::string networkAddr,
                                    std::string addr,
                                    std::string atCommand,
                                    std::string parameter = "",
                                    bool sleeping = false,
                                    API_AT_RemoteCommand::RemoteCommandOption option = API_AT_RemoteCommand::APPLY_CH,
                                    std::string* generatedFrame = 0);

        /** This method purpose is to write (WR) changes set in a given node to non-volatile memory, so that the configuration
         * is able to persist after a reset.
         * \param A STL string holding the value of either the Node Identification (NI) or the
         *                    network address (MY). This parameter must have a valid address or node
         *                    identification (non-empty). If empty, the AT command shall be treated as a local command and
         *                    sent to the controller.
         */
        unsigned char writeChanges(std::string networkAddr);

        //unsigned char writeChangesByIdentifier(std::string nodeIdentifier);

        /** This method purpose is to instantiate ZB_Frame_TXRX threaded class, so that
         * the controller XBee device can be monitored for new messages.
         */
        void startMonitoring();

        /** This method will stop the threaded class ZB_Frame_TXRX from monitor the XBee device controller.
         * The thread itself shall be terminated.
         */
        void stopMonitoring();

        /** This method purpose is to convert an integer number to a hexadecimal one (separated in bytes).
         * \param number An unsigned integer holding the value to be converted.
         * \return A STL string holding the converted number into a set of bytes.
         * \todo This type of routines should be contained in a Utils class accessible by all the project.
         */
        std::string int2Hex(unsigned int number);

        /** This method purpose is to update the Network Node List with a new given Node. This means that
         * the provided node will be added or simply updated (if its settings have been changed) on the current Node List.
         * \param node A pointer to ZB_Node holding all information, of a given node, related to a AT Node Discovery command answer.
         * \note The node pointer shall be dealt with within the method. Do not try to delete or change the pointer.
         */
        void updateNodeList(ZB_Node* node);

    protected:

    private:

        /** Constructor. By default API mode will be set to use non-escaped control characters (ATAP=1)
         *  The auto update node list feature is turned ON (set to "true") by default.
         * \param device A string holding the tty interface that shall be used to send and receive ZigBee Frames
         *        from / to  controller.
         * \param max_sample_queue_size An unsigned long holding the maximum size of the sample queue before the oldest samples
         *        start to be excluded.
         */
        ZB_MonitoringAndControl(std::string device, unsigned long max_sample_queue_size);

        /** Default constructor. The auto update node list feature is turned ON (set to "true") by default.
         * \param api_mode A enumerated type holding the API mode to use: ESCAPED (ATAP=2) or NON-ESCAPED (ATAP=1)
         * \param device A string holding the tty interface that shall be used to send and receive ZigBee Frames
         *        from / to  controller.
         * \param max_sample_queue_size An unsigned long holding the maximum size of the sample queue before the oldest samples
         *        start to be excluded.
         */
        ZB_MonitoringAndControl(ZB_Frame_TXRX::API_MODE api_mode, std::string device, unsigned long max_sample_queue_size);

        /** This method is the implementation of the abstract Thread::job.
         * It is this method who will actually implement whatever the thread is suppose to do. In this case, it shall gather in the message pool
         * whatever messages arriving at the serial port.
         */
        void job();

        /** This method purpose is to process any incoming AT command response, making the result available on diferent structures or classes,
         * depending on the type of AT command.
         * \param at_response An API_AT_CommandResponse pointer for the AT response to process.
         */
        void processATCommandStatus(API_AT_CommandResponse* at_response);

        /** This method only purpose is to iterate to the next frameID value.
         *  \return An unsigned char holding the value of the next frameID to be used.
         */
        unsigned char incrementFrameID();

        /** This method purpose is to count the number of bits used to express a given number.
         * \param number An unsigned int holding the value for which the number of bits is going to be calculated.
         * \return An unsigned integer holding the number of bits calculated.
         * \note This type of routines should be contained in a Utils class accessible by all the project.
         */
        unsigned int bitCount(unsigned int number);

        /** This method purpose is to check the existent of qeued AT commands and send them if the provided
         *  network address matches any of the destination network addresses of the qeued commands.
         * \param networkAddress A STL string holding the value of the network address to matched against.
         */
        unsigned int sendQeuedCommands(std::string networkAddress);

        /** This method purpose is to set the correct destination addresses and network addresses to the AT remote
         *  command to be sent.
         * \param remoteCommand An API_AT_RemoteCommand object to where the address should be set.
         * \note Needs further description.
         */
        bool setRemoteAddressing(API_AT_RemoteCommand* remoteCommand, std::string nodeIdent, std::string address = "");

        static ZB_MonitoringAndControl* instance_; //!< An instance to this class.

        bool run_; //!< A boolean variable indicating if this class thread should continue to run or not.

        bool auto_update_nodes_; /**< A boolean value indicating if this object should automatically update its node list
                                  *   with new discovered nodes. If set to false discoverNodes method must be called
                                  *   so that the node list is updated.
                                  */

        unsigned char frameId_; //!< An unsigned char variable holding the auto-incrementing frame ID to keep track of AT command responses.

        unsigned long max_sample_queue_size_; //!< An unsigned long holding the maximum number of IO Samples the sample queue can hold without starting to exclude older ones.

        std::vector<ZB_Node*> nodeList_; //!< A vector of pointers of ZB_Nodes. This vector shall be use to maintain a list of all active network nodes.

        std::map< unsigned char, Resumed_AT_Response > commandsResponse_buffer_; /**< A STL map holding the unseen commands responses.
                                                                                                        * This map key is the an unsigned char representing the frame ID to which the AT response refers to.
                                                                                                        * The object is a pair composed by the CommandStatus enumerated type (holding the respective command status)
                                                                                                        * and a STL string for those cases where the AT command returns a given node registry value.
                                                                                                        * In special cases this string value is actually parsed into a specialized structure as it is for the AT ND
                                                                                                        * command case.
                                                                                                        * \sa ZB_Node
                                                                                                        * \sa nodeList_
                                                                                                        */

        std::queue<API_IO_Sample*> sample_queue_; //!< A STL queue holds the queue of unprocessed samples represented by an API_IO_Sample object.


        std::vector< std::pair<unsigned char, std::string> > internalAT_frameID_vector_; /**< A STL vector holding a pair of frameID from the AT command issued and the frame string, that should
                                                                * be dealt internally.
                                                                */

        std::vector<API_AT_Command*> qeuedAT_Commands_vector_; /** < A STL vector holding the AT Commands queued to be issued. Tipically used, to send
                                                         * AT commands to sleeping END POINTS. The only requirement is that the Sleeping END POINT
                                                         * is configured to send an IO Sample as soon as it awakes.
                                                         */

        ZB_Frame_TXRX* txrx_; //!< A pointer to the ZB_Frame_TXRX, that will provide the interface with the XBee controller device.

        sem_t available_samples_; //!< A semaphor to allow effecient sample access without polling queries to check samples existence on the sample queue.
};

#endif // ZB_MONITORINGANDCONTROL_H
