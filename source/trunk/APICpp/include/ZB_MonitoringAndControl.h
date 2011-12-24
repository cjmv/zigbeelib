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

#include "ZB_Frame_TXRX.h"
#include "ZB_Node"
#include "API_IO_Sample.h"
#include "API_AT_RemoteCommandResponse.h"


class ZB_MonitoringAndControl : public Thread
{
    public:

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
        inline std::vector<ZB_Node*>& getNodeList() {
            return nodeList_;
        }

        /** Set method to update the nodeList_ member variable value
         * \param nodeList A STL vector holding the new value to set
         */
        inline void setNodeList(std::vector<ZB_Node*> nodeList) {
            nodeList_ = nodeList;
        }

        /**
         * This method has the purpose to collect the network node list
         * and set it up on the node list member variable.
         * \return An unsigned char with the frame ID used while sending the AT ND command.
         * \sa nodeList_
         */
        unsigned char discoverNetworkNodes();

        /** This method purpose is to instantiate ZB_Frame_TXRX threaded class, so that
         * the controller XBee device can be monitored for new messages.
         */
        void startMonitoring();

        /** This method will stop the threaded class ZB_Frame_TXRX from monitor the XBee device controller.
         * The thread itself shall be terminated.
         */
        void stopMonitoring();

        /** This method is the implementation of the abstract Thread::job.
         * It is this method who will actually implement whatever the thread is suppose to do. In this case, it shall gather in the message pool
         * whatever messages arriving at the serial port.
         */
        void job();

    protected:

    private:

        void processATCommandStatus(API_AT_CommandResponse* at_response);

        bool run_; //!< A boolean variable indicating if this class thread should continue to run or not.

        unsigned char frameId_; //!< An unsigned char variable holding the auto-incrementing frame ID to keep track of AT command responses.

        std::vector<ZB_Node*> nodeList_; //!< A vector of pointers of ZB_Nodes. This vector shall be use to maintain a list of all active network nodes.

        std::map< unsigned char, std::pair< API_AT_CommandResponse::CommandStatus, std::string> > commandsResponse_buffer_; /**< A STL map holding the unseen commands responses.
                                                                                                        * This map key is the an unsigned char representing the frame ID to which the AT response refers to.
                                                                                                        * The object is a pair composed by the CommandStatus enumerated type (holding the respective command status)
                                                                                                        * and a STL string for those cases where the AT command returns a given node registry value.
                                                                                                        * In special cases this string value is actually parsed into a specialized structure as it is for the AT ND
                                                                                                        * command case.
                                                                                                        * \sa ZB_Node
                                                                                                        * \sa nodeList_
                                                                                                        */

        ZB_Frame_TXRX* txrx_;
};

#endif // ZB_MONITORINGANDCONTROL_H
