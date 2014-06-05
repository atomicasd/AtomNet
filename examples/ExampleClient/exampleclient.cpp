#include <anet/ClientConnection.h>
#include <anet/ClientUdp.h>
#include <anet/Process.h>
#include <anet/Packet.h>
#include <anet/Client.h>

#include <stdio.h>
#include <memory>

#include <Windows.h>

//Needs to be identical on the server

typedef enum Processes
{
	HANDSHAKE,
	ENCRYPTION,
	LOGIN,
	GAME,
	CHAT

} Processes;

class Login : public anet::Process
{
public:
	Login(anet::ClientConnection& connection)
		: Process(Processes::LOGIN),
		connection_(connection)
	{

	}

	void HandlePacket(anet::Packet& packet)
	{
		std::string message;
		packet >> message;

		if (message.compare("hello") == 0)
		{
			printf("SERVER: %s\n", message.c_str());

			anet::Packet cretentials;

			cretentials << Processes::LOGIN;

			cretentials << "credentials";

			cretentials << "username";

			cretentials << "password";

			connection_.SendPacket(cretentials);
		}
		else if (message.compare("welcome") == 0)
		{
			std::string username;
			packet >> username;

			printf("SERVER: %s %s\n", message.c_str(), username.c_str());

			anet::Packet bye;

			bye << Processes::LOGIN;

			bye << "bye";

			connection_.SendPacket(bye);

			connection_.Disconnect();
		}
	}

private:
	anet::ClientConnection& connection_;
};
static bool connected = true;

void OnConnectionResult(bool result)
{
	if (result == false)
	{
		connected = false;
	}
}

int main()
{
	Sleep(1000);

	anet::ClientUdp udp(OnConnectionResult);

	anet::ClientConnection connection(udp);

	connection.SetHost("127.0.0.1", 9000);
	connection.Connect();

	Login login(connection);
	connection.AddProcess(login);

	while (connected)
	{
		Sleep(10);

		connection.Update();
	}

	return 0;
}