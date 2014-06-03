#include <anet/ClientConnections.h>
#include <anet/ServerUdp.h>
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
	Login(std::shared_ptr<anet::Client> client)
		: Process(Processes::LOGIN),
		client_(client)
	{

	}

	void SendHello()
	{
		std::shared_ptr<anet::Packet> hello(new anet::Packet());

		*hello.get() << Processes::LOGIN;

		*hello.get() << "hello";

		client_->SendPacket(hello);
	}

	void HandlePacket(anet::Packet& packet)
	{
		std::string message;
		packet >> message;

		if (message.compare("credentials") == 0)
		{
			printf("CLIENT: %s\n", message.c_str());

			std::string username, password;
			packet >> username >> password;

			printf("CLIENT: %s %s\n", username.c_str(), password.c_str());

			std::shared_ptr<anet::Packet> welcome(new anet::Packet());

			*welcome.get() << Processes::LOGIN;

			*welcome.get() << "welcome";

			*welcome.get() << username;

			client_->SendPacket(welcome);
		}
		else if (message.compare("bye") == 0)
		{
			printf("SERVER: %s\n", message);

		}
	}

private:
	std::shared_ptr<anet::Client> client_;
};

static bool connected = true;
static anet::ClientConnections *connections;

std::shared_ptr<anet::Client> OnCreatedClient(std::shared_ptr<anet::Client> client)
{
	std::shared_ptr<Login> login(new Login(client));

	client->AddProcess(login);

	login->SendHello();

	return client;
}

void OnClientConnected(unsigned short id)
{
	printf("CONNECTED: id %hu", id);
}

void OnClientDisconnected(unsigned short id)
{
	printf("DISCONNECTED: id %hu", id);
}

int main()
{
	std::shared_ptr<anet::ServerUdp> udp(new anet::ServerUdp(9000, 32));
	connections = new anet::ClientConnections(OnCreatedClient, udp);

	//Functions will automatically unregister when tokens go out of scope

	auto functoken = connections->OnClientConnected(OnClientConnected);
	auto functoken2 = connections->OnClientDisconnected(OnClientDisconnected);

	while (connected)
	{
		Sleep(10);

		udp->Update();

		connections->Update();
	}

	return 0;
}