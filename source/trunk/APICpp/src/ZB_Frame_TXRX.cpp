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
    api_mode_ = NON_ESCAPED;
    device_ = "";
    run_ = false;
}

// Constructor
ZB_Frame_TXRX::ZB_Frame_TXRX(string device): Thread(), device_(device)
{
    api_mode_ = NON_ESCAPED;
    run_ = false;
}

// Constructor
ZB_Frame_TXRX::ZB_Frame_TXRX(API_MODE api_mode, string device): Thread(), api_mode_(api_mode), device_(device)
{
    run_ = false;
}

// Copy constructor
ZB_Frame_TXRX::ZB_Frame_TXRX(const ZB_Frame_TXRX& other)
{
    //copy ctor
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
    unsigned int message_length = 0;
	bool found_init = false, found_size = false;
    unsigned char buff[255] = {0};
	string message = "";
    struct termios tio;

    cout << "DEBUG: Running TXRX job..." << endl;

    cfmakeraw(&tio);
    cfsetospeed(&tio,B9600);            // 9600 baud
    cfsetispeed(&tio,B9600);            // 9600 baud
    tio.c_cc[VMIN]=0; tio.c_cc[VTIME]=10; // Wait one second (10 * 0.1s) or 1 byte to return from read.

    serial_fd_ = open(device_.c_str(), O_RDWR); // device is normally something like "/dev/ttyUSB0"

	if (serial_fd_ < 0){

        cout << "Error while opening device \"" << device_ << "\"..." << endl
            << "errno = " << errno << "\t Description: " << strerror(errno) << endl;

        exit(1);
	}

	tcsetattr(serial_fd_,TCSANOW,&tio);

	if (serial_fd_ < 0){

        cout << "Error while setting device attributes... " << endl
            << "errno = " << errno << "\t Description: " << strerror(errno) << endl;

        exit(1);
    }

	//cout << "Wating for response..." << endl;
	nBytes = 0;
	while (run_)
	{
		nBytes = read(serial_fd_, buff, 255);
		if(nBytes > 0){
			//cout << "nBytes read: " << dec << nBytes << endl;
			for(int i = 0; i < nBytes; i++)
			{
			    if(!found_size && message.length() > 3){

			        found_size = true;

			        if(api_mode_ == ESCAPED){

			            string aux = message;
			            message_length = ((unsigned char)aux[1] << 8) + (unsigned char)aux[2];
			            message_length += 4;
			        }
			        else{
                        message_length = ((unsigned char)message[1] << 8) + (unsigned char)message[2];
                        message_length += 4;
			        }

			        //cout << "Incoming frame size: " << message_length << endl;
			    }

				if(buff[i] == 0x7E && !found_init){

					found_init = true;
					message += buff[i];
					//cout << hex << (int)buff[i] << " ";
				}
				else if (buff[i] != 0x7E && found_init){
					message += buff[i];
					//cout << (int)buff[i] << " ";
				}

				/*else if (buff[i] == 0x7E && found_init){

					cout << "Adding message to pool.";

					// If in ESCAPED mode, remove escape control characters from incoming frame.
					if(api_mode_ == ESCAPED)
                        removeEscapes(message);

					accessMessagePool(message);

					message = "";
					message += buff[i];
					found_size = false;

					//cout << hex << (int)buff[i] << " ";

				}*/
				// Check if the string retrieved until now,
				// has the expected frame length. If yes, it means the frame is complete.
				if(found_size && message.length() == message_length){

                    //cout << "Adding message to pool (by size)." << endl;
				    // If in ESCAPED mode, remove escape control characters from incoming frame.
					if(api_mode_ == ESCAPED)
                        removeEscapes(message);

					accessMessagePool(message);

					message = "";
					found_size = false;
					found_init = false;
					message_length = 0;

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

    // If in escaped mode, add escape control characters to message.
    if(api_mode_ == ESCAPED)
        addEscapes(message);

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

// removeEscapes method
void ZB_Frame_TXRX::removeEscapes(string& frame)
{
    cout << "Removing escape control characters..." << endl;

    for (unsigned int i = 1; i < frame.length(); i++){

        if((unsigned char)frame[i] == 0x7D){

            frame = frame.erase(i, 1);

            if(i < frame.length())
                frame[i] = (unsigned char)frame[i] ^ 0x20;
        }
    }
}

// addEscapes method
void ZB_Frame_TXRX::addEscapes(string& frame)
{
    cout << "Adding escape control characters..." << endl;

    for (unsigned int i=1; i < frame.length(); i++){

        // Special characters that need escape
        if ((unsigned char)frame[i] == 0x7E ||
            (unsigned char)frame[i] == 0x7D ||
            (unsigned char)frame[i] == 0x11 ||
            (unsigned char)frame[i] == 0x13){

                frame = frame.insert(i, 1, (unsigned char)0x7D);
                i++;
                frame[i] = (unsigned char)frame[i] ^ 0x20;
        }
    }
}
