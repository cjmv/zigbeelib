/*
 * Implementation of the API_AT_CommandTest class.
 */

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "API_AT_CommandTest.h"

using namespace std;

// Default constructor
API_AT_CommandTest::API_AT_CommandTest()
{
    //ctor
}

// Default destructor
API_AT_CommandTest::~API_AT_CommandTest()
{
    //dtor
}

// setUp method
void API_AT_CommandTest::setUp()
{

}

// tearDown method
void API_AT_CommandTest::tearDown()
{

}

// Implementing test for the API_AT_Command constructor
void API_AT_CommandTest::testConstructor()
{
    // set up
    string command = "NI";
    unsigned char frameID = 0x1;
    string parameterValue = "NODE NAME";

    // process
    API_AT_Command atCommand(frameID, command, parameterValue);

    // check
    CPPUNIT_ASSERT_EQUAL(frameID, atCommand.getFrameId());
    CPPUNIT_ASSERT_EQUAL(command, atCommand.getATCommand());
    CPPUNIT_ASSERT_EQUAL(parameterValue, atCommand.getParameterValue());

}

// Implementing test for the getter and setter method of API_AT_Command class
void API_AT_CommandTest::testGetSet()
{

}

// Implementing test for the getFrame method of API_AT_Command class
void API_AT_CommandTest::testGetFrame()
{

}
