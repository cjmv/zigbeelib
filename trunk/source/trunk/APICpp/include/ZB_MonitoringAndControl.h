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

#include "Thread.h"
#include "ZB_Node"


class ZB_MonitoringAndControl : public Thread
{
    public:
        /** Default constructor */
        ZB_MonitoringAndControl();
        /** Default destructor */
        virtual ~ZB_MonitoringAndControl();
        /** Copy constructor
         *  \param other Object to copy from
         */
        ZB_MonitoringAndControl(const ZB_MonitoringAndControl& other);
        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        ZB_MonitoringAndControl& operator=(const ZB_MonitoringAndControl& other);
        /** Access nodeList_
         * \return The current value of nodeList_
         */
        std::vector<ZB_Node*> GetnodeList_() { return nodeList_; }
        /** Set nodeList_
         * \param val New value to set
         */
        void SetnodeList_(std::vector<ZB_Node*> val) { nodeList_ = val; }

        /** This method is the implementation of the abstract Thread::job.
         * It is this method who will actually implement whatever the thread is suppose to do. In this case, it shall gather in the message pool
         * whatever messages arriving at the serial port.
         */
        void job();

    protected:
    private:
        std::vector<ZB_Node*> nodeList_; //!< Member variable "nodeList_"
};

#endif // ZB_MONITORINGANDCONTROL_H
