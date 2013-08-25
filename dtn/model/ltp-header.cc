/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 COPYRIGHTHOLDER
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Rubén Martínez <rmartinez@deic.uab.cat>
 */

#include "ltp-header.h"
#include "ns3/random-variable.h"
#include "ns3/tag-buffer.h"
#include <bitset>

namespace ns3 {

SessionId::SessionId() :
		m_sessionOriginator(),
		m_sessionNumber(0)
{

}

SessionId::~SessionId()
{

}

SessionId::SessionId(Address peer)
{
	AddressToInteger(peer);
	m_sessionNumber = GenSessionNumber();
}

uint32_t
SessionId::GenSessionNumber() {
	UniformVariable rand;
	return rand.GetInteger(0x0FFFFFFF,0xFFFFFFFF);
}

void
SessionId::AddressToInteger(Address peer) {
	/*
	byte[] addrbuf=addr.getAddress();
		long val=(addrbuf[0] & 0xFF);
		for (int i=1; i<addrbuf.length; i++)
			val=(val<<8)+(addrbuf[i] & 0xFF);
		return val;*/

	uint32_t sz = peer.GetSerializedSize();
	uint8_t data[sz];

//	std::cout << "Add to in " << sz << std::endl;

	TagBuffer buff = TagBuffer(&data[0],&data[sz]);
	peer.Serialize(buff);


	uint8_t type = buff.ReadU8(); // type
	uint8_t len = buff.ReadU8(); //len

	std::cout << (std::bitset<8>) type << std::endl;

	m_sessionOriginator.clear();


	for (uint32_t i = 0; i < len; i++)
	{
		m_sessionOriginator.push_back(buff.ReadU8() ); // Serialized data
	}
/*
	for(uint32_t i = 0; i < m_sessionOriginator.size(); i++)
	{
		std::cout << (std::bitset<8>) m_sessionOriginator[i] << std::endl;
	}*/



}

LtpHeader::LtpHeader(SegmentType t):
		m_version(0),
		m_typeFlags(t),
		m_sessionId(),
		m_hdrExtensionCnt(0),
		m_trailerExtensionCnt(0),
		m_extensions()
{

}
LtpHeader::LtpHeader ():
		m_version(0),
		m_typeFlags(0),
		m_sessionId(),
		m_hdrExtensionCnt(0),
		m_trailerExtensionCnt(0),
		m_extensions()
{
}

LtpHeader::~LtpHeader()
{
}

TypeId
LtpHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LtpHeader")
    .SetParent<Header> ()
    .AddConstructor<LtpHeader> ()
  ;
  return tid;
}

TypeId
LtpHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LtpHeader::Print (std::ostream &os) const
{
  // This method is invoked by the packet printing
  // routines to print the content of my header.
  os << "data=";
}

uint32_t
LtpHeader::GetSerializedSize (void) const
{
  // we reserve 2 bytes for our header.
  return 2;
}
void
LtpHeader::Serialize (Buffer::Iterator start) const
{
  // we can serialize two bytes at the start of the buffer.
  // we write them in network byte order.
  //start.WriteHtonU16 (m_data);
	Buffer::Iterator i = start;

	uint8_t control_byte;


	  i.WriteHtonU16 (m_sourcePort);
	  i.WriteHtonU16 (m_destinationPort);
	  i.WriteHtonU16 (start.GetSize ());
	  i.WriteU16 (0);

	  if (m_calcChecksum)
	    {
	      uint16_t headerChecksum = CalculateHeaderChecksum (start.GetSize ());
	      i = start;
	      uint16_t checksum = i.CalculateIpChecksum (start.GetSize (), headerChecksum);

	      i = start;
	      i.Next (6);
	      i.WriteU16 (checksum);
	    }


}
uint32_t
LtpHeader::Deserialize (Buffer::Iterator start)
{
  // we can deserialize two bytes from the start of the buffer.
  // we read them in network byte order and store them
  // in host byte order.
  //m_data = start.ReadNtohU16 ();

  // we return the number of bytes effectively read.
  return 2;
}

}
