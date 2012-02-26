/*
 * File created to test the Library
 */

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "ZB_MonitoringAndControl.h"
#include "ZB_Node.h"
#include "API_IO_Sample.h"
#include "API_AT_RemoteCommandResponse.h"

using namespace std;

bool unitTesting()
{

    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
                                                           std::cerr ) );
    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}

double voltage_divider(vector<int>& imp_elements, int grounded_element, double Vin, double Vout)
{
	int aux = 0;

	for (unsigned int i = 0; i < imp_elements.size(); i++){

		aux += imp_elements[i];
	}

	if (Vout == 0.0)
		return ((double)grounded_element * Vin) / (double)aux;

	else if (Vin == 0.0)
		return ((double)aux * Vout) / (double)grounded_element;

	else
		return 1.0;
}

double AD_output_to_mV (int AD_value)
{
	return ((double)AD_value * (double)1200.0) / (double)1024.0;
}

double get_lm335A_Sensor_KelvinValue(double AD_mV)
{
	return AD_mV / (double)10.0;
}

double kelvin2Celsius(double kelvinDeg)
{
	return kelvinDeg - (double)273.15;
}

void getTemperatureCelsius(int hexAD)
{
	double ADmV = 0.0, kelvin_deg = 0.0, celcius_deg = 0.0;

	cout << "AD in HEX: " << hex << hexAD << dec << endl;
	ADmV = AD_output_to_mV(hexAD);
	cout << "AD(mV): " << ADmV << "mV" << endl;
	ADmV *= (double)3.0;
	cout << "AD(mV) w/t VD: " << ADmV << "mV" << endl;
	kelvin_deg = get_lm335A_Sensor_KelvinValue(ADmV);
	cout << "Temperature (Kelvin): " << kelvin_deg << " Kelvin" << endl;
	celcius_deg = kelvin2Celsius(kelvin_deg);
	cout << "Temperature (Celcius): " << celcius_deg << "ºC" << endl;

}

void useTXRX()
{
    char option[256];
    string message = "";
    ZB_Frame_TXRX *mc = new ZB_Frame_TXRX("/dev/ttyUSB0");
    API_Frame* frame;
    ZB_Node* node;
    vector<ZB_Node*> networkNodes;
    //ZB_Node* nodeArray[2] = {0};

    //mc->setDevice("/dev/ttyUSB0");

    mc->start();


     while(true){

        cout << "Option: ";
        cin.getline(option, 256, '\n');

        if (string(option).compare("length") == 0)
            cout << "Message pool size: " << mc->messagePoolSize() << endl;

        else if (string(option).compare("print") == 0){
            cout << "Message: " << endl;
            mc->accessMessagePool(message);
            for(unsigned int x = 0; x < message.length(); x++){
                cout << hex << (int)(unsigned char)message[x] << " ";
            }
            cout << endl;

            switch((unsigned char)message[3])
            {
                case API_Frame::AT_COMMAND_RESPONSE:

                    API_AT_CommandResponse* at_response;

                    cout << "It's an AT_COMMAND_RESPONSE" << endl;
                    frame = new API_AT_CommandResponse();
                    frame->parseFrame(message);

                    at_response = dynamic_cast<API_AT_CommandResponse*>(frame);

                    cout << "Command Status: ";
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

                                for(unsigned int i = 0; i < at_response->getParameterValue().length(); i++){

                                    if(i == 0)
                                        bandwith = ((unsigned char)at_response->getParameterValue()[i] * 0x100) + (unsigned char)at_response->getParameterValue()[i+1];

                                    else if(i != at_response->getParameterValue().length()-1)
                                        bandwith = (bandwith * 0x100) + (unsigned char)at_response->getParameterValue()[i+1];
                                }

                                cout << dec << API_AT_CommandResponse::BDParameterRange[bandwith] << endl;

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

                    delete at_response;
                    break;
                case API_Frame::RX_PACKET:
                    cout << "It's a RECEIVE PACKET" << endl;
                    frame = new API_Receive_Packet();
                    frame->parseFrame(message);
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
        else if (string(option).compare("send") == 0){

            API_AT_Command *at_frame = new API_AT_Command();
            string parameter = "";

            parameter.append(1, 0x1);

            cout << "Parameter size: " << parameter.size() << endl;

            at_frame->setATCommand("ND");
            at_frame->setParameterValue("");
            networkNodes.clear();
            /*delete nodeArray[0];
            delete nodeArray[1];*/
            /*at_frame->setATCommand("NR");
            at_frame->setParameterValue(parameter);*/
            at_frame->setFrameId(1);
            mc->sendMessage(at_frame->getFrame());

            delete at_frame;
        }

        else if (string(option).compare("quit") == 0){

            delete mc;
            break;
        }
     }
}

void useMC()
{
    unsigned char frameID = 0x0;
    char option[256];
    string message = "";

    ZB_MonitoringAndControl *mc = new ZB_MonitoringAndControl("/dev/ttyUSB0");

    mc->startMonitoring();
    sleep(2);
    frameID = mc->discoverNetworkNodes();

    while(true){

        cout << "Option: ";
        cin.getline(option, 256, '\n');

        if(string(option).compare("nodes") == 0){

            cout << "Nodes:" << endl;
            for(unsigned int i = 0; i < mc->getNodeList().size(); i++){

                cout << "\t(size: " << mc->getNodeList()[i]->getNodeIdent().size() << "): " <<  mc->getNodeList()[i]->getNodeIdent() << endl;

                cout << "Network Address (" << mc->getNodeList()[i]->getNetworkAddr().size() << " bytes):" <<  hex;

                for(unsigned int x = 0; x < mc->getNodeList()[i]->getNetworkAddr().size(); x++){

                    cout << (int)(unsigned char)mc->getNodeList()[i]->getNetworkAddr()[x];
                }
                cout << endl;
            }
        }

        else if(string(option).compare("sample ENDPOINT") == 0){

            string sample = "";
            API_IO_Sample io_sample;

            if (mc->accessNodeIOSample("END POINT", &io_sample)){

                cout << "DEBUG: IO Frame length: " << io_sample.getLength() << endl;
                sample = io_sample.getAnalogSamples().begin()->second;

                getTemperatureCelsius((unsigned char)sample[0]*0x100 + (unsigned char)sample[1]);

            }
            else
                cout << "No Sample found for END POINT node." << endl;
        }

        else if(string(option).compare("response") == 0){

            ZB_MonitoringAndControl::Resumed_AT_Response response;

            cout << "Got answer from AT Command with frameID: " << hex << (int)frameID << endl;
            if (mc->retrieveCommandsResponseBuffer(frameID, response)){

                cout << "Status: ";
                switch(response.commandStatus){

                    case API_AT_CommandResponse::OK:

                        cout << "OK" << endl;
                        break;

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

                        cout << "TRANSMISSION FAILURE" << endl;
                        break;

                    default:

                        cout << "UNKOWN STATUS" << endl;
                        break;
                }

                cout << "Parameter: ";
                switch(response.parameterValueType){

                    case ZB_MonitoringAndControl::UNSIGNED_INT:

                        cout << *(unsigned int*)response.parameterValue << endl;
                        break;

                    case ZB_MonitoringAndControl::STRING:

                        cout << (char*)response.parameterValue << endl;
                        break;

                    default:

                        cout << "ERROR: No type for parameter value!" << endl;
                        break;
                }
            }
        }

        else if (string(option).find("command") != string::npos){

            if(string(option).find("NI") != string::npos){

                mc->setNodeIdentifier(mc->int2Hex((unsigned int)0x479c), "ROUTER");
            }
        }

        else if (string(option).find("discover") != string::npos)

            mc->discoverNetworkNodes();

        else if (string(option).compare("quit") == 0){

            delete mc;
            break;
        }
    }
}


 int main()
 {
     //unitTesting();
     //useTXRX();
     useMC();

     return 0;
 }

