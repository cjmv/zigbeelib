/*
 * File created to test the Library
 */

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "ZB_Frame_TXRX.h"
#include "ZB_Node.h"
#include "API_IO_Sample.h"
#include "API_AT_RemoteCommandResponse.h"

 using namespace std;

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


 int main()
 {
     char option[256];
     string message = "";
     ZB_Frame_TXRX *mc = new ZB_Frame_TXRX("/dev/ttyUSB0");
     API_Frame* frame;
     ZB_Node* node;
     vector<ZB_Node*> networkNodes;
     //ZB_Node* nodeArray[2] = {0};

     //mc->setDevice("/dev/ttyUSB0");

     mc->startMonitoring();


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


     return 0;
 }

