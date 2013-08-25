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

#ifndef LTP_CODEC_H
#define LTP_CODEC_H

#include "ns3/object.h"
#include "ns3/buffer.h"
#include "vector.h"

namespace ns3 {


class LtpCodec : public Object
{
public:
	class SDNV {
	public:
		inline SDNV() {}
		inline ~SDNV(){}

		void Add(uint8_t); // Values should be added from LSB to MSB

		uint8_t Get(uint32_t) const;
		uint32_t GetN(void) const;

		void Clear();

		void Print(std::ostream &os) const;

		bool operator== (SDNV const & o) const;

	private:
		std::vector<uint8_t> m_data;

	};

	static TypeId GetTypeId (void);

	LtpCodec();
	~LtpCodec();

	inline SDNV U8toSdnv (uint8_t data)   {	return InternalUtoSdnv( (uint64_t) data); }
	inline SDNV U16toSdnv (uint16_t data) { return InternalUtoSdnv( (uint64_t) data); }
	inline SDNV U32toSdnv (uint32_t data) { return InternalUtoSdnv( (uint64_t) data); }
	inline SDNV U64toSdnv (uint64_t data) { return InternalUtoSdnv(data); }

	inline uint8_t ReqSize(uint8_t data) { return InternalReqSize( (uint64_t) data,GetMaxSize(8)); }
	inline uint8_t ReqSize(uint16_t data){ return InternalReqSize( (uint64_t) data,GetMaxSize(16)); }
	inline uint8_t ReqSize(uint32_t data){ return InternalReqSize( (uint64_t) data, GetMaxSize(32));}
	inline uint8_t ReqSize(uint64_t data){ return InternalReqSize(data,GetMaxSize(64)); }


private:

	uint8_t GetMaxSize(uint8_t);
	uint8_t InternalReqSize(uint64_t,uint8_t);
	SDNV InternalUtoSdnv(uint64_t);


};

} // namespace ns3
#endif /* LTP_CODEC_H_ */
