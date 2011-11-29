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

 using namespace std;

 int main()
 {
     string option = "", message = "";
     ZB_MonitoringAndControl *mc = new ZB_MonitoringAndControl("/dev/ttyUSB0");

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
            message = "";
        }
        else if (option.compare("send") == 0)
            mc->sendMessage(message);

        else if (option.compare("quit") == 0){

            delete mc;
            break;
        }
     }


     return 0;
 }

