#pragma once

//클라이언트 프로토콜
namespace frontend
{
	enum PacketType {
		PacketType_Ping = 0,
		PacketType_Login = 1,
		PacketType_LoginResult = 2,
		PacketType_Join = 3,
		PacketType_JoinResult = 4,
		PacketType_GameStart = 5,
		PacketType_GameEnter = 6,
		PacketType_ServerAddress = 7,
		PacketType_EnterRoom = 8,
		PacketType_ExistingUsers = 9,
		PacketType_MoveCharacter = 10
	};
}

//game1 서버 프로토콜
namespace game1_server
{
	enum PacketType {
		PacketType_Login = 0,
		PacketType_LoginResult = 1,
		PacketType_Logout = 2
	};
}