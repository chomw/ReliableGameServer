#include "stdafx.h"
#include "JsonProtocol.h"

using namespace rgs::protocol::json;
using Error = rgs::Error;

Packet::~Packet()
{
	RowData::deallocate(rowData_);
}

bool Packet::dispatch(std::shared_ptr<rgs::io::Session> session)
{
	assert(handler_);
	assert(rowData_.data);

	nlohmann::json json = nlohmann::json::parse((char*)rowData_.data);
	(*handler_)(session, &json);

	return true;
}

bool Packet::dispatch(std::shared_ptr<rgs::io::Session> session, const rgs::protocol::RowData& rowData)const
{
	assert(handler_);
	assert(rowData.data);

	char* data = new char[rowData.size + 1];
	
	data[rowData.size] = 0;
	::memcpy(data, rowData.data, rowData.size);
	
	nlohmann::json json = nlohmann::json::parse(data);
	(*handler_)(session, &json);

	delete[] data;

	return true;
}

bool Packet::deserialize(const rgs::protocol::RowData& rowData)
{
	assert(rowData.data);

	RowData::deallocate(rowData_);

	rowData_ = RowData::allocate(rowData.size + 1);
	rowData_.data[rowData.size] = 0;
	
	RowData::copy(rowData_, rowData);

	return true;
}

void Packet::setHandler(Handler* handler)
{
	handler_ = handler;
}

Serialization::Serialization(const std::string& packetName, const nlohmann::json& packet) :
	packetName_(packetName),
	packet_(packet){}

Serialization::~Serialization()
{
	RowData::deallocate(rowData_);
}

rgs::protocol::RowData
Serialization::operator()()
{
	unsigned int headerSize = packetName_.length();
	if (headerSize > MAX_PACKET_NAME)
	{
		headerSize = MAX_PACKET_NAME;
	}

	std::string packet = packet_.dump();
	unsigned int bodySize = packet.length();
	unsigned int packetSize = sizeof(unsigned int) + headerSize + sizeof(unsigned int) + bodySize;

	RowData::deallocate(rowData_);

	rowData_ = RowData::allocate(packetSize);

	unsigned int writeBytes = 0;

	//header size
	::memcpy(rowData_.data, &headerSize, sizeof(unsigned int));
	writeBytes += sizeof(unsigned int);

	//header
	::memcpy(rowData_.data + writeBytes, packetName_.c_str(), headerSize);
	writeBytes += headerSize;

	//body size
	::memcpy(rowData_.data + writeBytes, &bodySize, sizeof(unsigned int));
	writeBytes += sizeof(unsigned int);

	//body
	::memcpy(rowData_.data + writeBytes, packet.c_str(), bodySize);

	return rowData_;
}

bool readHeaderSize(const rgs::protocol::RowData& rowData, unsigned int& headerSize, unsigned int& bytes, int& error)
{
	if (rowData.size < sizeof(unsigned int))
	{
		error = 0;
		return false;
	}

	::memcpy(&headerSize, rowData.data, sizeof(unsigned int));
	bytes += sizeof(unsigned int);

	if (headerSize > rgs::SocketConfig::BUFFER_SIZE)
	{
		error = Error::ERROR_ROW_DATA;
		return false;
	}

	return true;
}

bool readHeader(const rgs::protocol::RowData& rowData, char* header, unsigned int headerSize, unsigned int& bytes, int& error)
{
	if (rowData.size < headerSize + bytes)
	{
		error = 0;
		return false;
	}

	::memcpy(header, rowData.data + bytes, headerSize > MAX_PACKET_NAME ? MAX_PACKET_NAME : headerSize);
	bytes += headerSize;

	return true;
}

bool readBobySize(const rgs::protocol::RowData& rowData, unsigned int& bodySize, unsigned int& bytes, int& error)
{
	if (rowData.size < sizeof(unsigned int) + bytes)
	{
		error = 0;
		return false;
	}

	::memcpy(&bodySize, rowData.data + bytes, sizeof(unsigned int));
	bytes += sizeof(unsigned int);

	if (bodySize > rgs::SocketConfig::BUFFER_SIZE)
	{
		error = Error::ERROR_ROW_DATA;
		return false;
	}

	return true;
}

int 
Protocol::readPacket(const rgs::protocol::RowData& rowData, rgs::protocol::Packet** packet, unsigned int& readBytes)const
{
	*packet = nullptr;

	readBytes = 0;
	int error = 0;
	unsigned int bytes = 0;
	
	//header size
	unsigned int headerSize = 0;
	if (readHeaderSize(rowData, headerSize, bytes, error) == false)
	{
		return error;
	}

	//header
	char header[MAX_PACKET_NAME] = { 0, };
	if (readHeader(rowData, header, headerSize, bytes, error) == false)
	{
		return error;
	}

	auto iterator = handlers_.find(header);
	if (iterator == handlers_.end())
	{
		return Error::ERROR_ROW_DATA;
	}

	//body size
	unsigned int bodySize = 0;
	if (readBobySize(rowData, bodySize, bytes, error) == false)
	{
		return error;
	}

	//body
	if (rowData.size < bodySize + bytes)
	{
		return 0;
	}
	
	auto readPacket = new Packet();
	readPacket->setHandler(iterator->second.get());

	rgs::protocol::RowData rowPacket(rowData.data + bytes, bodySize);
	if (readPacket->deserialize(rowPacket) == false)
	{
		delete readPacket;
		readPacket = nullptr;

		return Error::ERROR_DESERIALIZE;
	}
	
	*packet = readPacket;

	bytes += bodySize;
	readBytes = bytes;

	return 0;
}

int Protocol::dispatchPacket(const rgs::protocol::RowData& rowData, std::shared_ptr<rgs::io::Session> session, unsigned int& readBytes)const
{
	readBytes = 0;
	int error = 0;
	unsigned int bytes = 0;

	//header size
	unsigned int headerSize = 0;
	if (readHeaderSize(rowData, headerSize, bytes, error) == false)
	{
		return error;
	}

	//header
	char header[MAX_PACKET_NAME] = { 0, };
	if (readHeader(rowData, header, headerSize, bytes, error) == false)
	{
		return error;
	}

	auto iterator = handlers_.find(header);
	if (iterator == handlers_.end())
	{
		return Error::ERROR_ROW_DATA;
	}

	//body size
	unsigned int bodySize = 0;
	if (readBobySize(rowData, bodySize, bytes, error) == false)
	{
		return error;
	}

	//body
	if (rowData.size < bodySize + bytes)
	{
		return 0;
	}
	
	Packet packet;
	packet.setHandler(iterator->second.get());

	rgs::protocol::RowData rowPacket(rowData.data + bytes, bodySize);
	packet.dispatch(session, rowPacket);

	bytes += bodySize;
	readBytes = bytes;

	return 0;
}