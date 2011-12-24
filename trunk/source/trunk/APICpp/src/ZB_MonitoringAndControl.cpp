///
/// Definition of ZB_MonitoringAndControl class.
///

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "ZB_MonitoringAndControl.h"

using namespaces std;

// Default constructor
ZB_MonitoringAndControl::ZB_MonitoringAndControl(string device): Thread()
{
    //ctor
    if(device.compare("") != 0)
        txrx = new ZB_Frame_TXRX(device);
        frameId_ = 1;
        run_ = false;
        return this;
    else
        return 0;
}

ZB_MonitoringAndControl::~ZB_MonitoringAndControl()
{
    //dtor

    delete txrx;
}

ZB_MonitoringAndControl::ZB_MonitoringAndControl(const ZB_MonitoringAndControl& other)
{
    //copy ctor
}

ZB_MonitoringAndControl& ZB_MonitoringAndControl::operator=(const ZB_MonitoringAndControl& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

// Discover network nodes method
unsigned char ZB_MonitoringAndControl::discoverNetworkNodes()
{
    API_AT_Command *atCommand = new API_AT_Command(frameId_, "ND", "");

    txrx_->sendMessage(atCommand->getFrame());
    commandsResponse_buffer_[frameId_] = make_pair(API_AT_CommandResponse::UNKOWN_STATUS, ""); // Populating the map with a new frame ID regarding a AT command.
    frameId_ += 0x1;

    return frameId_ - 0x1;
}

// start monitoring method
void ZB_MonitoringAndControl::startMonitoring()
{
    txrx->start();
    openThread();
    run_ = true;
}

// stop monitoring method
void ZB_MonitoringAndControl::stopMonitoring()
{
    run_ = false;
    txrx->stop();
}

// job threaded method
void ZB_MonitoringAndControl::job()
{
    string message = "";
    while(run_)
    {
        mc->accessMessagePool(message);
            /*for(unsigned int x = 0; x < message.length(); x++){
                cout << hex << (int)(unsigned char)message[x] << " ";
            }
            cout << endl;*/

        if(message.compare("") != 0){
            switch((unsigned char)message[3])
            {

                case API_Frame::AT_COMMAND_RESPONSE:

                    API_AT_CommandResponse* at_response;

                    cout << "It's an AT_COMMAND_RESPONSE" << endl;
                    frame = new API_AT_CommandResponse();
                    frame->parseFrame(message);

                    at_response = dynamic_cast<API_AT_CommandResponse*>(frame);

                    cout << "Command Status: ";
                    processATCommandStatus(at_response);

                    delete at_response;
                    break;

                case API_Frame::MODEM_STATUS:

                    cout << "It's a MODEM STATUS" << endl;
                    break;

                case API_Frame::TX_STATUS:

                    cout << "It's TRANSMISSION STATUS" << endl;
                    break;

                case API_Frame::RX_PACKET:

                    cout << "It's a RECEIVE PACKET" << endl;
                    frame = new API_Receive_Packet();
                    frame->parseFrame(message);
                    break;

                case API_Frame::EADDRESS_RX_PACKET:

                    cout << "It's EXPLICIT RX INDICATOR" << endl;
                    break;

                case API_Frame::IO_RX_SAMPLE:
                {
                    string sample = "";
                    API_IO_Sample* io_sample;

                    cout << "It's an IO Sample" << endl;
                    frame = new API_IO_Sample();
                    frame->parseFrame(message);

                    io_sample = dynamic_cast<API_IO_Sample*>(frame);
                    sample = io_sample->getAnalogSamples().begin()->second;

                    if(!networkNodes.empty()){

                        //cout << "Comparing..." << endl;
                        for(unsigned int i = 0; i < networkNodes.size(); i++){

                            /*cout << *networkNodes[i];

                            cout << endl << "Source Network Address:" << hex;
                            for(unsigned int x = 0; x < io_sample->getSourceNetworkAddress().size(); x++){
                                cout << (int)(unsigned char)io_sample->getSourceNetworkAddress()[x] << " ";
                            }

                            cout << dec << endl;*/

                            if(networkNodes[i]->getNetworkAddr().compare(io_sample->getSourceNetworkAddress()) == 0){

                                cout << "Source: " << networkNodes[i]->getNodeIdent() << endl;
                            }
                        }
                    }

                    getTemperatureCelsius((unsigned char)sample[0]*0x100 + (unsigned char)sample[1]);

                    delete io_sample;
                    break;
                }
                case API_Frame::NODE_IDENT:

                    cout << "It's NODE IDENT" << endl;
                    break;

                case API_Frame::AT_REMOTE_RESPONSE:
                    cout << "It's  a AT REMOTE RESPONSE" << endl;
                    frame = new API_AT_RemoteCommandResponse();
                    frame->parseFrame(message);
                    break;

                default:
                    cout << "Another message type..." << endl;
                    break;
            }
            message = "";
        }
    }
}

// Process AT command status method
void ZB_MonitoringAndControl::processATCommandStatus(API_AT_CommandResponse* at_command)
{
    switch(at_response->getCommandStatus())
    {
        // If OK status has been received from an issued AT command, then the parameter value should be shown (if any).
        case API_AT_CommandResponse::OK:
        {

            unsigned int bandwith = 0;
            API_AT_CommandResponse::ATCommands atCommand = API_AT_CommandResponse::UNKNOWN_COMMAND;
            cout << "OK" << endl;
            cout << "AT Command: " << at_response->getATCommand() << endl;


            /* Depending on the command issued, the parameter value should get different treatment. For this particular case
             * the AT command was BD, so BD parameter ranges apply.
             */
            atCommand = API_AT_CommandResponse::ATCommands(((unsigned  char)at_response->getATCommand()[0]*0x100) + (unsigned  char)at_response->getATCommand()[1]);

            if (atCommand == API_AT_CommandResponse::SERIAL_CRE_INTERFACE_DATA_RATE){

                commandsResponse_buffer_[at_response->getFrameId()] = make_pair(at_response->getCommandStatus(), at_response->getParameterValue()); // This doesn't make much sense, since I'm able to interpret
                                                                                                                                                    // the acual result in the next "for" cicle.
                for(unsigned int i = 0; i < at_response->getParameterValue().length(); i++){

                    if(i == 0)
                        bandwith = ((unsigned char)at_response->getParameterValue()[i] * 0x100) + (unsigned char)at_response->getParameterValue()[i+1];

                    else if(i != at_response->getParameterValue().length()-1)
                        bandwith = (bandwith * 0x100) + (unsigned char)at_response->getParameterValue()[i+1];
                }

                cout << dec << API_AT_CommandResponse::BDParameterRange_[bandwith] << endl;

            }
            else if (atCommand == API_AT_CommandResponse::EXEC_CRE_NODE_DISCOVERY){

                node = new ZB_Node(at_response->getParameterValue());

                /*if(nodeArray[0] == 0)
                    nodeArray[0] = node;
                else
                    nodeArray[1] = node;*/

                networkNodes.push_back(node);
                /*cout << "FROM ARRAY..." << endl;

                if(nodeArray[1] == 0)
                    cout << *nodeArray[0] << endl;
                else{
                    cout << *nodeArray[0] << endl;
                    cout << *nodeArray[1] << endl;
                }*/
                //networkNodes.push_back(ZB_Node(at_response->getParameterValue()));
                /*cout << "FROM ELEMENT..." << endl;
                cout << *node << endl;
                cout << "ELEMENTS ON vector..." << networkNodes.size() << endl;
                for (unsigned int i = 0; i < networkNodes.size(); i++){
                    cout << *networkNodes[i] << endl;
                }*/
                //cout << networkNodes.back();

                //delete node;
            }
            // Otherwise just print the parameter value
            else{

                cout << "Parameter value: ";
                for(unsigned int i = 0; i < at_response->getParameterValue().length(); i++){

                    cout << hex << (int)(unsigned char)at_response->getParameterValue()[i];
                }
            }

            cout << endl;

            break;
        }
        case API_AT_CommandResponse::ERROR:
            cout << "ERROR" << endl;
            break;

        case API_AT_CommandResponse::INVALID_COMMAND:
            cout << "INVALID COMMAND" << endl;
            break;

        case API_AT_CommandResponse::INVALID_PARAMETER:
            cout << "INVALID PARAMETER" << endl;
            break;

        case API_AT_CommandResponse::TX_FAILURE:
            cout << "TX_FAILURE" << endl;
            break;

        default:
            cout << "UNKNOWN" << endl;
            break;
    }
}
