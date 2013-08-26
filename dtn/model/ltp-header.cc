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

#include "ns3/abort.h"
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

	uint32_t sz = peer.GetSerializedSize();
	uint8_t data[sz];

	TagBuffer buff = TagBuffer(&data[0],&data[sz]);
	peer.Serialize(buff);


	buff.ReadU8(); // type
	uint8_t len = buff.ReadU8(); //len

	m_sessionOriginator.clear();

	for (uint32_t i = 0; i < len; i++)
	{
		m_sessionOriginator.push_back(buff.ReadU8() ); // Serialized data
	}
}

uint32_t
SessionId::GetU32SessionOriginator(void) const
{

	uint32_t data = 0;
	uint8_t offset = 24;

	NS_ABORT_MSG_UNLESS (m_sessionOriginator.size() > 0, "SessionId::Session originator contains no data");
	NS_ABORT_MSG_UNLESS (m_sessionOriginator.size() <= 4, "SessionId::Session originator will not fit into uint32_t");

	for (uint32_t i = 0; i< m_sessionOriginator.size() ; i++)
	{
		data = (m_sessionOriginator[i] << offset);
		offset-=8;
	}

	return data;
}


uint64_t
SessionId::GetU64SessionOriginator(void) const
{

	uint64_t data = 0;
	uint8_t offset = 56;

	NS_ABORT_MSG_UNLESS (m_sessionOriginator.size() > 0, "SessionId::Session originator contains no data");
	NS_ABORT_MSG_UNLESS (m_sessionOriginator.size() <= 8, "SessionId::Session originator will not fit into uint64t");

	for (uint32_t i = 0; i< m_sessionOriginator.size() ; i++)
	{
		data = (m_sessionOriginator[i] << offset);
		offset-=8;
	}

	return data;

}

void
SessionId::SetU32SessionOriginator(uint32_t data)
{

	uint8_t offset = 0;
	m_sessionOriginator.clear();

	for (uint32_t i = 0; i< 4 ; i++)
	{
		m_sessionOriginator.insert(m_sessionOriginator.begin(),(uint8_t) data >> offset);
		offset+=8;
	}

}


void
SessionId::SetU64SessionOriginator(uint64_t data)
{

	uint8_t offset = 0;
	m_sessionOriginator.clear();

	for (uint32_t i = 0; i< 8 ; i++)
	{
		m_sessionOriginator.insert(m_sessionOriginator.begin(),(uint8_t) data >> offset);
		offset+=8;
	}

}




LtpHeader::LtpHeader(SegmentType t, SessionId id):
		m_version(0),
		m_typeFlags(t),
		m_sessionId(id),
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

  uint32_t size = 2; // Control + Extensions Byte.

  LtpCodec codec;

  // Assume that both peers are running the protocol on the same layer (i.e both using IP addresses as session originator)
  if(m_sessionId.m_sessionOriginator.size() > 4)	// SDNV encoded Session Originator
  		size += codec.ReqSize(m_sessionId.GetU64SessionOriginator());
  	else
  		size += codec.ReqSize(m_sessionId.GetU32SessionOriginator());

  size += codec.ReqSize(m_sessionId.m_sessionNumber); // SDNV encoded Session Number

  // No extensions for now

  return size;
}
void
LtpHeader::Serialize (Buffer::Iterator start) const
{
	Buffer::Iterator i = start;
	LtpCodec codec;

	uint8_t control_byte = 0;

	control_byte = m_version << 4;
	control_byte += m_typeFlags;

	i.WriteU8(control_byte); // Write Control Byte

	LtpCodec::SDNV sdnv;

	if(m_sessionId.m_sessionOriginator.size() > 4)
		sdnv = codec.U64toSdnv(m_sessionId.GetU64SessionOriginator());
	else
		sdnv = codec.U32toSdnv(m_sessionId.GetU32SessionOriginator());

	for (uint32_t j=0; j < sdnv.GetN();j++)
	{
		i.WriteU8(sdnv.Get(j));	//Write SDNV encoded Session Originator
	}

	sdnv = codec.U32toSdnv(m_sessionId.m_sessionNumber);

	for (uint32_t j=0; j < sdnv.GetN();j++)
	{
		i.WriteU8(sdnv.Get(j)); //Write SDNV encoded Session Number
	}

	//No LTP extensions supported for now.

	uint8_t extensions_byte = 0;

	extensions_byte = m_hdrExtensionCnt << 4;
	extensions_byte += m_trailerExtensionCnt;

	i.WriteU8(extensions_byte); // Write Control Byte


}
uint32_t
LtpHeader::Deserialize (Buffer::Iterator start)
{
  // we can deserialize two bytes from the start of the buffer.
  // we read them in network byte order and store them
  // in host byte order.
  //m_data = start.ReadNtohU16 ();

   Buffer::Iterator i = start;
   uint32_t size = 0;

   uint8_t control_byte = i.ReadU8(); // Read Control Byte
   size++;

   m_version = control_byte >> 4;
   m_typeFlags = control_byte & 0x0F;

   LtpCodec::SDNV sdnv;
   LtpCodec codec;

   uint8_t read_byte = 0;

   do
   {
	  read_byte = i.ReadU8();
	  sdnv.PushBack(read_byte);
	  size++;

   } while ((read_byte >> 7) == 1); // Read SDNV encoded Session Originator, the last octet starts with 0, then stop

   if (sdnv.GetOriginalSize() < 32)
	   m_sessionId.SetU32SessionOriginator(codec.SdnvtoU32(sdnv));
   else
	   m_sessionId.SetU64SessionOriginator(codec.SdnvtoU64(sdnv));

   sdnv.Clear();

   do
   {
  	  read_byte = i.ReadU8();
  	  sdnv.PushBack(read_byte);
  	  size++;

   } while ((read_byte >> 7) == 1); // Read SDNV encoded Session Number, the last octet starts with 0, then stop

   m_sessionId.m_sessionNumber = codec.SdnvtoU32(sdnv);

   uint8_t extensions_byte = i.ReadU8();
   size++;

   m_hdrExtensionCnt = extensions_byte >> 4;
   m_trailerExtensionCnt = extensions_byte & 0x0F;

   //No LTP extensions supported for now.

   NS_ASSERT_MSG(size == GetSerializedSize(), "Ltp-Header deserialize error,  wrong size");

   return GetSerializedSize ();
}

}
