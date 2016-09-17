#pragma once
#include <memory>

namespace rgs
{
	namespace io
	{
		class Socket;
		class Session;
	}

	/**
	@namespace protocol
	@brief 패킷의 Serialization, Deserialization과 패킷 핸들링 프로토콜 네임스페이스
	@since 1.0.0
	*/
	namespace protocol
	{
		struct RowData
		{
			RowData() = default;
			RowData(BYTE* data, unsigned int size) :
				data(data),
				size(size) {}

			static RowData allocate(unsigned int size)
			{
				RowData rowData;
				rowData.data = new BYTE[size];
				rowData.size = size;
				return rowData;
			}

			static RowData allocate(const BYTE* data, unsigned int size)
			{
				RowData rowData = RowData::allocate(size);

				::memcpy(rowData.data, data, size);

				return rowData;
			}

			static RowData allocate(const RowData& origin)
			{
				return RowData::allocate(origin.data, origin.size);
			}

			static void deallocate(RowData& rowData)
			{
				if (rowData.data != nullptr)
				{
					delete[] rowData.data;
					rowData.data = nullptr;
				}
			}

			static void copy(RowData& dest, const RowData& src)
			{
				::memcpy(dest.data, src.data, src.size);
			}

			BYTE* data = nullptr;
			unsigned int size = 0;
		};

		class Packet
		{
		public:
			virtual ~Packet() = default;

			virtual bool dispatch(std::shared_ptr<rgs::io::Session> ) = 0;

			virtual bool deserialize(const RowData& rowData) = 0;
		};

		class CreatePacket
		{
		public:
			virtual ~CreatePacket() = default;
			virtual Packet* operator()()const = 0;
		};

		class Serialization
		{
		public:
			virtual ~Serialization() = default;
			virtual RowData operator()() = 0;
		};

		class Deserialization
		{
		public:
			virtual ~Deserialization() = default;
			virtual bool operator()(const rgs::protocol::RowData& rowData) = 0;
		};

		class Protocol
		{
		public:
			virtual ~Protocol() = default;
			virtual int readPacket(const RowData& rowData, Packet** packet, unsigned int& readBytes)const = 0;
		};
	}
}