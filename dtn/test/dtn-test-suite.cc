/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

// Include a header file from your module to test.
#include "ns3/log.h"
#include "ns3/abort.h"

// An essential include is test.h
#include "ns3/test.h"

#include "ns3/ltp-codec.h"
#include "ns3/ltp-header.h"
#include "ns3/simulator.h"

#include "ns3/ipv4-address.h"
#include "ns3/mac48-address.h"

// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LtpCodecTest");

class LtpCodecTest : public TestCase
{
public:
  LtpCodecTest ();
  virtual ~LtpCodecTest ();

private:
  virtual void DoRun (void);

  void Set8BTests();
  void Set16BTests();
  void Set32BTests();
  void Set64BTests();

  template <class T>
  class TestVector {
  	  public:
	  	  uint8_t m_expectedSz;
	  	  LtpCodec::SDNV sdnv;
	  	  T m_data;
    };

    TestVectors< TestVector<uint8_t> > m_testU8Vectors;
    TestVectors< TestVector<uint16_t> > m_testU16Vectors;
    TestVectors< TestVector<uint32_t> > m_testU32Vectors;
    TestVectors< TestVector<uint64_t> > m_testU64Vectors;

};

LtpCodecTest::LtpCodecTest()
: TestCase ("Check correctness of methods in LTP Header"), m_testU8Vectors(), m_testU16Vectors(), m_testU32Vectors(),m_testU64Vectors()
{
}

LtpCodecTest::~LtpCodecTest()
{
}

void
LtpCodecTest::Set8BTests() {
	TestVector<uint8_t> testVector;

	testVector.m_data = 0x02;
	testVector.m_expectedSz = 1;
	testVector.sdnv.Add(0b00000010);
	m_testU8Vectors.Add(testVector);

	testVector.m_data = 0x7F;
	testVector.m_expectedSz = 1;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b01111111);
	m_testU8Vectors.Add(testVector);

	testVector.m_data = 0x80;
	testVector.m_expectedSz = 2;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b00000000);
	testVector.sdnv.Add(0b10000001);
	m_testU8Vectors.Add(testVector);

	testVector.m_data = 0xDC;
	testVector.m_expectedSz = 2;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b01011100);
	testVector.sdnv.Add(0b10000001);
	m_testU8Vectors.Add(testVector);
}

void
LtpCodecTest::Set16BTests()
{
	TestVector<uint16_t> testVector;

	testVector.m_data = 0x0ABC;
	testVector.m_expectedSz = 2;
	testVector.sdnv.Add(0b00111100);
	testVector.sdnv.Add(0b10010101);
	m_testU16Vectors.Add(testVector);

	testVector.m_data = 0x1234;
	testVector.m_expectedSz = 2;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b00110100);
	testVector.sdnv.Add(0b10100100);
	m_testU16Vectors.Add(testVector);

	testVector.m_data = 0x4234;
	testVector.m_expectedSz = 3;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b00110100);
	testVector.sdnv.Add(0b10000100);
	testVector.sdnv.Add(0b10000001);
	m_testU16Vectors.Add(testVector);

	testVector.m_data = 0xFFFF;
	testVector.m_expectedSz = 3;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b01111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b10000011);
	m_testU16Vectors.Add(testVector);
}

void
LtpCodecTest::Set32BTests()
{
	TestVector<uint32_t> testVector;

	testVector.m_data = 0x00000ABC;
	testVector.m_expectedSz = 2;
	testVector.sdnv.Add(0b00111100);
	testVector.sdnv.Add(0b10010101);
	m_testU32Vectors.Add(testVector);

	testVector.m_data = 0x0FFFFFFF;
	testVector.m_expectedSz = 4;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b01111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	m_testU32Vectors.Add(testVector);

	testVector.m_data = 0x12345678;
	testVector.m_expectedSz = 5;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b01111000);
	testVector.sdnv.Add(0b10101100);
	testVector.sdnv.Add(0b11010001);
	testVector.sdnv.Add(0b10010001);
	testVector.sdnv.Add(0b10000001);
	m_testU32Vectors.Add(testVector);

	testVector.m_data = 0x80000000;
	testVector.m_expectedSz = 5;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b00000000);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b10001000);
	m_testU32Vectors.Add(testVector);
}

void
LtpCodecTest::Set64BTests()
{
	TestVector<uint64_t> testVector;

	testVector.m_data = 0x00000ABC00000ABCLLU;
	testVector.sdnv.Add(0b00111100);
	testVector.sdnv.Add(0b10010101);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b11000000);
	testVector.sdnv.Add(0b11010111);
	testVector.sdnv.Add(0b10000010);
	testVector.m_expectedSz = 7;
	m_testU64Vectors.Add(testVector);

	testVector.m_data = 0x0002FABC00000ABCLLU;
	testVector.m_expectedSz = 8;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b00111100);
	testVector.sdnv.Add(0b10010101);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b11000000);
	testVector.sdnv.Add(0b11010111);
	testVector.sdnv.Add(0b10111110);
	testVector.sdnv.Add(0b10000001);
	m_testU64Vectors.Add(testVector);

	testVector.m_data = 0x01FFFABC00000ABCLLU;
	testVector.m_expectedSz = 9;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b00111100);
	testVector.sdnv.Add(0b10010101);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b10000000);
	testVector.sdnv.Add(0b11000000);
	testVector.sdnv.Add(0b11010111);
	testVector.sdnv.Add(0b11111110);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b10000001);
	m_testU64Vectors.Add(testVector);


	testVector.m_data = 0xFFFFFFFFFFFFFFFFLLU;
	testVector.m_expectedSz = 10;
	testVector.sdnv.Clear();
	testVector.sdnv.Add(0b01111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b11111111);
	testVector.sdnv.Add(0b10000001);
	m_testU64Vectors.Add(testVector);

}


void
LtpCodecTest::DoRun (void)
{
	LtpCodec codec;

	Set8BTests();
	Set16BTests();
	Set32BTests();
	Set64BTests();

	for (uint32_t i = 0; i < m_testU8Vectors.GetN (); ++i)
		{
		  TestVector<uint8_t> testVector = m_testU8Vectors.Get (i);
		  NS_TEST_EXPECT_MSG_EQ(codec.ReqSize(testVector.m_data),testVector.m_expectedSz,"sdnv size calculation failed");
		  NS_TEST_EXPECT_MSG_EQ((codec.U8toSdnv(testVector.m_data) == testVector.sdnv),true,"sdnv calculation failed");
		}

	for (uint32_t i = 0; i < m_testU16Vectors.GetN (); ++i)
	    {
		  TestVector<uint16_t> testVector = m_testU16Vectors.Get (i);
		  NS_TEST_EXPECT_MSG_EQ(codec.ReqSize(testVector.m_data),testVector.m_expectedSz,"sdnv size calculation failed");
		  NS_TEST_EXPECT_MSG_EQ((codec.U16toSdnv(testVector.m_data) == testVector.sdnv),true,"sdnv calculation failed");
	    }

	for (uint32_t i = 0; i < m_testU32Vectors.GetN (); ++i)
		{
		  TestVector<uint32_t> testVector = m_testU32Vectors.Get (i);
		  NS_TEST_EXPECT_MSG_EQ(codec.ReqSize(testVector.m_data),testVector.m_expectedSz,"sdnv size calculation failed");
		  NS_TEST_EXPECT_MSG_EQ((codec.U32toSdnv(testVector.m_data) == testVector.sdnv),true,"sdnv calculation failed");
		}
	for (uint64_t i = 0; i < m_testU64Vectors.GetN (); ++i)
		{
		  TestVector<uint64_t> testVector = m_testU64Vectors.Get (i);
		  NS_TEST_EXPECT_MSG_EQ(codec.ReqSize(testVector.m_data),testVector.m_expectedSz,"sdnv size calculation failed");
		  NS_TEST_EXPECT_MSG_EQ((codec.U64toSdnv(testVector.m_data) == testVector.sdnv),true,"sdnv calculation failed");
		}
}


class SessionIdTest : public TestCase
{
public:
  SessionIdTest ();
  virtual ~SessionIdTest ();

private:
  virtual void DoRun (void);

  void SessionOriginatorTest();

  class TestVector {
  	  public:
	  	  uint8_t m_addrType;
	  	  std::string m_peerAddress;
	  	  std::vector<uint8_t> m_serializedBytes;
    };

  TestVectors< TestVector> m_testsSessionId;
};

SessionIdTest::SessionIdTest()
: TestCase ("Check correctness of methods in LTP Header")
{
}

SessionIdTest::~SessionIdTest()
{
}

void
SessionIdTest::SessionOriginatorTest() {

	TestVector testVector;

	testVector.m_addrType = 2;
	testVector.m_peerAddress = "192.168.1.1";
	testVector.m_serializedBytes.push_back(192);
	testVector.m_serializedBytes.push_back(168);
	testVector.m_serializedBytes.push_back(1);
	testVector.m_serializedBytes.push_back(1);
	m_testsSessionId.Add(testVector);

	testVector.m_addrType = 3;
	testVector.m_peerAddress = "12:34:56:78:90:12";
	testVector.m_serializedBytes.clear();
	testVector.m_serializedBytes.push_back(0x12);
	testVector.m_serializedBytes.push_back(0x34);
	testVector.m_serializedBytes.push_back(0x56);
	testVector.m_serializedBytes.push_back(0x78);
	testVector.m_serializedBytes.push_back(0x90);
	testVector.m_serializedBytes.push_back(0x12);
	m_testsSessionId.Add(testVector);

}

void
SessionIdTest::DoRun (void)
{
	SessionId id;

	SessionOriginatorTest();
	Address addr;

	for(uint32_t i = 0; i < m_testsSessionId.GetN();i++) {

		  TestVector testVector = m_testsSessionId.Get (i);

		  if (testVector.m_addrType == 2)
				 addr =  Ipv4Address(testVector.m_peerAddress.c_str());
		  else if (testVector.m_addrType == 3)
				 addr = Mac48Address (testVector.m_peerAddress.c_str());

		  id = SessionId(addr);

		  NS_TEST_EXPECT_MSG_EQ(testVector.m_serializedBytes.size(),id.m_sessionOriginator.size(),"sessionId session originator wrong size");

		  NS_TEST_EXPECT_MSG_EQ( (testVector.m_serializedBytes == id.m_sessionOriginator), true, "sessionId session originator wrong serialitzation");

	}
}

// This is an example TestCase.
class DtnTestCase1 : public TestCase
{
public:
  DtnTestCase1 ();
  virtual ~DtnTestCase1 ();

private:
  virtual void DoRun (void);
};

// Add some help text to this case to describe what it is intended to test
DtnTestCase1::DtnTestCase1 ()
  : TestCase ("Dtn test case (does nothing)")
{
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
DtnTestCase1::~DtnTestCase1 ()
{
}

//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
DtnTestCase1::DoRun (void)
{
  // A wide variety of test macros are available in src/core/test.h
  NS_TEST_ASSERT_MSG_EQ (true, true, "true doesn't equal true for some reason");
  // Use this one for floating point comparisons
  NS_TEST_ASSERT_MSG_EQ_TOL (0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run.  Typically, only the constructor for
// this class must be defined
//
class DtnTestSuite : public TestSuite
{
public:
  DtnTestSuite ();
};

DtnTestSuite::DtnTestSuite ()
  : TestSuite ("dtn", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
 // AddTestCase (new DtnTestCase1, TestCase::QUICK);
  AddTestCase (new SessionIdTest,TestCase::QUICK);
  AddTestCase (new LtpCodecTest,TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static DtnTestSuite dtnTestSuite;

