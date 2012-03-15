/*
 * Implementation of the API_FrameTest class.
 */

/*
 * $Author$
 * $Id$
 * $Date$
 * $Revision$
 * */

#include "API_FrameTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(API_FrameTest);

// Default Constructor
API_FrameTest::API_FrameTest()
{
    //ctor
}

// Default destructor
API_FrameTest::~API_FrameTest()
{
    //dtor
}

// setUp method
void API_FrameTest::setUp()
{

}

// tearDown method
void API_FrameTest::tearDown()
{

}

// Implemening test to the API_Frame constructor
void API_FrameTest::testConstructor()
{
    // set up
    unsigned char checksum = 0x1;
    API_Frame::FrameType frameType = API_Frame::AT_COMMAND;
    unsigned int length = 10;

    // process
    API_FrameChild frame(length, frameType, checksum);

    // check
    CPPUNIT_ASSERT_EQUAL(length, frame.getLength());
    CPPUNIT_ASSERT_EQUAL(frameType, frame.getFrameType());
    CPPUNIT_ASSERT_EQUAL(checksum, frame.getChecksum());
}

// Implementing test for the getter and setter methods of API_Frame.
void API_FrameTest::testGetSet()
{
    // set up
    unsigned char checksum = 0x1, startDelimiter = 0x7E;
    API_Frame::FrameType frameType = API_Frame::AT_COMMAND;
    unsigned int length = 10;

    // process
    API_FrameChild frame;
    frame.setChecksum(checksum);
    frame.setFrameType(frameType);
    frame.setLength(length);

    CPPUNIT_ASSERT_EQUAL(length, frame.getLength());
    CPPUNIT_ASSERT_EQUAL(frameType, frame.getFrameType());
    CPPUNIT_ASSERT_EQUAL(checksum, frame.getChecksum());
    CPPUNIT_ASSERT_EQUAL(startDelimiter, frame.getStartDelimiter());
}
