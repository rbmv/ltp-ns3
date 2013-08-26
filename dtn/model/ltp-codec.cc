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

#include "ns3/ltp-codec.h"
#include "ns3/abort.h"
#include "ns3/log.h"
#include <bitset>
#include <math.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LtpCodec");

LtpCodec::LtpCodec()
{
}

LtpCodec::~LtpCodec()
{
}

uint8_t
LtpCodec::GetMaxSize(uint8_t bits)
{
	return  (uint8_t) ceil( (double) bits/7 );
}

uint8_t
LtpCodec::InternalReqSize(uint64_t data, uint8_t maxsize)
{

	uint8_t maxSize = maxsize;
	uint32_t sz = 1;

	for (uint64_t bound = 0x80; bound <= data && sz < maxSize; bound = bound << 7) {
		sz++;
	}

	return sz;
}

LtpCodec::SDNV
LtpCodec::InternalUtoSdnv (uint64_t data) {

	NS_LOG_DEBUG("data: " << (std::bitset<64>) data);

	uint32_t sz = ReqSize(data);

	SDNV sdnv;
	uint8_t offset = 0;
	uint8_t mask = 0x7F;
	uint8_t msb = 0x00;

	for(uint32_t i=0;i<sz;i++)
	{
		uint8_t octet = ((data>>offset) & mask);
		sdnv.Add(octet | msb);
		offset+=7;
		msb = 0x80;
	}

	return sdnv;
}


uint64_t
LtpCodec::InternalSdnvtoU(LtpCodec::SDNV sdnv) {

	uint64_t data = 0;

	uint8_t offset = 0;
	uint8_t mask = 0x7F;

	for(uint32_t i = 0; i < sdnv.GetN(); i++) {

		uint8_t	octet = sdnv.Get(i);
		uint64_t data_aux = (octet &  mask);
		data += (data_aux << offset);
		offset+=7;
	}


	return data;
}

uint8_t
LtpCodec::SDNV::GetOriginalSize() {

	uint8_t size = (m_data.size()-1) * 7;

	uint8_t last_octet = m_data[m_data.size()-1];
	uint8_t mask = 0;

	for(uint32_t i = 0; i < 8; i++)
	{
		mask = (last_octet >> i) & 0x01;

		if (mask == 0)
		{
			size += i;
			break;
		}
	}
	return size;
}


void
LtpCodec::SDNV::Add(uint8_t octet)
{
	uint8_t msb;

	uint8_t msb_bit = octet >> 7;

	if(m_data.size()>0)
		msb = 0x01;
	else
		msb = 0x00;


	NS_ABORT_MSG_UNLESS (msb_bit == msb, "SDNV::Add(): SDNV misplaced octet");
	m_data.insert(m_data.begin(), octet);

}

void
LtpCodec::SDNV::PushBack(uint8_t octet)
{

	m_data.push_back(octet);

}


void
LtpCodec::SDNV::Print(std::ostream &os) const
{
	for (uint32_t i = 0; i<m_data.size();i++)
		os << (std::bitset<8>) m_data[i]  << " ";

	os << std::endl;
}

uint32_t
LtpCodec::SDNV::GetN() const
{
	return m_data.size();
}

uint8_t
LtpCodec::SDNV::Get(uint32_t i) const
{
	NS_ABORT_MSG_UNLESS (m_data.size () > i, "SDNV::Get(): Bad index");
	return m_data[i];
}

void
LtpCodec::SDNV::Clear()
{
	m_data.clear();
}


bool
LtpCodec::SDNV::operator== (LtpCodec::SDNV const & o) const
{
	if (GetN() == o.GetN())
	{
		for(uint32_t i=0; i< GetN() ;i++)
		{
			if(Get(i) != o.Get(i)) return false;
		}
		return true;
	}

	return false;
}

} //namespace ns3



