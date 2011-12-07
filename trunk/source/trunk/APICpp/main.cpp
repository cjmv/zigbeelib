/*
 * File created to test the Library
 */

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

 #include "ZB_MonitoringAndControl.h"
 #include "API_IO_Sample.h"
 #include "API_AT_CommandResponse.h"

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
     ZB_MonitoringAndControl *mc = new ZB_MonitoringAndControl("/dev/ttyUSB0");
     API_Frame* frame;

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
                            cout << "OK" << endl;
                            cout << "AT Command: " << at_response->getATCommand() << endl;
                            cout << "Parameter value: ";


                            /* Depending on the command issued, the parameter value should get different treatment. For this particular case
                             * the AT command was BD, so BD parameter ranges apply.
                             */
                            if ((((unsigned  char)at_response->getATCommand()[0]*0x100) + (unsigned  char)at_response->getATCommand()[1]) == API_AT_CommandResponse::SERIAL_CRE_INTERFACE_DATA_RATE){

                                for(unsigned int i = 0; i < at_response->getParameterValue().length(); i++){

                                    if(i == 0)
                                        bandwith = ((unsigned char)at_response->getParameterValue()[i] * 0x100) + (unsigned char)at_response->getParameterValue()[i+1];

                                    else if(i != at_response->getParameterValue().length()-1)
                                        bandwith = (bandwith * 0x100) + (unsigned char)at_response->getParameterValue()[i+1];
                                }

                                cout << dec << API_AT_CommandResponse::BDParameterRange_[bandwith] << endl;

                            }
                            // Otherwise just print the parameter value
                            else{

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

                    getTemperatureCelsius((unsigned char)sample[0]*0x100 + (unsigned char)sample[1]);

                    delete io_sample;
                    break;
                }
                default:
                    cout << "Another message type..." << endl;
                    break;
            }
            message = "";
        }
        else if (string(option).compare("send") == 0){

            API_AT_Command *at_frame = new API_AT_Command();

            at_frame->setATCommand("ND");
            at_frame->setParameterValue("");
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

