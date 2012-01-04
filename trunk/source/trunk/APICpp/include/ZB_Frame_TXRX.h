///
/// The ZB_Frame_TXRX class.
/// This class purpose is to provide facilities for reading the output comming from the device interface
/// associated to the XBee radio, as well as to issue commands to that same XBee, by writing to configured
/// device interface.
/// To enable continuous monitoring and control of a XBee controller and eventually a mesh network, the listening
/// process for new frames shall be done in a thread.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef ZB_FRAME_TXRX_H
#define ZB_FRAME_TXRX_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Thread.h"

class ZB_Frame_TXRX: public Thread
{
    public:

        /** Default constructor */
        ZB_Frame_TXRX();

        /** Copy constructor
         *  \param other Object to copy from
         */
        ZB_Frame_TXRX(const ZB_Frame_TXRX& other);

        /** Constructor for instantiation of a new ZB_Frame_TXRX object for a given interface device.
         * \param device A string holding the value / path of the device to use.
         */
        ZB_Frame_TXRX(std::string device);

        /** Default destructor */
        virtual ~ZB_Frame_TXRX();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        ZB_Frame_TXRX& operator=(const ZB_Frame_TXRX& other);

        /** Get method to access the device member variable
         * \return A string holding the current value of device_
         */
        inline std::string getDevice(){
            return device_;
        }

        /** Set method for updating the value of device
         * \param device A string holding the new value to set.
         */
        inline void setDevice(std::string device){
            device_ = device;
        }

        /** Get method to access the message pool member variable.
         * \return A vector fo strings, holding the current pool of messages
         */
        inline std::vector<std::string> getMessagePool(){
            return messagePool_;
        }

        /** This method purpose is to return the message pool size.
         * \return A unsigned integer value holding the size of message pool vector.
         */
        inline unsigned int messagePoolSize(){
            return messagePool_.size();
        }

        /** Set method for updating the messagePool_ vector.
         * \param messagePool A vector of strings holding the new pool of messages to set
         */
        /*inline void setMessagePool(std::vector<std::string>& messagePool){
            messagePool_ = messagePool;
        }*/


        /** This method purpose is to open the thread where the device will be monitored for new messages. */
        void start();

        /** This method purpose is to stop the monitoring process by setting a boolean value to false,
         * thus terminating the active thread by the end of the funtion thread.
         * \sa listenDevice
         * \sa run_
         */
        void stop();

        /** This method purpose is to access the messagePool which updated, either by inserting new messages,
         * received from the interface device, or by popping one out, with the intention to process it.
         * This method uses a thread mutex to allow mutiple threads to access it while preventing the risk corrupt the message pool.
         * \param message A reference to string. When called, if parameter is empty(""), then it is considered a query call, returning the value
         *        in the message paramter. If otherwise, the string is not empty, it will be considered a message, and it is inserted on the message pool.
         */
        void accessMessagePool(std::string& message);

        /** This method purpose is to send a API Frame to the interface device applied to the connected XBee.
         * \param message A string holding the API frame to be sent.
         */
        void sendMessage(std::string message);

        /** This method is the implementation of the abstract Thread::job.
         * It is this method who will actually implement whatever the thread is suppose to do. In this case, it shall gather in the message pool
         * whatever messages arriving at the serial port.
         */
        void job();

    private:

        bool run_; //!< A boolean value for signalling the thread funtion to stop monitoring the XBee radio device for messages.
        int serial_fd_; //!< An integer value to hold the file descriptor to be used for reading and writing to the serial port where the XBee radio is connected.
        std::string device_; //!< A string holding the value / path of the device associated to the connected XBee radio.
        std::vector<std::string> messagePool_; //!< A vector of strings holding the pool of messages received from the connected XBee radio that haven't been processed yet.

};

#endif // ZB_FRAME_TXRX_H
