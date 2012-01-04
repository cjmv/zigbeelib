///
/// The ZB_Node class.
/// This class purpose, is to hold and deal with Node identity information, i.e., the information retrieved with issuing
/// a Node Discovery AT command (AT ND):
///     MY
///     SH
///     SL
///     NI (Variable length)
///     PARENT_NETWORK ADDRESS (2 Bytes)
///     DEVICE_TYPE (1 Byte: 0=Coord, 1=Router, 2=End Device)
///     STATUS (1 Byte: Reserved)
///     PROFILE_ID (2 Bytes)
///     MANUFACTURER_ID (2 Bytes)
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#ifndef ZB_NODE_H
#define ZB_NODE_H

#include <string>
#include <ostream>


class ZB_Node
{
    public:

        /**
         * Enumerated type identifying the 3 possible types of Devices:
         * COORD - Coordinator (it only exists 1)
         * ROUTER - Routers to sense and forward commands fromt/to end devices / coordinators. Routers may never sleep.
         * END_DEVICE - Modules that have the unique purpose to sense and send information to the Coordinator. With this
         *              configuration, the ZigBee radios can sleep for a configurable ammount of time and awake just to
         *              send sensing data or to receive incoming commands.
         */
        enum DeviceType {
            COORD = 0,
            ROUTER,
            END_DEVICE
        };


        /** Default constructor */
        ZB_Node();

        /** This constructor returns a ZB_Node object by providing the AT Node discovery command response parameter
         * \param responseParameter A string holding the AT Node discovery command response parameter.
         *                          The string should have this format:
         *                              MY (2 bytes)
         *                              SH (4 bytes)
         *                              SL (4 bytes)
         *                              NI (Variable length)
        `*                              PARENT_NETWORK ADDRESS (2 Bytes)
         *                              DEVICE_TYPE (1 Byte: 0=Coord, 1=Router, 2=End Device)
         *                              STATUS (1 Byte: Reserved)
         *                              PROFILE_ID (2 Bytes)
         *                              MANUFACTURER_ID (2 Bytes)
         */
        ZB_Node(std::string responseParameter);

        /** Full constructor.
         *  This constructor returns a ZB_Node object by providing all the identification node parameters returned from the AT node discovery command.
         * \param networkAddress A string holding the value of the network address.
         * \param serialNumberHigh A string holding the high 32 bits part of the module serial number.
         * \param serialNumberLow A string holding the low 32 bits part of the module serial number.
         * \param nodeIdent A string holding the user friendly node identification.
         * \param parentNetworkAddress A string holding this module parent network address.
         * \param deviceType An enumerated DeviceType holding this device type.
         * \param status An unsigned char holding the status value.
         * \param profileIdent A string holding the value of the profile identification.
         * \param manufacturerIdent A string holding the value of the manufacturer identification.
         */
        ZB_Node(std::string networkAddress, std::string serialNumberHigh, std::string serialNumberLow, std::string nodeIdent, std::string parentNetworkAddr,
                DeviceType deviceType, unsigned char status, std::string profileIdent, std::string manufacturerIdent);

        /** Copy constructor
         *  \param other Object to copy from
         */
        ZB_Node(const ZB_Node& other);

        /** Default destructor */
        virtual ~ZB_Node();

        /** Assignment operator
         *  \param other Object to assign from
         *  \return A reference to this
         */
        //ZB_Node& operator=(const ZB_Node& other);

        /** << Operator overloading to allow printing to the standart output all node information.
         * \param out The std::ostream to be used.
         * \param node The ZB_Node class to be output.
         */
        friend std::ostream& operator << (std::ostream& out, const ZB_Node& node);


        /** Get method to access node network address.
         * \return A string holding the value of the networkAddress_ member variable.
         */
        inline std::string getNetworkAddr() {
            return networkAddr_;
        }

        /** Set method for updating the value of the network address.
         * \param networkAddr A string holding the new value to set.
         */
        inline void setNetworkAddr(std::string networkAddr) {
            networkAddr_ = networkAddr;
        }

        /** Get method to access the 32 bits high serial number.
         * \return A string holding the current value of serialNumberHigh_.
         */
        inline std::string getSerialNumberHigh() {
            return serialNumberHigh_;
        }

        /** Set method for updating the value of the 32 bits high serial number.
         * \param serialNumberHigh A string holding the new value to set.
         */
        inline void setSerialNumberHigh(std::string serialNumberHigh) {
            serialNumberHigh_ = serialNumberHigh;
        }

        /** Get merhod to access 32 bits low serial number.
         * \return A string holding the current value of serialNumberLow_.
         */
        inline std::string getSerialNumberLow() {
            return serialNumberLow_;
        }

        /** Set method for updating the value of the 32 bits low serial number.
         * \param serialNumberLow A string holding the new value to set.
         */
        inline void setSerialNumberLow(std::string serialNumberLow) {
            serialNumberLow_ = serialNumberLow;
        }

        /** Get method to access the node user friendly identification (a string with something like "kitchen sensor").
         *  The string maximum length is of 20 bytes, thus 20 characteres.
         * \return A string holding the current value of nodeIdent_.
         */
        inline std::string getNodeIdent() {
            return nodeIdent_;
        }

        /** Set method for updating the value of node user friendly identification.
         * \param nodeIdent A string holding the new value to set.
         */
        inline void setNodeIdent(std::string nodeIdent) {
            nodeIdent_ = nodeIdent;
        }

        /** Get method to access parent network address.
         * \return A string holding the current value of parentNetworkAddr_.
         */
        inline std::string getParentNetworkAddr() {
            return parentNetworkAddr_;
        }

        /** Set method for updating the value of the parent network address.
         * \param parentNetworkAddr A string holding the new value to set.
         */
        inline void setParentNetworkAddr(std::string parentNetworkAddr) {
            parentNetworkAddr_ = parentNetworkAddr;
        }

        /** Get method to access device type.
         * \return A enumerated type DeviceType holding the current value of deviceType_.
         */
        inline DeviceType getDeviceType() {
            return deviceType_;
        }

        /** Set method for updating the value of the device type.
         * \param deviceType A enumerated type DeviceType holding the new value to set.
         */
        inline void setDeviceType(DeviceType deviceType) {
            deviceType_ = deviceType;
        }

        /** Get method to access the status.
         * \return A unsigned char holding the current value of status_.
         */
        inline unsigned char getStatus() {
            return status_;
        }

        /** Set method for updating the value of status_.
         * \param status An unsigned char holding the new value to set.
         */
        inline void setStatus(unsigned char status) {
            status_ = status;
        }

        /** Get method to access the profile identification.
         * \return A string holding the current value of profileIdent_.
         */
        inline std::string getProfileIdent() {
            return profileIdent_;
        }

        /** Set method for updating the value of the profile identification.
         * \param profileIdent A string holding the new value to set.
         */
        inline void setProfileIdent(std::string profileIdent) {
            profileIdent_ = profileIdent;
        }

        /** Get method to access manufacturer identification.
         * \return A string holding the current value of manufacturerIdent_.
         */
        inline std::string getManufacturerIdent() {
            return manufacturerIdent_;
        }

        /** Set method for updating the value of the manufacturer identification.
         * \param manufacturerIdent A string holding the new value to set.
         */
        inline void setManufacturerIdent(std::string manufacturerIdent) {
            manufacturerIdent_ = manufacturerIdent;
        }

        /** This method purpose is to parse the AT Node discovery response parameter.
         * \param responseParameter A string holding the node identification parameters.
         * \sa ZB_Node(std::string responseParameter)
         */
        bool parseNodeIdentification(std::string responseParameter);


    protected:

    private:
        std::string networkAddr_; /**< A string Member variable holding the value of the network address.
                                   *   This was address provided by the controller when this module entered the network. */
        std::string serialNumberHigh_; //!< A string Member variable holding the value of the high 32 bits part of the serial number.
        std::string serialNumberLow_; //!< A string Member variable holding the value of the low 32 bits part of the serial number.
        std::string nodeIdent_; //!< A string Member variable holding the value of the user friendly node identification.
        std::string parentNetworkAddr_; //!< A string Member variable holding the value of parent network address.
        DeviceType deviceType_; //!< A DeviceType enumerated type Member variable holding the device type.
        unsigned char status_; //!< An unsigned char Member variable holding the status value.
        std::string profileIdent_; //!< A string Member variable holding the profile identification.
        std::string manufacturerIdent_; //!< A string Member variable holding the value of the manufacturer identification.
};

#endif // ZB_NODE_H
