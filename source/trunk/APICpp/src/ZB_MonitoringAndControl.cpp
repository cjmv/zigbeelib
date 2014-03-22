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
//#include <fstream> // DEBUG

using namespace std;

ZB_MonitoringAndControl* ZB_MonitoringAndControl::instance_ = 0;

// getInstance method
ZB_MonitoringAndControl* ZB_MonitoringAndControl::getInstance(string device, unsigned long max_sample_queue_size)
{
    if (instance_ == 0)
        instance_ = new ZB_MonitoringAndControl(device, max_sample_queue_size);

    return instance_;
}

// getInstance method
ZB_MonitoringAndControl* ZB_MonitoringAndControl::getInstance(ZB_Frame_TXRX::API_MODE api_mode, string device, unsigned long max_sample_queue_size)
{
    if (instance_ == 0)
        instance_ = new ZB_MonitoringAndControl(api_mode, device, max_sample_queue_size);

    return instance_;
}

// Default constructor
ZB_MonitoringAndControl::ZB_MonitoringAndControl(string device, unsigned long max_sample_queue_size):
Thread(), max_sample_queue_size_(max_sample_queue_size)
{
    //ctor
    if(device.compare("") != 0){
        txrx_ = new ZB_Frame_TXRX(device);
        frameId_ = 1;
        run_ = false;
        auto_update_nodes_ = true;
        sem_init(&available_samples_, 0, 0);
    }
        //return this;
    /*else
        return 0;*/
}

// Constructor
ZB_MonitoringAndControl::ZB_MonitoringAndControl(ZB_Frame_TXRX::API_MODE api_mode, string device, unsigned long max_sample_queue_size):
Thread(), max_sample_queue_size_(max_sample_queue_size)
{
    //ctor
    if(device.compare("") != 0){
        txrx_ = new ZB_Frame_TXRX(api_mode, device);
        frameId_ = 1;
        run_ = false;
        auto_update_nodes_ = true;
    }
        //return this;
    /*else
        return 0;*/
}

ZB_MonitoringAndControl::~ZB_MonitoringAndControl()
{
    //dtor
    stopMonitoring();
    delete txrx_;
}

/*ZB_MonitoringAndControl::ZB_MonitoringAndControl(const ZB_MonitoringAndControl& other)
{
    //copy ctor
}*/

ZB_MonitoringAndControl& ZB_MonitoringAndControl::operator=(const ZB_MonitoringAndControl& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

// retrieveCommandsResponseBuffer method
bool ZB_MonitoringAndControl::retrieveCommandsResponseBuffer(unsigned char frameID, Resumed_AT_Response& commandResponse)
{
    bool found = false;
    timespec timeout;
    sem_t *response_available = 0;
    map<unsigned char, pair<sem_t*,Resumed_AT_Response> >::iterator it;

    clock_gettime(CLOCK_REALTIME, &timeout);

    timeout.tv_sec += 30;


    /* NEW method implementation */
    lock();
    {
        it = commandsResponse_buffer_.find(frameID);
        if(it != commandsResponse_buffer_.end()){

            cout << "DEBUG -> Requesting for a already registered response..." << endl;
            //sem_timedwait(&available_samples_, &timeout);
            response_available = it->second.first;
            /*commandResponse = it->second.second;
            commandsResponse_buffer_.erase(it);
            found = true;*/
        }
        else{

            cout << "DEBUG -> Requesting for a NOT registered response..." << endl;
            Resumed_AT_Response response;
            response.commandStatus = API_AT_CommandResponse::UNKOWN_STATUS;
            response.parameterValueType = NO_TYPE;

            response_available = new sem_t;
            sem_init(response_available, 0, 0);
            commandsResponse_buffer_[frameID] = make_pair(response_available, response);
        }
    }
    unlock();

    // wait for semaphore.
    sem_timedwait(response_available, &timeout);
    lock();
    {
        it = commandsResponse_buffer_.find(frameID);
        if(it != commandsResponse_buffer_.end() && (it->second.second.commandStatus != API_AT_CommandResponse::UNKOWN_STATUS ||
                                                    it->second.second.parameterValueType != NO_TYPE)){

            commandResponse = it->second.second;
            sem_destroy(response_available);
            commandsResponse_buffer_.erase(it);
            found = true;
        }
    }
    unlock();
    /*   *********************** */



    /*lock();
    {
        map<unsigned char, Resumed_AT_Response>::iterator it = commandsResponse_buffer_.find(frameID);
        if(it != commandsResponse_buffer_.end()){

            sem_timedwait(&available_samples_, &timeout);
            commandResponse = it->second.second;
            commandsResponse_buffer_.erase(it);
            found = true;
        }
        else{
            sem_t response_available;
            Resumed_AT_Response response;
            sem_init(&response_available, 0, 0);
            commandsResponse_buffer_[frameId_] = make_pair(response_available, response);
        }
    }
    unlock();*/

    return found;
}

// retrieveRXPacket method
bool ZB_MonitoringAndControl::retrieveRXPacket(API_Receive_Packet** rx_packet)
{

    bool found = false;
    timespec timeout;

    clock_gettime(CLOCK_REALTIME, &timeout);

    timeout.tv_sec += 2;

    sem_timedwait(&available_samples_, &timeout);
    lock();
    {
        if(!rx_sample_queue_.empty()){
            *rx_packet = rx_sample_queue_.front();
            rx_sample_queue_.pop();
            found = true;
        }
    }
    unlock();

    return found;
}

// Discover network nodes method
unsigned char ZB_MonitoringAndControl::discoverNetworkNodes(string NI)
{
    //API_AT_Command *atCommand = new API_AT_Command(frameId_, "ND", NIorMY);
    API_AT_Command *atCommand = new API_AT_Command(frameId_, int2Hex((unsigned int)API_AT_Command::EXEC_CRE_NODE_DISCOVERY), NI);
    /*Resumed_AT_Response resumedResponse;
    resumedResponse.commandStatus = API_AT_CommandResponse::UNKOWN_STATUS;
    resumedResponse.parameterValueType = NO_TYPE;
    resumedResponse.parameterValue = 0;*/

    txrx_->sendMessage(atCommand->getFrame());

    delete atCommand;
    return incrementFrameID() - 0x1;
}

// setSleepMode method
unsigned char ZB_MonitoringAndControl::setSleepMode(string networkAddr, API_AT_Command::SleepMode sleepMode)
{
    API_AT_RemoteCommand *at_remoteCommand;
    string parameter = "";

    parameter.append(1, sleepMode);

    if(networkAddr.compare("") == 0){
        cout << "No node network address provided!" << endl;
        return 0x0;
    }
    else{

        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "SM", parameter);
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);
        txrx_->sendMessage(at_remoteCommand->getFrame());

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

// setNumberOfSleepPeriods method
unsigned char ZB_MonitoringAndControl::setNumberOfSleepPeriods(string networkAddr, unsigned int numberOfPeriods)
{
    /*Resumed_AT_Response resumedResponse;
    resumedResponse.commandStatus = API_AT_CommandResponse::UNKOWN_STATUS;
    resumedResponse.parameterValueType = NO_TYPE;
    resumedResponse.parameterValue = 0;*/

    if(networkAddr.compare("") == 0){

        API_AT_Command *atCommand;
        atCommand = new API_AT_Command(frameId_, "SN", int2Hex(numberOfPeriods));
        txrx_->sendMessage(atCommand->getFrame());

        /*lock();
        commandsResponse_buffer_[frameId_] = resumedResponse;
        unlock();*/

        delete atCommand;
        return incrementFrameID() - 0x1;
    }
    else{

        API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "SN", int2Hex(numberOfPeriods));
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);

        txrx_->sendMessage(at_remoteCommand->getFrame());

        /*lock();
        commandsResponse_buffer_[frameId_] = resumedResponse;
        unlock();*/

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

// setSleepPeriod method
unsigned char ZB_MonitoringAndControl::setSleepPeriod(string networkAddr, unsigned int sleepPeriod)
{
    if(networkAddr.compare("") == 0){

        API_AT_Command *atCommand;
        atCommand = new API_AT_Command(frameId_, "SP", int2Hex(sleepPeriod));
        txrx_->sendMessage(atCommand->getFrame());

        delete atCommand;
        return incrementFrameID() - 0x1;
    }
    else{

        API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "SP", int2Hex(sleepPeriod));
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);

        txrx_->sendMessage(at_remoteCommand->getFrame());

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

unsigned char ZB_MonitoringAndControl::setTimeBeforeSleep(string networkAddr, unsigned int timeBeforeSleep)
{
    if(networkAddr.compare("") == 0){

        API_AT_Command *atCommand;
        atCommand = new API_AT_Command(frameId_, "ST", int2Hex(timeBeforeSleep));
        txrx_->sendMessage(atCommand->getFrame());

        delete atCommand;
        return incrementFrameID() - 0x1;
    }
    else{

        API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "ST", int2Hex(timeBeforeSleep));
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);

        txrx_->sendMessage(at_remoteCommand->getFrame());

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

// setSleepOptions method
unsigned char ZB_MonitoringAndControl::setSleepOptions(string networkAddr, API_AT_Command::SleepOptions sleepOptions)
{
    API_AT_RemoteCommand *at_remoteCommand;
    string parameter = "";
    //Resumed_AT_Response resumedResponse;
    parameter.append(1, sleepOptions);
    /*resumedResponse.commandStatus = API_AT_CommandResponse::UNKOWN_STATUS;
    resumedResponse.parameterValueType = NO_TYPE;
    resumedResponse.parameterValue = 0;*/

    if(networkAddr.compare("") == 0){
        cout << "No node network address provided!" << endl;
        return 0x0;
    }
    else{

        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "SO", parameter);
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);
        txrx_->sendMessage(at_remoteCommand->getFrame());

        /*lock();
        commandsResponse_buffer_[frameId_] = resumedResponse; // Populating the map with a new frame ID regarding a AT command.
        unlock();*/

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

// setNodeIdentifier method
unsigned char ZB_MonitoringAndControl::setNodeIdentifier(string networkAddr, string nodeIdent)
{
    if(networkAddr.compare("") == 0){

        API_AT_Command *atCommand = new API_AT_Command(frameId_, "NI", nodeIdent);
        txrx_->sendMessage(atCommand->getFrame());

        delete atCommand;
        return incrementFrameID() - 0x1;
    }
    else{

        API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "NI", nodeIdent);
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);

        txrx_->sendMessage(at_remoteCommand->getFrame());

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

// endpointCyclicSleepConfiguration method
unsigned char ZB_MonitoringAndControl::endpointCyclicSleepConfiguration(string networkAddr, unsigned long sleepPeriod)
{
    return '\0';
}

unsigned char ZB_MonitoringAndControl::setIOSampleRate(string networkAddr, unsigned int ioSampleRate)
{
    if(networkAddr.compare("") == 0){

        return 0x0;
    }
    else{

        API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "IR", int2Hex(ioSampleRate));
        setRemoteAddressing(at_remoteCommand, networkAddr);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);

        txrx_->sendMessage(at_remoteCommand->getFrame());

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;
    }
}

// Send AT remote command method
unsigned char ZB_MonitoringAndControl::sendATCommand(string networkAddr,
                                                     string addr,
                                                     string atCommand,
                                                     string parameter,
                                                     bool slepping,
                                                     API_AT_RemoteCommand::RemoteCommandOption option,
                                                     string* generatedFrame)
{
    if(networkAddr.compare("") == 0 && addr.compare("") == 0){

        API_AT_Command *atLocalCommand = new API_AT_Command(frameId_, atCommand, parameter);
        txrx_->sendMessage(atLocalCommand->getFrame());

        delete atLocalCommand;
        return incrementFrameID() - 0x1;
    }
    else{

        API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, atCommand, parameter);
        setRemoteAddressing(at_remoteCommand, networkAddr, addr);
        at_remoteCommand->setRemoteCommandOption(option);

        if(!slepping)
        {
            if (generatedFrame  != 0){

                *generatedFrame = at_remoteCommand->getFrame();
                //txrx_->sendMessage(at_remoteCommand->getFrame());
                txrx_->sendMessage(*generatedFrame);
            }
            else{
                cout << "DEBUG: Generated frame for sending command has " << at_remoteCommand->getFrame().size() << " bytes." << endl;
                txrx_->sendMessage(at_remoteCommand->getFrame());
            }

            delete at_remoteCommand;
        }
        else
            qeuedAT_Commands_vector_.push_back(at_remoteCommand);

        return incrementFrameID() - 0x1;
    }
}

// writeChanges method
unsigned char ZB_MonitoringAndControl::writeChanges(string networkAddr)
{
    if(networkAddr.compare("") == 0){

        API_AT_Command *atCommand = new API_AT_Command(frameId_, "WR", "");

        txrx_->sendMessage(atCommand->getFrame());

        delete atCommand;
        return incrementFrameID() - 0x1;
    }
    else{

        return sendATCommand(networkAddr, "", "WR");
        /*API_AT_RemoteCommand *at_remoteCommand;
        at_remoteCommand = new API_AT_RemoteCommand(frameId_, "WR", "");
        setRemoteAddressing(networkAddr, at_remoteCommand);
        at_remoteCommand->setRemoteCommandOption(API_AT_RemoteCommand::APPLY_CH);

        txrx_->sendMessage(at_remoteCommand->getFrame());

        delete at_remoteCommand;
        return incrementFrameID() - 0x1;*/
    }
}

// start monitoring method
void ZB_MonitoringAndControl::startMonitoring()
{
    txrx_->start();
    run_ = true;
    openThread();
}

// stop monitoring method
void ZB_MonitoringAndControl::stopMonitoring()
{
    run_ = false;
    //txrx_->stop();
}

// job threaded method
void ZB_MonitoringAndControl::job()
{
    string message = "";
    API_Frame* frame;

    //cout << "DEBUG: Running MC Job..." << endl;

    while(run_)
    {
        txrx_->accessMessagePool(message);

        if(message.compare("") != 0){
            cout << "DEBUG: Message received: ";
            for(unsigned int x = 0; x < message.length(); x++){
                cout << hex << (int)(unsigned char)message[x] << " ";
            }
            cout << endl;
            switch((unsigned char)message[3])
            {

                case API_Frame::AT_COMMAND_RESPONSE:
                {
                    API_AT_CommandResponse* at_response;

                    cout << "It's an AT_COMMAND_RESPONSE" << endl;
                    frame = new API_AT_CommandResponse();
                    frame->parseFrame(message);

                    at_response = dynamic_cast<API_AT_CommandResponse*>(frame);

                    cout << "Command Status: ";
                    processATCommandStatus(at_response);

                    delete at_response;
                    break;
                }
                case API_Frame::MODEM_STATUS:

                    cout << "It's a MODEM STATUS" << endl;
                    break;

                case API_Frame::TX_STATUS:

                    cout << "It's TRANSMISSION STATUS" << endl;
                    break;

                case API_Frame::RX_PACKET:
                {
                    cout << "It's a RECEIVE PACKET" << endl;

                    bool newNodeFound = true;
                    API_Receive_Packet* rx_packet = new API_Receive_Packet();

                    rx_packet->parseFrame(message);
                    cout << "received data: " << rx_packet->getReceivedData() << endl;

                    // Send any queued messages saved for this io_sample node.
                    sendQeuedCommands(rx_packet->getSourceNetworkAddress());

                    if(auto_update_nodes_){

                        if(!nodeList_.empty()){

                            for(unsigned int i = 0; i < nodeList_.size(); i++){

                                if(nodeList_[i]->getNetworkAddr().compare(rx_packet->getSourceNetworkAddress()) == 0){

                                    newNodeFound = false;
                                    lock();

                                    if(rx_sample_queue_.size() >= max_sample_queue_size_){

                                        API_Receive_Packet *to_exclude = rx_sample_queue_.front();
                                        rx_sample_queue_.pop();
                                        delete to_exclude;
                                    }

                                    rx_sample_queue_.push(rx_packet);
                                    unlock();
                                    sem_post(&available_samples_);

                                    break;
                                }
                            }

                            // I'll have to check the NI from the sourceNetworkAddress and only then
                            // call the discoverNetworkNodes with the NI response as the parameter.
                            if(newNodeFound){

                                cout << "DEBUG: New node found!..." << endl;

                                /* Started new code for auto-discovery */
                                unsigned char frameId;
                                string* generatedFrame = new string("");
                                ZB_Node* node = new ZB_Node();

                                node->setSerialNumberHigh(rx_packet->getSourceAddress().substr(0, 4));
                                node->setSerialNumberLow(rx_packet->getSourceAddress().substr(4, 4));
                                node->setNetworkAddr(rx_packet->getSourceNetworkAddress());

                                // Add network address of node to network node list.
                                lock();
                                    nodeList_.push_back(node);
                                unlock();

                                // Send AT command ("AT NI") to newly found node
                                frameId = sendATCommand(rx_packet->getSourceNetworkAddress(),
                                                        rx_packet->getSourceAddress(),
                                                        "NI", "", false, API_AT_RemoteCommand::APPLY_CH, generatedFrame);

                                internalAT_frameID_vector_.push_back(make_pair(frameId, *generatedFrame));

                                delete generatedFrame;
                            }
                        }
                        else{
                            discoverNetworkNodes();
                            delete rx_packet;
                        }
                    }

                    break;
                }

                case API_Frame::EADDRESS_RX_PACKET:

                    cout << "It's EXPLICIT RX INDICATOR" << endl;
                    break;

                case API_Frame::IO_RX_SAMPLE:
                {

                    cout << "It's a IO SAMPLE" << endl;
                    bool newNodeFound = true;
                    API_IO_Sample* io_sample = new API_IO_Sample();

                    io_sample->parseFrame(message);

                    // Send any queued messages saved for this io_sample node.
                    sendQeuedCommands(io_sample->getSourceNetworkAddress());

                    if(auto_update_nodes_){

                        if(!nodeList_.empty()){

                            for(unsigned int i = 0; i < nodeList_.size(); i++){

                                if(nodeList_[i]->getNetworkAddr().compare(io_sample->getSourceNetworkAddress()) == 0){

                                    newNodeFound = false;
                                    lock();

                                    if(rx_sample_queue_.size() >= max_sample_queue_size_){

                                        API_Receive_Packet *to_exclude = rx_sample_queue_.front();
                                        rx_sample_queue_.pop();
                                        delete to_exclude;
                                    }

                                    rx_sample_queue_.push(io_sample);
                                    unlock();
                                    sem_post(&available_samples_);

                                    break;
                                }
                            }

                            // I'll have to check the NI from the sourceNetworkAddress and only then
                            // call the discoverNetworkNodes with the NI response as the parameter.
                            if(newNodeFound){

                                cout << "DEBUG: New node found!..." << endl;

                                /* Started new code for auto-discovery */
                                unsigned char frameId;
                                string* generatedFrame = new string("");
                                ZB_Node* node = new ZB_Node();

                                node->setSerialNumberHigh(io_sample->getSourceAddress().substr(0, 4));
                                node->setSerialNumberLow(io_sample->getSourceAddress().substr(4, 4));
                                node->setNetworkAddr(io_sample->getSourceNetworkAddress());

                                // Add network address of node to network node list.
                                lock();
                                    nodeList_.push_back(node);
                                unlock();

                                // Send AT command ("AT NI") to newly found node
                                frameId = sendATCommand(io_sample->getSourceNetworkAddress(),
                                                        io_sample->getSourceAddress(),
                                                        "NI", "", false, API_AT_RemoteCommand::APPLY_CH, generatedFrame);

                                internalAT_frameID_vector_.push_back(make_pair(frameId, *generatedFrame));

                                delete generatedFrame;
                            }
                        }
                        else{
                            discoverNetworkNodes();
                            delete io_sample;
                        }
                    }

                    break;
                }
                case API_Frame::NODE_IDENT:

                    cout << "It's NODE IDENT" << endl;
                    break;

                case API_Frame::AT_REMOTE_RESPONSE:
                {

                    API_AT_RemoteCommandResponse* at_response;

                    cout << "It's  a AT REMOTE RESPONSE" << endl;
                    frame = new API_AT_RemoteCommandResponse();
                    frame->parseFrame(message);

                    at_response = dynamic_cast<API_AT_RemoteCommandResponse*>(frame);

                    cout << "Command Status: ";
                    processATCommandStatus(at_response);

                    delete at_response;
                    break;
                }

                default:
                    cout << "Another message type..." << endl;
                    break;
            }
            message = "";
        }

        //sleep (1);
    }
}

// Process AT command status method
void ZB_MonitoringAndControl::processATCommandStatus(API_AT_CommandResponse* at_response)
{
    bool processInternally = false;
    string *generatedString = 0;

    // Check if the AT response received is to be processed internally or not.
    for (unsigned int i = 0; i < internalAT_frameID_vector_.size(); i++){

        if(at_response->getFrameId() == internalAT_frameID_vector_[i].first){
            processInternally = true;
            generatedString = new string(internalAT_frameID_vector_[i].second);
            internalAT_frameID_vector_.erase(internalAT_frameID_vector_.begin() + i);
            break;
        }
    }

    switch(at_response->getCommandStatus())
    {
        // If OK status has been received from an issued AT command, then the parameter value should be shown (if any).
        case API_AT_CommandResponse::OK:
        {

            //unsigned int bandwith = 0;
            API_AT_CommandResponse::ATCommands atCommand = API_AT_CommandResponse::UNKNOWN_COMMAND;
            cout << "OK" << endl;
            cout << "AT Command: " << at_response->getATCommand() << endl;


            /* Depending on the command issued, the parameter value should get different treatment. For this particular case
             * the AT command was BD, so BD parameter ranges apply.
             */
            atCommand = API_AT_CommandResponse::ATCommands(((unsigned char)at_response->getATCommand()[0]*0x100) + (unsigned  char)at_response->getATCommand()[1]);

            switch(atCommand)
            {
                case API_AT_CommandResponse::SERIAL_CRE_INTERFACE_DATA_RATE:
                {
                    Resumed_AT_Response resumedATResponse;
                    resumedATResponse.commandStatus = API_AT_CommandResponse::CommandStatus(at_response->getCommandStatus());
                    resumedATResponse.parameterValueType = UNSIGNED_INT;
                    resumedATResponse.parameterValue.ui_value = at_response->get_ATBD_Value();

                    lock();
                    {
                        map<unsigned char, pair<sem_t*, Resumed_AT_Response> >::iterator it = commandsResponse_buffer_.find(at_response->getFrameId());
                        if(it == commandsResponse_buffer_.end()){

                            sem_t *response_available = new sem_t;
                            sem_init(response_available, 0, 1);
                            commandsResponse_buffer_[at_response->getFrameId()] = make_pair(response_available, resumedATResponse);
                        }
                        else{

                            it->second.second = resumedATResponse;
                            sem_post(it->second.first);
                        }

                    }
                    unlock();

                    break;
                }

                case API_AT_CommandResponse::EXEC_CRE_NODE_DISCOVERY:
                {
                    updateNodeList(new ZB_Node(at_response->getParameterValue()));

                    break;
                }

                case API_AT_CommandResponse::ADDR_CRE_NODE_IDENTIFIER:
                {

                    if(processInternally)
                    {
                        internalAT_frameID_vector_.push_back(make_pair(discoverNetworkNodes(at_response->getParameterValue()), ""));

                        API_AT_RemoteCommandResponse* at_rmt_response = dynamic_cast<API_AT_RemoteCommandResponse*>(at_response);

                        lock();
                        {
                            for(unsigned int i = 0; i < nodeList_.size(); i++){

                                if (nodeList_[i]->getNetworkAddr().compare(at_rmt_response->getSourceNetworkAddress()) == 0)

                                    nodeList_[i]->setNodeIdent(at_response->getParameterValue());
                            }
                        }
                        unlock();
                    }
                    else
                    {
                        Resumed_AT_Response resumedATResponse;
                        resumedATResponse.commandStatus = API_AT_CommandResponse::CommandStatus(at_response->getCommandStatus());
                        resumedATResponse.parameterValueType = STRING;
                        resumedATResponse.s_value = at_response->getParameterValue();

                        lock();
                        {
                            map<unsigned char, pair<sem_t*, Resumed_AT_Response> >::iterator it = commandsResponse_buffer_.find(at_response->getFrameId());
                            if(it == commandsResponse_buffer_.end()){

                                sem_t *response_available = new sem_t;
                                sem_init(response_available, 0, 1);
                                commandsResponse_buffer_[at_response->getFrameId()] = make_pair(response_available, resumedATResponse);
                            }
                            else{
                                it->second.second = resumedATResponse;
                                sem_post(it->second.first);
                            }
                        }
                        unlock();

                        /*cout << "DEBUG: Received response: ADDR_CRE_NODE_IDENTIFIER" << endl
                        << "Value: " << resumedATResponse.s_value << endl;*/
                    }

                    break;
                }

                case API_AT_CommandResponse::IO_CRE_AD0:
                {
                    Resumed_AT_Response resumedATResponse;
                    resumedATResponse.commandStatus = API_AT_CommandResponse::CommandStatus(at_response->getCommandStatus());
                    resumedATResponse.parameterValueType = UNSIGNED_INT;
                    resumedATResponse.s_value = at_response->getParameterValue();

                    lock();
                    {
                        map<unsigned char, pair<sem_t*, Resumed_AT_Response> >::iterator it = commandsResponse_buffer_.find(at_response->getFrameId());
                        if(it == commandsResponse_buffer_.end()){

                            cout << "DEBUG -> Received a reponse message NOT registered..." << endl;
                            sem_t *response_available = new sem_t;
                            sem_init(response_available, 0, 1);
                            commandsResponse_buffer_[at_response->getFrameId()] = make_pair(response_available, resumedATResponse);
                        }
                        else{
                            cout << "DEBUG -> Received a response registered message..." << endl;
                            it->second.second = resumedATResponse;
                            sem_post(it->second.first);
                        }
                    }
                    unlock();
                }

                default:

                    cout << "Parameter value: ";
                    for(unsigned int i = 0; i < at_response->getParameterValue().length(); i++){

                        cout << hex << (int)(unsigned char)at_response->getParameterValue()[i];
                    }

                    break;
            }

            cout << endl;

            break;
        }
        case API_AT_CommandResponse::ERROR:
            cout << "ERROR" << endl;
            cout << "AT Command: " << at_response->getATCommand() << endl;
            break;

        case API_AT_CommandResponse::INVALID_COMMAND:
            cout << "INVALID COMMAND" << endl;
            cout << "AT Command: " << at_response->getATCommand() << endl;
            break;

        case API_AT_CommandResponse::INVALID_PARAMETER:
            cout << "INVALID PARAMETER" << endl;
            cout << "AT Command: " << at_response->getATCommand() << endl;
            cout << "AT parameter: " << at_response->getParameterValue() << endl;
            break;

        case API_AT_CommandResponse::TX_FAILURE:
        {
            //unsigned int bandwith = 0;
            API_AT_CommandResponse::ATCommands atCommand = API_AT_CommandResponse::UNKNOWN_COMMAND;
            cout << "TX FAILURE" << endl;
            cout << "AT Command: " << at_response->getATCommand() << endl;


            /* Depending on the command issued, the parameter value should get different treatment.
             */
            atCommand = API_AT_CommandResponse::ATCommands(((unsigned char)at_response->getATCommand()[0]*0x100) + (unsigned  char)at_response->getATCommand()[1]);

            switch(atCommand)
            {
                case API_AT_CommandResponse::SERIAL_CRE_INTERFACE_DATA_RATE:
                {
                    if(processInternally)
                    {
                        // Can't see why I would issue this AT command internally
                    }
                    else
                    {
                        Resumed_AT_Response resumedATResponse;
                        resumedATResponse.commandStatus = API_AT_CommandResponse::CommandStatus(at_response->getCommandStatus());
                        resumedATResponse.parameterValueType = STRING;
                        resumedATResponse.s_value = at_response->getParameterValue();

                        lock();
                        {
                            map<unsigned char, pair<sem_t*, Resumed_AT_Response> >::iterator it = commandsResponse_buffer_.find(at_response->getFrameId());
                            if(it == commandsResponse_buffer_.end()){

                                sem_t *response_available = new sem_t;
                                sem_init(response_available, 0, 1);
                                commandsResponse_buffer_[at_response->getFrameId()] = make_pair(response_available, resumedATResponse);
                            }
                            else{

                                it->second.second = resumedATResponse;
                                sem_post(it->second.first);
                            }

                        }
                        unlock();


                    }

                    break;
                }

                case API_AT_CommandResponse::EXEC_CRE_NODE_DISCOVERY:
                {
                    if(processInternally)
                    {
                        // Check if this AT ND command was parameterized.
                        // If it was, it means that this command was issued to a specific node NI.
                        if(at_response->getParameterValue().compare("") != 0){
                            // This command should be qeued.
                            internalAT_frameID_vector_.push_back(make_pair(discoverNetworkNodes(at_response->getParameterValue()), ""));
                        }
                    }
                    else
                    {
                        Resumed_AT_Response resumedATResponse;
                        resumedATResponse.commandStatus = API_AT_CommandResponse::CommandStatus(at_response->getCommandStatus());
                        resumedATResponse.parameterValueType = STRING;
                        resumedATResponse.s_value = at_response->getParameterValue();

                        lock();
                        {
                            map<unsigned char, pair<sem_t*, Resumed_AT_Response> >::iterator it = commandsResponse_buffer_.find(at_response->getFrameId());
                            if(it == commandsResponse_buffer_.end()){

                                sem_t *response_available = new sem_t;
                                sem_init(response_available, 0, 1);
                                commandsResponse_buffer_[at_response->getFrameId()] = make_pair(response_available, resumedATResponse);
                            }
                            else{

                                it->second.second = resumedATResponse;
                                sem_post(it->second.first);
                            }

                        }
                        unlock();
                    }

                    break;
                }

                case API_AT_CommandResponse::ADDR_CRE_NODE_IDENTIFIER:
                {

                    if(processInternally)
                    {
                        if (at_response->getFrameType() == API_Frame::AT_REMOTE_RESPONSE)
                        {
                            /*API_AT_RemoteCommandResponse *at_rmt_response = dynamic_cast<API_AT_RemoteCommandResponse*>(at_response);
                            internalAT_frameID_vector_.push_back(sendATCommand(at_rmt_response->getSourceNetworkAddress(),
                                                                               at_rmt_response->getSourceAddress(),
                                                                               "NI", "", true));*/
                            txrx_->sendMessage(*generatedString);
                            internalAT_frameID_vector_.push_back(make_pair(at_response->getFrameId(), *generatedString));
                            delete generatedString;
                        }
                        else if (at_response->getFrameType() == API_Frame::AT_COMMAND_RESPONSE)
                        {
                            internalAT_frameID_vector_.push_back(make_pair(sendATCommand("", "", "NI"), ""));
                        }
                    }
                    else
                    {
                        Resumed_AT_Response resumedATResponse;
                        resumedATResponse.commandStatus = API_AT_CommandResponse::CommandStatus(at_response->getCommandStatus());
                        resumedATResponse.parameterValueType = STRING;
                        resumedATResponse.s_value = at_response->getParameterValue();

                        lock();
                        {
                            map<unsigned char, pair<sem_t*, Resumed_AT_Response> >::iterator it = commandsResponse_buffer_.find(at_response->getFrameId());
                            if(it == commandsResponse_buffer_.end()){

                                sem_t *response_available = new sem_t;
                                sem_init(response_available, 0, 1);
                                commandsResponse_buffer_[at_response->getFrameId()] = make_pair(response_available, resumedATResponse);
                            }
                            else{

                                it->second.second = resumedATResponse;
                                sem_post(it->second.first);
                            }

                        }
                        unlock();

                        /*cout << "DEBUG: Received response: ADDR_CRE_NODE_IDENTIFIER" << endl
                        << "Value: " << resumedATResponse.s_value << endl;*/
                    }

                    break;
                }

                default:

                    cout << "Parameter value: ";
                    for(unsigned int i = 0; i < at_response->getParameterValue().length(); i++){

                        cout << hex << (int)(unsigned char)at_response->getParameterValue()[i];
                    }

                    break;
            }

            cout << endl;
            break;
        } // TX_FAILURE

        default:
            cout << "UNKNOWN" << endl;
            break;
    }
}

// incrementFrameID method
unsigned char ZB_MonitoringAndControl::incrementFrameID()
{
    if(frameId_ == 0xFF){
        frameId_ = 0x1;
        return 0xFF;
    }
    else{
        frameId_ += 0x1;
        return frameId_;
    }
}

// int2Hex method
string ZB_MonitoringAndControl::int2Hex(unsigned long number)
{
    string hexNumber = "", aux = "";

	while(number > 0){

		aux.append(1, (number & 0xFF));
		aux.append(hexNumber);
		hexNumber = aux;
		aux = "";
		number = number >> 8;
	}

	return hexNumber;
}

// updateNodeList method
void ZB_MonitoringAndControl::updateNodeList(ZB_Node* node)
{
    bool found = false, updated = false;

    lock();
    {
        for (unsigned int i = 0; i < nodeList_.size(); i++){

            if (nodeList_[i]->getNetworkAddr() == node->getNetworkAddr()){

                found = true;
                if(*nodeList_[i] == *node)
                    break;

                else{
                    nodeList_.erase(nodeList_.begin() + i);
                    nodeList_.push_back(node);
                    updated = true;
                    break;
                }
            }
        }

        if(!found)
            nodeList_.push_back(node);
        else if(found && !updated)
            delete node;
    }
    unlock();
}

// biCount method
unsigned int ZB_MonitoringAndControl::bitCount(unsigned int number)
{
    unsigned int count = 0;

	while(number > 0)
	{
		number = number >> 1;
		count++;
	}

	return count;
}

// send queued commands method
unsigned int ZB_MonitoringAndControl::sendQeuedCommands(string networkAddress)
{
    unsigned int numberOfMatches = 0;
    lock();
    {
        // Browse the qeued AT Commands vector
        for (vector<API_AT_Command*>::iterator it = qeuedAT_Commands_vector_.begin(); it != qeuedAT_Commands_vector_.end();){

            // If the frame type is a AT Remote command
            if ((*it)->getFrameType() == API_Frame::AT_REMOTE_COMMAND){

                API_AT_RemoteCommand *at_rmt_command = dynamic_cast<API_AT_RemoteCommand*>(*it);

                // If the network address matches the one on the AT command message
                if(networkAddress.compare(at_rmt_command->getDestinationNetworkAddress()) == 0){

                    numberOfMatches++; // Incremente the number of messages found
                    txrx_->sendMessage(at_rmt_command->getFrame()); // send the qeued AT command
                    it = qeuedAT_Commands_vector_.erase(it); // Remove the AT command from the vector.
                }
                else
                    it++;
            }
            else if ((*it)->getFrameType() == API_Frame::AT_COMMAND){
                it++;
            }
            else
                it++;
        }
    }
    unlock();

    return numberOfMatches;
}

// setRemoteAddressing method
bool ZB_MonitoringAndControl::setRemoteAddressing(API_AT_RemoteCommand* remoteCommand, string nodeIdent, string address)
{
    bool set = false;

    //cout << "DEBUG: " << remoteCommand->getATCommand() << endl;

    lock();
    {
        for(unsigned int i = 0; i < nodeList_.size(); i++){

            if(nodeList_[i]->getNodeIdent().compare(nodeIdent) == 0 ||
               nodeList_[i]->getNetworkAddr().compare(nodeIdent) == 0 ||
               string(nodeList_[i]->getSerialNumberHigh() + nodeList_[i]->getSerialNumberLow()).compare(address) == 0){

                 remoteCommand->setDestinationAddress(string(nodeList_[i]->getSerialNumberHigh() + nodeList_[i]->getSerialNumberLow()));
                 remoteCommand->setDestinationNetworkAddress(nodeList_[i]->getNetworkAddr());
                 set = true;
                 break;
            }
        }
    }
    unlock();

    // If current node ins't yet on the node list, use only the (valid?) network address
    if (!set){

        if(nodeIdent.size() == 2){

            remoteCommand->setDestinationNetworkAddress(nodeIdent);
        }
        else{

            remoteCommand->setDestinationNetworkAddress(int2Hex((unsigned int)0xfffe));
        }

        if(address.size() == 8){

            remoteCommand->setDestinationAddress(address);
        }
        else{
            string missing_bytes = "";
            while(missing_bytes.size() < 6){
                missing_bytes += (char)0;
            }
            remoteCommand->setDestinationAddress(missing_bytes + int2Hex((unsigned long)0x000000000000ffff));
        }

        set = true;
    }

    return set;
}
