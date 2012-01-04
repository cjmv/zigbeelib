///
/// Definition of ZB_Frame_TXRX class.
///
/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "ZB_Frame_TXRX.h"

using namespace std;


// Default constructor
ZB_Frame_TXRX::ZB_Frame_TXRX(): Thread()
{
    run_ = false;
}

// Copy constructor
ZB_Frame_TXRX::ZB_Frame_TXRX(const ZB_Frame_TXRX& other)
{
    //copy ctor
}

// Constructor
ZB_Frame_TXRX::ZB_Frame_TXRX(string device): Thread(), device_(device)
{
    run_ = false;
}

// Default destructor
ZB_Frame_TXRX::~ZB_Frame_TXRX()
{
    //dtor
    stop(); // set run_ flag to false, this will make the reading cycle to break;
    close(serial_fd_); // close serial file descriptor.
}

ZB_Frame_TXRX& ZB_Frame_TXRX::operator=(const ZB_Frame_TXRX& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

// startMonitoring method
void ZB_Frame_TXRX::start()
{
    run_ = true;
    openThread();

}

// stopMonitoring method
void ZB_Frame_TXRX::stop()
{
    run_ = false;

    // I probably should check here if thread is really killed or not.
}

// accessMessagePool method.
void ZB_Frame_TXRX::accessMessagePool(string& message)
{
    lock(); // acquire lock
    {
        if (message.compare("") == 0){ // check if the parameter message value is empty.
            // Since message is empty, than this method call has the purpose to acquire a message from the message pool.
            if(!messagePool_.empty()){

                message = messagePool_.front();
                messagePool_.erase(messagePool_.begin());
            }
        }
        else
            messagePool_.push_back(message); // message was not empty, thus this call has the purpose to add one more message to the pool.
    }
    unlock();
}

// Thread method
void ZB_Frame_TXRX::job()
{
    int nBytes = 0;
	bool found_init = false;
    unsigned char buff[255] = {0};
	string message = "";
    struct termios tio;

    cout << "DEBUG: Running TXRX job..." << endl;

    cfmakeraw(&tio);
    cfsetospeed(&tio,B9600);            // 9600 baud
    cfsetispeed(&tio,B9600);            // 9600 baud
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    serial_fd_ = open(device_.c_str(), O_RDWR); // device is normally somethind like "/dev/ttyUSB0"

	if (serial_fd_ < 0){

		cout << "Error while opening device..." << endl;
		exit(1);
	}

	tcsetattr(serial_fd_,TCSANOW,&tio);
	//tcsetattr(serial_fd,TCSADRAIN,&tio);


	/*for (unsigned int i = 0; i < sizeof(buff); i++)
	{
		buff[i] = '\0';
	}*/


	//cout << "Wating for response..." << endl;
	nBytes = 0;
	while (run_)
	{
		nBytes = read(serial_fd_, buff, 255);
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

					//cout << "Adding message to pool:";
					accessMessagePool(message);
					/*for(unsigned int x = 0; x < message.length(); x++){
						cout << hex << (int)(unsigned char)message[x] << " ";
					}*/
					/*cout << dec << endl;

					cout << "Processing Message:" << endl << "TYPE:\t";


					switch((unsigned char)message[3]){

						case 0x88:
							cout << "AT COMMAND RESPONSE" << endl;
							break;
						case 0x92:
							//double Vlm335A = 0.0;
							cout << "RX I/O DATA" << endl;

							getTemperatureCelsius((unsigned char)message[19]*0x100 + (unsigned char)message[20]);

							break;
						default:
							cout << "Unknown message type!" << endl;
							break;
					}*/
					message = "";
					message += buff[i];

					//cout << hex << (int)buff[i] << " ";

				}
			}
		}
		else if (nBytes < 0)
			cout << "Error while reading from serial port!" << endl;
    }

    //close(serial_fd);

}

// sendMessage method
void ZB_Frame_TXRX::sendMessage(string message)
{
    unsigned char* buff = new unsigned char[message.length()];
    unsigned int nBytes = 0;

    cout << "Sending message: " << hex;
    for(unsigned int i = 0; i < message.length(); i++){
        buff[i] = (unsigned char)message[i];
        cout << (int)buff[i] << " ";
    }
    cout << endl;

    // Implement here
    /*buff[0] = 0x7E;
	buff[1] = 0x00;
	buff[2] = 0x04;
	buff[3] = 0x08;
	buff[4] = 0x01;
	buff[5] = 0x49;
	buff[6] = 0x44;
	buff[7] = 0x69;
	buff[8] = '\n';*/


	cout << "Sending command to XBee..." << endl;
	nBytes = write(serial_fd_, buff, message.length());

	if (nBytes != message.length()){
		cout << "Error: Number of bytes written= " << nBytes << endl;
		exit(1);
	}
	cout << "Command sent..." << endl;
}
