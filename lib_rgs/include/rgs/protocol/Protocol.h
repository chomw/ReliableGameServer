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
		/**
		@class RowData

		@brief binary stream형태의 데이터와 그 데이터의 크기를 담는 구조체

		binary stream 데이터를 담고 있으며, 데이터의 할당 및 해제, 복사 함수를 가지고 있습니다

		@author 조민우
		@since 1.0.0
		*/
		struct RowData
		{
			/// @name Constructors 
			/// @{
			
			/**
			@brief 디폴트 생성자

			null이나 0으로 초기화

			@since 1.0.0
			*/
			RowData() = default;

			/**
			@brief null이 아닌 할당된 데이터와 데이터 크기로 초기화하며 생성한다.

			@since 1.0.0
			*/
			RowData(
				BYTE* data
				, unsigned int size
			) :
				data(data),
				size(size) {}
			
			/// @}

			/// @name Memory Control Functions
			/// @{

			/**
			@brief 지정한 인자만큼 메모리를 할당한 RowData를 생성한다.

			@param[in] size 메모리를 할당할 크기

			@return 메모리가 할당된 새로운 RowData 객체

			@since 1.0.0
			*/
			static RowData allocate(unsigned int size)
			{
				RowData rowData;
				rowData.data = new BYTE[size];
				rowData.size = size;
				return rowData;
			}

			/**
			@brief 지정한 데이터와 데이터 크기를 복사한 RowData를 생성한다.

			메모리를 할당한 후에 지정한 인자들을 복사해 새로운 RowData 객체를 생성한다.

			@param[in] data null이 아닌 할당된 데이터
			@param[in] size 데이터의 크기

			@return 데이터가 복사된 새로운 RowData 객체

			@since 1.0.0
			*/
			static RowData allocate(
				const BYTE* data
				, unsigned int size
			)
			{
				RowData rowData = RowData::allocate(size);

				::memcpy(rowData.data, data, size);

				return rowData;
			}

			/**
			@brief 지정한 RowData의 복사본을 생성한다.

			@param[in] origin 복사할 원본

			@return 복사된 새로운 RowData 객체

			@since 1.0.0
			*/
			static RowData allocate(const RowData& origin)
			{
				return RowData::allocate(origin.data, origin.size);
			}

			/**
			@brief 지정된 RowData를 해제한다.

			@param[out] rowData 메모리가 할당된 RowData 객체

			@since 1.0.0
			*/
			static void deallocate(RowData& rowData)
			{
				if (rowData.data != nullptr)
				{
					delete[] rowData.data;
					rowData.data = nullptr;
				}
			}

			/**
			@brief RowData 객체 간 복사한다.

			@param[out] dest 복사할 대상
			@param[in] src 복사할 원본

			@since 1.0.0
			*/
			static void copy(RowData& dest, const RowData& src)
			{
				::memcpy(dest.data, src.data, src.size);
			}

			/// @}

			BYTE* data = nullptr;
			unsigned int size = 0;
		};

		
		/**
		@class Packet

		@brief 다양한 패킷 포맷을 추상화하고, 패킷 핸들러로 디스패치(dispatch)하는 추상 클래스

		(1) 다양한 패킷 포맷(protobuf, json...etc)을 추상화한다.
		-Packet을 상속받아 다양한 패킷 포맷을 정의해야 합니다.

		(2) 패킷 핸들러 디스패치(dispatch)
		-패킷 핸들러를 소유하고, dispatch()가 호출되면 패킷을 전달해야 합니다.

		(3) protocol::Deserialization을 이용해 역직렬화
		-deserialize()로 전달된 RowData를 역직렬화하여 자신이 정의하는 패킷으로 가공해야 합니다.

		@author 조민우
		@since 1.0.0
		*/
		class Packet
		{
		public:
			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~Packet() = default;

			/**
			@brief 패킷 핸들러 디스패치(dispatch)

			자신이 정의한 패킷 객체를 패킷 핸들러를 호출하여 전달합니다.

			@param[in] session 패킷을 보낸 Session

			@return 패킷 핸들러 호출하면 true, 실패하면 false

			@since 1.0.0
			*/
			virtual bool dispatch(std::shared_ptr<rgs::io::Session> session) = 0;

			/**
			@brief RowData에 있는 데이터를 역직렬화하여 패킷으로 저장합니다.

			protocol::Deserialization의 구현 객체를 이용해 역직렬화를 처리하게 합니다.
			그 결과로 패킷을 받아 저장합니다.

			@param[in] rowData binary stream형태의 데이터를 가진 RowData 객체

			@return 역직렬화를 성공하면 true, 실패하면 false

			@since 1.0.0
			*/
			virtual bool deserialize(const RowData& rowData) = 0;
		};


		/**
		@class CreatePacket

		@brief Packet을 생성하고 초기화하는 팩토리 클래스

		@author 조민우
		@since 1.0.0
		*/
		class CreatePacket
		{
		public:
			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~CreatePacket() = default;

			/**
			@brief Packet을 생성한다

			@return 새로운 Packet 객체

			@since 1.0.0
			*/
			virtual Packet* operator()()const = 0;
		};

		/**
		@class Serialization

		@brief 직렬화 추상 클래스

		이 클래스는 직렬화 추상 클래스로, 이 클래스를 상속받은 클래스들은
		패킷 포맷에 따른 직렬화를 정의해야 합니다.
		
		@author 조민우
		@since 1.0.0
		*/
		class Serialization
		{
		public:
			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~Serialization() = default;

			/**
			@brief 직렬화를 한다.

			@return 직렬화한 binary stream형태의 데이터를 가진 RowData 객체

			@since 1.0.0
			*/
			virtual RowData operator()() = 0;
		};

		/**
		@class Deserialization

		@brief 역직렬화 추상 클래스

		이 클래스는 역직렬화 추상 클래스로, 이 클래스를 상속받은 클래스들은
		패킷 포맷에 따른 역직렬화를 정의해야 합니다.

		@author 조민우
		@since 1.0.0
		*/
		class Deserialization
		{
		public:
			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~Deserialization() = default;

			/**
			@brief 역직렬화를 한다.

			@param[in] rowData 역직렬화를 할 binary stream 형태의 데이터를 담은 RowData 객체

			@return 역직렬화를 성공하면 true, 실패하면 false

			@since 1.0.0
			*/
			virtual bool operator()(const rgs::protocol::RowData& rowData) = 0;
		};


		/**
		@class Protocol

		@brief 패킷 헤더를 읽어 응용 레벨 프로토콜에 따라 패킷 처리를 하는 추상 클래스

		이 추상 클래스를 상속받은 클래스는 
		binary stream 형태의 데이터의 헤더를 읽는 방법을 정의하고, 
		헤더로 패킷을 식별해 응용 레벨 프로토콜에 따라 패킷 처리를 해야 합니다.

		@author 조민우
		@since 1.0.0
		*/
		class Protocol
		{
		public:
			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~Protocol() = default;

			/**
			@brief 응용 레벨 프로토콜에 따라 패킷 처리를 합니다.

			@param[in] rowData binary stream 형태의 데이터를 가진 RowData 객체
			@param[out] packet 패킷 처리의 결과를 받을 Packet 포인터. 패킷 처리가 실패하면 null이 설정된다.
			@param[out] readBytes 패킷의 바이트 크기

			@return 패킷 처리가 성공하면 0, 실패하면 에러 코드

			@since 1.0.0
			*/
			virtual int readPacket(
				const RowData& rowData
				, Packet** packet
				, unsigned int& readBytes
			)const = 0;
		};
	}
}