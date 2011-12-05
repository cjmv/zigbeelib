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
     string option = "", message = "";
     ZB_MonitoringAndControl *mc = new ZB_MonitoringAndControl("/dev/ttyUSB0");
     API_Frame* frame;

     //mc->setDevice("/dev/ttyUSB0");

     mc->startMonitoring();


     while(true){

        cout << "Option: ";
        cin >> option;

        if (option.compare("length") == 0)
            cout << "Message pool size: " << mc->messagePoolSize() << endl;

        else if (option.compare("print") == 0){
            cout << "Message: " << endl;
            mc->accessMessagePool(message);
            for(unsigned int x = 0; x < message.length(); x++){
                cout << hex << (int)(unsigned char)message[x] << " ";
            }
            cout << endl;

            switch((unsigned char)message[3])
            {
                case API_Frame::AT_COMMAND_RESPONSE:
                    cout << "It's an AT_COMMAND_RESPONSE" << endl;
                    frame = new API_AT_CommandResponse();
                    frame->parseFrame(message);
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
        else if (option.compare("send") == 0){
            mc->sendMessage(message);
            message = "";
        }

        else if (option.compare("quit") == 0){

            delete mc;
            break;
        }
     }


     return 0;
 }

