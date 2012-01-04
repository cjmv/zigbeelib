///
/// The ZB_Frame_TXRX class.
/// This class purpose is to provide high level facilities for interpreting the output comming from the device interface
/// associated to the XBee radio, as well as to issue typical query AT commands to that same XBee.
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

//#include "Thread.h"
#include "ZB_Frame_TXRX.h"
#include "ZB_Node.h"
#include "API_IO_Sample.h"
#include "API_AT_RemoteCommandResponse.h"
#include "API_AT_RemoteCommand.h"


class ZB_MonitoringAndControl: public Thread
{
    public:

        enum AT_ParameterValue_Type
        {
            UNSIGNED_INT = 0,
            UNSIGNED_CHAR,
            STRING,
            NO_TYPE
        };

        struct Resumed_AT_Response
        {
            API_AT_CommandResponse::CommandStatus commandStatus;
            AT_ParameterValue_Type parameterValueType;
            void* parameterValue;
        };

        /** Default constructor
         * \param device A string holding the tty interface that shall be used to send and receive ZigBee Frames
         *        from / to  controller.
         */
        ZB_MonitoringAndControl(std::string device);

        /** Copy constructor
         *  \param other Object to copy from
         */
        ZB_MonitoringAndControl(const ZB_MonitoringAndControl& other);

        /** Default destructor */
        virtual ~ZB_MonitoringAndControl();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        ZB_MonitoringAndControl& operator=(const ZB_MonitoringAndControl& other);

        /** Get method to access the network node list.
         * \return A STL vector holding the current value of node list member variable.
         */
        inline std::vector<ZB_Node*> getNodeList() {
            return nodeList_;
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

        /** This method purpose is to access the IO Sample regarding a given node in the network.
         * \param node A STL string holding the user defined NodeIdent (information obtain from AT NI),
         *             or the node network address.
         * \param io_sample An API_IO_Sample object with the sample frame for the requested node.
         * \return A boolean value indicating if the an io sample has been found for the requested node.
         * \note The reason why the whole API_IO_Sample is returned here and not only the sample itself
         *       is because the IO SAmple data comming from a given XBee depends very much on how many
         *       sensors are being used on that node. For those cases where more than 1 pin is being used
         *       the programmer has also to access the analog and / or digital pin information. Since all this
         *       information is already in the API_IO_Sample class, there's not great advantage on creating a
         *       subset of this class.
         */
        bool accessNodeIOSample(std::string node, API_IO_Sample* io_sample);

        /**
         * This method has the purpose to collect the network node list
         * and set it up on the node list member variable.
         * \return An unsigned char with the frame ID used while sending the AT ND command.
         * \sa nodeList_
         */
        unsigned char discoverNetworkNodes();

        unsigned char setSleepMode(std::string networkAddr, API_AT_Command::SleepMode sleepMode);

        unsigned char setNumberOfSleepPeriods(std::string networkAddr, unsigned int numberOfPeriods);

        unsigned char setSleepPeriod(std::string networkAddr, unsigned int sleepPeriod);

        unsigned char setSleepOptions(std::string networkAddr, API_AT_Command::SleepOptions);

        unsigned char setNodeIdentifier(std::string networkAddr, std::string nodeIdent);

        unsigned char endpointCyclicSleepConfiguration(std::string networkAddr, unsigned long sleepPeriod);

        unsigned char writeChanges(std::string networkAddr);

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
         * \note This type of routines should be contained in a Utils class accessible by all the project.
         */
        std::string int2Hex(unsigned int number);

    protected:

    private:

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

        bool setRemoteAddressing(std::string nodeIdent, API_AT_RemoteCommand* remoteCommand);

        bool run_; //!< A boolean variable indicating if this class thread should continue to run or not.

        unsigned char frameId_; //!< An unsigned char variable holding the auto-incrementing frame ID to keep track of AT command responses.

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

        std::map< std::string, API_IO_Sample*> nodeSample_map_; //!< A STL map with the node identification as the key and the corresponding sample represented by an API_IO_Sample object.

        ZB_Frame_TXRX* txrx_; //!< A pointer to the ZB_Frame_TXRX, that will provide the interface with the XBee controller device.
};

#endif // ZB_MONITORINGANDCONTROL_H
