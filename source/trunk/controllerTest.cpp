/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <iostream>
#include <fstream>
#include <vector>

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
 
int main(int argc,char** argv)
{

	unsigned char buff[255] = {0};
	string message = "";
        struct termios tio;
        int serial_fd;
	int nBytes = 0;
	bool found_init = false;
 
	//bzero(&tio, sizeof(tio));
 
        /*memset(&tio,0,sizeof(tio));
        tio.c_iflag=ICRNL;
        tio.c_oflag=OCRNL;
        //tio.c_cflag=CS8|CLOCAL|CREAD;           // 8n1, see termios.h for more information
	tio.c_cflag |= (CLOCAL | CREAD);
	tio.c_cflag &= ~PARENB; // Mask the character size to 8 bits, no parity
	tio.c_cflag &= ~CSTOPB;
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |=  CS8;                              //Select 8 data bits
	tio.c_cflag &= ~CRTSCTS;
        tio.c_lflag=ICANON;*/
        //tio.c_lflag=0;
        /*tio.c_cc[VMIN]=1;
        tio.c_cc[VTIME]=5;*/
 
	cfmakeraw(&tio);
        cfsetospeed(&tio,B9600);            // 9600 baud
        cfsetispeed(&tio,B9600);            // 9600 baud

        serial_fd=open("/dev/ttyUSB0", O_RDWR);

	if (serial_fd < 0){
	
		cout << "Error while opening device..." << endl;
		exit(1);
	}      
 
        tcsetattr(serial_fd,TCSANOW,&tio);
        //tcsetattr(serial_fd,TCSADRAIN,&tio);


	buff[0] = 0x7E;	
	buff[1] = 0x00;	
	buff[2] = 0x04;	
	buff[3] = 0x08;	
	buff[4] = 0x01;	
	buff[5] = 0x49;	
	buff[6] = 0x44;	
	buff[7] = 0x69;
	buff[8] = '\n';


	cout << "Sending command to XBee..." << endl;
	nBytes = write(serial_fd, buff, 9);
	if (nBytes != 9){
		cout << "Error: Number of bytes written= " << nBytes << endl;
		exit(1);
	}
	cout << "Command sent..." << endl;

	for (unsigned int i = 0; i < sizeof(buff); i++)
	{
		buff[i] = '\0';
	}		


	cout << "Wating for response..." << endl;
	nBytes = 0;
        while (true)
        {
		nBytes = read(serial_fd, buff, 255);
		if(nBytes > 0){
			//cout << "nBytes read: " << nBytes << endl;
			for(int i = 0; i < nBytes; i++)
			{
				if(buff[i] == 0x7E && !found_init){
				
					found_init = true;

					message += buff[i];
					//cout << hex << (int)buff[i] << " ";
				}
				else if (buff[i] != 0x7E && found_init){
					message += buff[i];
					//cout << (int)buff[i] << " ";
				}

				else if (buff[i] == 0x7E && found_init){

					//cout << dec;
					cout << "Message: ";
					for(unsigned int x = 0; x < message.length(); x++){
						cout << hex << (int)(unsigned char)message[x] << " ";
					}
					cout << dec << endl;

					cout << "Processing Message:" << endl << "TYPE:\t";
					
					switch((unsigned char)message[3]){

						case 0x88:
							cout << "AT COMMAND RESPONSE" << endl;
							break;
						case 0x92:
							//double Vlm335A = 0.0;
							cout << "RX I/O DATA" << endl;

							getTemperatureCelsius((unsigned char)message[19]*0x100 + (unsigned char)message[20]);

							//c = (unsigned char)message[19]*0x100 + (unsigned char)message[20];

							//cout << "AD hex value: " << (int)((unsigned char)message[19]*0x100 + (unsigned char)message[20]) << endl;

							//Vlm335A = AD_output_to_mV((int)((unsigned char)message[19]*0x100 + (unsigned char)message[20])) * 3.0;

							//cout << "Temperature: " << kelvin2Celsius(get_lm335A_Sensor_KelvinValue(AD_output_to_mV((int)((unsigned char)message[19]*0x100 + (unsigned char)message[20])) * 3.0)) << endl;
							
							break;
						default:
							cout << "Unknown message type!" << endl;
							break;
					}
					message = "";
					message += buff[i];

					//cout << hex << (int)buff[i] << " ";
					
				}
			}
		}
		else if (nBytes < 0){
			cout << "Error while reading from serial port!" << endl;
			exit(1);
		}
		
        }
 
        close(serial_fd);
}
