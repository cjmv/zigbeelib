///
/// Definition of ZB_Node class.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "ZB_Node.h"

using namespace std;

// Default constructor
ZB_Node::ZB_Node()
{
    //ctor
}

// AT response parameter constructor
ZB_Node::ZB_Node(string responseParameter)
{
    // If this condition fails the object is created empty.
    // Exception treatment is needed. One should be throw if this condition isn't met.
    // Another way to avoid this, is to not provide this constructor and only a function to parse the AT response parameter.
    if(responseParameter.size() > 18){

        networkAddr_ = responseParameter.substr(0, 2);
        responseParameter.erase(0, 2);
        serialNumberHigh_ = responseParameter.substr(0, 4);
        responseParameter.erase(0, 4);
        serialNumberLow_ = responseParameter.substr(0, 4);
        responseParameter.erase(0, 4);
        manufacturerIdent_ = responseParameter.substr(responseParameter.size()-3, 2);
        responseParameter.erase(responseParameter.size()-3, 2);
        profileIdent_ = responseParameter.substr(responseParameter.size()-3, 2);
        responseParameter.erase(responseParameter.size()-3, 2);
        status_ = (unsigned char)responseParameter.at(responseParameter.size()-1);
        responseParameter.erase(responseParameter.size()-1);
        deviceType_ = (DeviceType)(unsigned char)responseParameter.at(responseParameter.size()-1);
        responseParameter.erase(responseParameter.size()-1);
        parentNetworkAddr_ = responseParameter.substr(responseParameter.size()-3, 2);
        responseParameter.erase(responseParameter.size()-3, 2);
        nodeIdent_ = responseParameter.erase(responseParameter.size()-1, 1);
    }
}

// Full constructor
ZB_Node::ZB_Node(string networkAddr, string serialNumberHigh, string serialNumberLow, string nodeIdent, string parentNetworkAddr,
                 DeviceType deviceType, unsigned char status, string profileIdent, string manufacturerIdent)
                 : networkAddr_(networkAddr), serialNumberHigh_(serialNumberHigh), serialNumberLow_(serialNumberLow), nodeIdent_(nodeIdent),
                 parentNetworkAddr_(parentNetworkAddr), deviceType_(deviceType), status_(status), profileIdent_(profileIdent), manufacturerIdent_(manufacturerIdent)
{

}

ZB_Node::~ZB_Node()
{
    //dtor
}

ZB_Node::ZB_Node(const ZB_Node& other)
{
    //copy ctor
}

/*ZB_Node& ZB_Node::operator=(const ZB_Node& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}*/

// Operator << overloading
ostream& operator << (ostream& out, const ZB_Node& node)
{
    unsigned int i = 0;
    out << "Network address: " << hex;
    for(; i < node.networkAddr_.size(); i++){
        out << (int)(unsigned char)node.networkAddr_[i] << " ";
    }
    out << endl;

    i = 0;
    out << "Serial Number (High): ";
    for(; i < node.serialNumberHigh_.size(); i++){
        out << (int)(unsigned char)node.serialNumberHigh_[i] << " ";
    }
    out << endl;

    i = 0;
    out << "Serial Number (Low): ";
    for(; i < node.serialNumberLow_.size(); i++){
        out << (int)(unsigned char)node.serialNumberLow_[i] << " ";
    }
    out << endl;

    out << "Node Identification: " << node.nodeIdent_ << endl;

    i = 0;
    out << "Parent Network Address: ";
    for(; i < node.parentNetworkAddr_.size(); i++){
        out << (int)(unsigned char)node.parentNetworkAddr_[i] << " ";
    }
    out << endl;

    out << "Device type: ";
    switch(node.deviceType_){

        case ZB_Node::COORD:
            out << "COORDINATOR";
            break;

        case ZB_Node::ROUTER:
            out << "ROUTER";
            break;

        case ZB_Node::END_DEVICE:
            out << "END POINT";
            break;

        default:
            out << "UNKNOWN";
            break;
    }
    out << endl;

    out << "Status: " << (int)node.status_ << endl;

    i = 0;
    out << "Profile ID: ";
    for(; i < node.profileIdent_.size(); i++){
        out << (int)(unsigned char)node.profileIdent_[i] << " ";
    }
    out << endl;

    i = 0;
    out << "Manufacturer ID: ";
    for(; i < node.manufacturerIdent_.size(); i++){
        out << (int)(unsigned char)node.manufacturerIdent_[i] << " ";
    }
    out << dec << endl;


    return out;
}

bool operator == (const ZB_Node& node1, const ZB_Node& node2)
{
    if(node1.deviceType_ != node2.deviceType_)
        return false;
    else if(node1.manufacturerIdent_ != node2.manufacturerIdent_)
        return false;
    else if(node1.networkAddr_ != node2.networkAddr_)
        return false;
    else if(node1.nodeIdent_ != node2.nodeIdent_)
        return false;
    else if(node1.parentNetworkAddr_ != node2.parentNetworkAddr_)
        return false;
    else if(node1.profileIdent_ != node2.profileIdent_)
        return false;
    else if(node1.serialNumberHigh_ != node2.serialNumberHigh_)
        return false;
    else if(node1.serialNumberLow_ != node2.serialNumberLow_)
        return false;
    else if(node1.status_ != node2.status_)
        return false;
    else
        return true;
}

// parse node identification method
bool ZB_Node::parseNodeIdentification(string responseParameter)
{
    if(responseParameter.size() > 18){

        networkAddr_ = responseParameter.substr(0, 2);
        responseParameter.erase(0, 2);
        serialNumberHigh_ = responseParameter.substr(0, 4);
        responseParameter.erase(0, 4);
        serialNumberLow_ = responseParameter.substr(0, 4);
        responseParameter.erase(0, 4);
        manufacturerIdent_ = responseParameter.substr(responseParameter.size()-3, 2);
        responseParameter.erase(responseParameter.size()-3, 2);
        profileIdent_ = responseParameter.substr(responseParameter.size()-3, 2);
        responseParameter.erase(responseParameter.size()-3, 2);
        status_ = (unsigned char)responseParameter.at(responseParameter.size()-1);
        responseParameter.erase(responseParameter.size()-1);
        deviceType_ = (DeviceType)(unsigned char)responseParameter.at(responseParameter.size()-1);
        responseParameter.erase(responseParameter.size()-1);
        parentNetworkAddr_ = responseParameter.substr(responseParameter.size()-3, 2);
        responseParameter.erase(responseParameter.size()-3, 2);
        nodeIdent_ = responseParameter;

        return true;
    }
    else
        return false;
}
