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
	Login(anet::ClientConnection* connection)
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

			std::shared_ptr<anet::Packet> cretentials(new anet::Packet());

			*cretentials.get() << Processes::LOGIN;

			*cretentials.get() << "credentials";

			*cretentials.get() << "username";

			*cretentials.get() << "password";

			connection_->SendPacket(cretentials);
		}
		else if (message.compare("welcome") == 0)
		{
			std::string username;
			packet >> username;

			printf("SERVER: %s %s\n", message.c_str(), username.c_str());

			std::shared_ptr<anet::Packet> bye(new anet::Packet());

			*bye.get() << Processes::LOGIN;

			*bye.get() >> "bye";

			connection_->SendPacket(bye);

			connection_->Disconnect();
		}
	}

private:
	anet::ClientConnection* connection_;
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

	std::shared_ptr<anet::ClientUdp> udp(new anet::ClientUdp(OnConnectionResult));
	anet::ClientConnection connection(udp);

	connection.SetHost("127.0.0.1", 9000);
	connection.Connect();

	std::shared_ptr<anet::Process> login(new Login(&connection));

	connection.AddProcess(login);

	while (connected)
	{
		Sleep(10);

		connection.Update();
	}

	return 0;
}