#include "stdafx.h"
#include "Protobuf.h"

using namespace rgs::protocol::protobuf;
using Error = rgs::Error;

Serialization::Serialization(int packetId, const google::protobuf::Message& message) :
	packetId_(packetId),
	message_(message) {}

Serialization::~Serialization()
{
	RowData::deallocate(rowData_);
}


rgs::protocol::RowData
Serialization::operator()()
{
	RowData::deallocate(rowData_);

	unsigned int bodySize = message_.ByteSize();

	//패킷 총길이만큼 배열을 만든다.
	rowData_ = RowData::allocate((sizeof(int) * 2) + bodySize);

	//배열에 시리얼라이징한 데이터를 넣는다.
	::memcpy(rowData_.data, &packetId_, sizeof(int));
	::memcpy(rowData_.data + sizeof(int), &bodySize, sizeof(int));

	message_.SerializeToArray(rowData_.data + sizeof(int) + sizeof(int), bodySize);

	return rowData_;
}

Deserialization::Deserialization(google::protobuf::Message& packet) : packet_(packet) {}

bool 
Deserialization::operator()(const rgs::protocol::RowData& rowData)
{
	//serialize by protobuf 
	if (packet_.ParseFromArray(rowData.data, rowData.size) == true)
	{
		return true;
	}

	return false;
}

int 
Protocol::readPacket(const rgs::protocol::RowData& rowData, rgs::protocol::Packet** packet, unsigned int& readBytes)const
{
	*packet = nullptr;

	readBytes = 0;
	unsigned int offset = 0;

	//read packetId 
	if (rowData.size < sizeof(unsigned int))
	{
		return 0;
	}
	
	unsigned int packetId = 0;
	::memcpy(&packetId, rowData.data, sizeof(unsigned int));

	offset += sizeof(unsigned int);

	if (packetId >= createPacketSize_)
	{
		return Error::ERROR_ROW_DATA;
	}

	auto createPacket = createPackets_[packetId];

	if (createPacket == nullptr)
	{
		return Error::ERROR_ROW_DATA;
	}

	//read packetSize
	if (rowData.size < offset + sizeof(unsigned int))
	{
		return 0;
	}

	unsigned int packetSize = 0;
	::memcpy(&packetSize, rowData.data + offset, sizeof(unsigned int));

	offset += sizeof(unsigned int);

	if (packetSize > rgs::SocketConfig::BUFFER_SIZE)
	{
		return Error::ERROR_ROW_DATA;
	}

	if (rowData.size < offset + packetSize)
	{
		return 0;
	}

	//read packet
	rgs::protocol::RowData rowPacket(rowData.data + offset, packetSize);
	
	auto readPacket = (*createPacket)();
	if (readPacket->deserialize(rowPacket) == false)
	{
		delete readPacket;
		return Error::ERROR_DESERIALIZE;
	}

	*packet = readPacket;

	offset += packetSize;
	readBytes = offset;

	return 0;
}

int Protocol::dispatchPacket(const rgs::protocol::RowData& rowData, std::shared_ptr<rgs::io::Session> session, unsigned int& readBytes)const
{
	readBytes = 0;
	unsigned int offset = 0;

	//read packetId 
	if (rowData.size < sizeof(unsigned int))
	{
		return 0;
	}

	unsigned int packetId = 0;
	::memcpy(&packetId, rowData.data, sizeof(unsigned int));

	offset += sizeof(unsigned int);

	if (packetId >= createPacketSize_)
	{
		return Error::ERROR_ROW_DATA;
	}

	auto packet = packets_[packetId];
	if (packet == nullptr)
	{
		return Error::ERROR_ROW_DATA;
	}

	//read packetSize
	if (rowData.size < offset + sizeof(unsigned int))
	{
		return 0;
	}

	unsigned int packetSize = 0;
	::memcpy(&packetSize, rowData.data + offset, sizeof(unsigned int));

	offset += sizeof(unsigned int);

	if (packetSize > rgs::SocketConfig::BUFFER_SIZE)
	{
		return Error::ERROR_ROW_DATA;
	}

	if (rowData.size < offset + packetSize)
	{
		return 0;
	}

	//read packet
	rgs::protocol::RowData rowPacket(rowData.data + offset, packetSize);

	if (packet->dispatch(session, rowPacket) == false)
	{
		return Error::ERROR_DESERIALIZE;
	}

	offset += packetSize;
	readBytes = offset;

	return 0;
}