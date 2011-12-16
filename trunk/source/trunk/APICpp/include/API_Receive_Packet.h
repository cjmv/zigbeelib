///
/// The API_Receive_Packet class.
/// This class holds the structure for a ZigBee Receive packet.
/// When the module receives a RF packet, it is sent out the UART using this message type.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef API_RECEIVE_PACKET_H
#define API_RECEIVE_PACKET_H

#include <API_Frame.h>


class API_Receive_Packet : public API_Frame
{
    public:

        /** Receveive Options enumerated type.
         * These options values can be combined. For example, a 0x40 and a 0x01 will show as 0x41.
         */
        enum ReceiveOptions {
            PACKET_ACKNOWLEDGED = 0x01,
            BROADCAST_PACKET = 0x02,
            APS_ENCRYPTED = 0x20,
            END_DEVICE = 0x40
        };

        /** Default constructor */
        API_Receive_Packet();

        /** Copy constructor
         *  \param other Object to copy from
         */
        API_Receive_Packet(const API_Receive_Packet& other);

        /** Constructor for intantiation of a API_Receive_Packet object with only specific information regarding this type of frame.
         * \param sourceAddress A string holding the value of the message 64 bits source address.
         * \param sourceNetworkAddress A string holding the value of the message 16 bits source network address.
         * \param receiveOptions A ReceiveOptions enumerated type holding the message receive options value.
         * \param receivedMessage A string holding the received message itself, i.e, the intended data to be delivered.
         */
         API_Receive_Packet(std::string sourceAddress, std::string sourceNetworAddress, ReceiveOptions receiveOptions, std::string receivedMessage);

         /** Constructor that provides full instantiation of a API_Receive_Packet object with all relevant information regarding this type of frame.
          * \param length An unsigned integer holding the length for this API Frame.
          * \param framType A FrameType enumerated value holding the Frame type for this API message.
          * \param checksum An unsigned char holding the value of the calculated checksum
          * \param sourceAddress A string holding the value of the message 64 bits source address.
          * \param sourceNetworkAddress A string holding the value of the message 16 bits source network address.
          * \param receiveOptions A ReceiveOptions enumerated type holding the message receive options value.
          * \param receivedMessage A string holding the received message itself, i.e, the intended data to be delivered.
          */
         API_Receive_Packet(unsigned int length, FrameType frameType, unsigned char checksum,
                            std::string sourceAddress, std::string sourceNetworAddress, ReceiveOptions receiveOptions, std::string receivedMessage);

        /** Default destructor */
        virtual ~API_Receive_Packet();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        API_Receive_Packet& operator=(const API_Receive_Packet& other);

        /** Get method to access source address memeber variable.
         * \return String holding the current value of sourceAddress_
         */
        inline std::string getSourceAddress(){
            return sourceAddress_;
        }

        /** Set method for updating the source address value.
         * \param sourceAddress A string value holding the new value to set.
         */
        inline void setSourceAddress(std::string sourceAddress){
            sourceAddress_ = sourceAddress;
        }

        /** Get method to access source network address member variable.
         * \return String holding the current value of sourceNetworkAddress_
         */
        inline std::string getSourceNetworkAddress(){
            return sourceNetworkAddress_;
        }

        /** Set method for updating the source network address value.
         * \param sourceNetworkAddress A string holding the new value to set
         */
        inline void setSourceNetworkAddress(std::string sourceNetworkAddress){
            sourceNetworkAddress_ = sourceNetworkAddress;
        }

        /** Get method to access the receive options member variable.
         * \return ReceiveOptions enumerated type holding the current value of receiveOptions_
         */
        inline ReceiveOptions getReceiveOptions(){
            return receiveOptions_;
        }

        /** Set method to update the receive options value.
         * \param receiveOptions A ReceiveOptions enumerated type holding the new value to set
         */
        inline void setReceiveOptions(ReceiveOptions receiveOptions){
            receiveOptions_ = receiveOptions;
        }

        /** Get method to access received data member variable.
         * \return A string holding the current value of receivedData_. This arbitrary data, doesn't need to follow specific structure.
         */
        inline std::string getReceivedData(){
            return receivedData_;
        }

        /** Set method to update the received data value.
         * \param receivedData A string holding the new value to set.
         */
        inline void setReceivedData(std::string receivedData){
            receivedData_ = receivedData;
        }

        /**
         * The main purpose of this pure virtual method is to parse an ZigBee API frame into the apropriate classes.
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

        std::string sourceAddress_; //!< A string holding the value of the message source address.
        std::string sourceNetworkAddress_; //!< A string holding the value of the message source network address
        ReceiveOptions receiveOptions_; //!< A ReceiveOptions enumerated type holding the message receive options.
        std::string receivedData_; //!< A string holding the received data itself.

    private:

};

#endif // API_RECEIVE_PACKET_H
