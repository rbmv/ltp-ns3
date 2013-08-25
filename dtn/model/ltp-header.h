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

#ifndef LTP_HEADER_H
#define LTP_HEADER_H

#include "ns3/header.h"
#include "ns3/ltp-codec.h"
#include "ns3/address.h"
#include <vector>

namespace ns3 {

enum SegmentType
{
  LTPTYPE_RD  = 0,   //!< LTPTYPE_RD
  LTPTYPE_RD_CP  = 1,   //!< LTPTYPE_RD_CP
  LTPTYPE_RD_CP_EORP  = 2,   //!< LTPTYPE_RD_CP_EORP
  LTPTYPE_RD_CP_EORP_EOB = 3, //!< LTPTYPE_RD_CP_EORP_EOB
  LTPTYPE_GD = 4,
  LTPTYPE_GD_UF1 = 5,
  LTPTYPE_GD_UF2 = 6,
  LTPTYPE_GD_EOB = 7,
  LTPTYPE_RS = 8,
  LTPTYPE_RAS = 9,
  LTPTYPE_CTRL1 = 10,
  LTPTYPE_CTRL2 = 11,
  LTPTYPE_CS = 12,
  LTPTYPE_CAS = 13,
  LTPTYPE_CR = 14,
  LTPTYPE_CAR = 15,
};

enum ExtensionType {
	LTPEXT_AUTH = 0,
	LTPEXT_COOKIE = 1
};

class LtpExtension {
	public:
		uint8_t m_extType; // 1 byte
		uint32_t m_len;
		std::vector<uint8_t> m_value;
};

class SessionId{
	public:

		SessionId();
		~SessionId();

		SessionId(Address);

		uint32_t GetU32SessionOriginator(void) const;
		uint64_t GetU64SessionOriginator(void) const;

		std::vector<uint8_t> m_sessionOriginator;
		uint32_t m_sessionNumber;

private:
		void AddressToInteger(Address);
		uint32_t GenSessionNumber();

};

/**
 * \brief Packet header for LTP Segments
  \verbatim

  0     1     2     3     4     5     6     7     8
  +-----+-----+-----+-----+-----+-----+-----+-----+
  |     Version Number    |	  Segment Type Flags  |
  +-----+-----+-----+-----+-----+-----+-----+-----+
  |                                   	          |
  /                   Session ID                  \
  \                                               /
  |                                               |
  +-----+-----+-----+-----+-----+-----+-----+-----+
  | Header Extension Cnt. | Trailer Extension Cnt.|
  +-----+-----+-----+-----+-----+-----+-----+-----+
  |             Header Extensions                 |
  +-----+-----+-----+-----+-----+-----+-----+-----+

  \endverbatim
*/
class LtpHeader : public Header
{

public:
   /**
    * \brief Construct a null LTP segment
	*/
	LtpHeader ();
	LtpHeader (SegmentType, SessionId);
	~LtpHeader ();

	void SetType();

	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;

	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual void Serialize (Buffer::Iterator start) const;

	virtual uint32_t GetSerializedSize (void) const;
	virtual void Print (std::ostream &os) const;

private:

	// Control byte
	uint8_t m_version; // 4 bits
	uint8_t m_typeFlags; // 4 bits

	// SessionId n-bytes
	SessionId m_sessionId; // 8 bit multiple (total size undefined)

	// Extension Count Byte
	uint8_t m_hdrExtensionCnt; // 4 bits
	uint8_t m_trailerExtensionCnt; // 4 bits

	std::vector<LtpExtension> m_extensions; // TLV format, from 0 to 15

};

} // namespace ns3

#endif /* LTP_HEADER_H */
