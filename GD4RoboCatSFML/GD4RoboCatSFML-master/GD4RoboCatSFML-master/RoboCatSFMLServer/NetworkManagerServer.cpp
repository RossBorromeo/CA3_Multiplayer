#include "RoboCatServerPCH.hpp"

NetworkManagerServer* NetworkManagerServer::sInstance;


NetworkManagerServer::NetworkManagerServer() :
	mNewPlayerId(1),
	mNewNetworkId(1),
	mTimeBetweenStatePackets(0.033f),
	mClientDisconnectTimeout(10.f),
	mReplicationManagerServer(new ReplicationManagerServer())
{
}

bool NetworkManagerServer::StaticInit(uint16_t inPort)
{
	sInstance = new NetworkManagerServer();
	return sInstance->Init(inPort);
}

void NetworkManagerServer::HandleConnectionReset(const SocketAddress& inFromAddress)
{
	//just dc the client right away...
	auto it = mAddressToClientMap.find(inFromAddress);
	if (it != mAddressToClientMap.end())
	{
		HandleClientDisconnected(it->second);
	}
}

void NetworkManagerServer::ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	//try to get the client proxy for this address
	//pass this to the client proxy to process
	auto it = mAddressToClientMap.find(inFromAddress);
	if (it == mAddressToClientMap.end())
	{
		//didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
		HandlePacketFromNewClient(inInputStream, inFromAddress);
	}
	else
	{
		ProcessPacket((*it).second, inInputStream);
	}
}

void NetworkManagerServer::ProcessPacket(ClientProxyPtr client, InputMemoryBitStream& inInputStream)
{
	client->UpdateLastPacketTime();

	uint32_t packetType;
	inInputStream.Read(packetType);

	switch (packetType)
	{
	case kHelloCC:
		std::cout << "[Server] Received duplicate Hello from: " << client->GetName() << std::endl;
		SendWelcomePacket(client);
		break;

	case kInputCC:
		if (client->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
		{
			HandleInputPacket(client, inInputStream);
		}
		break;

	default:
		std::cerr << "[Server] Unknown packet type " << packetType
			<< " from " << client->GetSocketAddress().ToString() << std::endl;
		break;
	}
}



void NetworkManagerServer::HandlePacketFromNewClient(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	uint32_t packetType;
	inInputStream.Read(packetType);

	std::cout << "[Server] Received new packet from: " << inFromAddress.ToString() << std::endl;
	std::cout << "[Server] Packet type: " << packetType << std::endl;

	if (packetType != kHelloCC)
	{
		std::cerr << "[Server] Unknown packet type from unknown client. Possible spoofing attempt.\n";
		return;
	}

	std::string name;
	inInputStream.Read(name);
	std::cout << "[Server] Hello from client: " << name << std::endl;

	auto newClientProxy = std::make_shared<ClientProxy>(inFromAddress, name, mNewPlayerId++);
	mAddressToClientMap[inFromAddress] = newClientProxy;
	mPlayerIdToClientMap[newClientProxy->GetPlayerId()] = newClientProxy;

	if (auto server = std::dynamic_pointer_cast<Server>(Engine::s_instance))
	{
		server->HandleNewClient(newClientProxy);
	}
	else
	{
		std::cerr << "[Server] Engine::s_instance not properly initialized!\n";
	}

	SendWelcomePacket(newClientProxy);

	for (const auto& [networkId, gameObject] : m_network_id_to_game_object_map)
	{
		newClientProxy->GetReplicationManagerServer().ReplicateCreate(networkId, gameObject->GetAllStateMask());
	}
}


void NetworkManagerServer::SendWelcomePacket(ClientProxyPtr client)
{
	OutputMemoryBitStream welcomePacket;
	welcomePacket.Write(kWelcomeCC);
	welcomePacket.Write(client->GetPlayerId());

	std::cout << "[Server] Sending welcome to " << client->GetName()
		<< " (PlayerID=" << client->GetPlayerId() << ") at "
		<< client->GetSocketAddress().ToString() << std::endl;

	SendPacket(welcomePacket, client->GetSocketAddress());
}


void NetworkManagerServer::RespawnCats()
{
	for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
	{
		ClientProxyPtr clientProxy = it->second;

		clientProxy->RespawnCatIfNecessary();
	}
}

void NetworkManagerServer::SendOutgoingPackets()
{
	//let's send a client a state packet whenever their move has come in...
	for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
	{
		ClientProxyPtr clientProxy = it->second;
		//process any timed out packets while we're going through the list
		clientProxy->GetDeliveryNotificationManager().ProcessTimedOutPackets();

		if (clientProxy->IsLastMoveTimestampDirty())
		{
			SendStatePacketToClient(clientProxy);
		}
	}
}

void NetworkManagerServer::UpdateAllClients()
{
	for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
	{
		//process any timed out packets while we're going throug hthe list
		it->second->GetDeliveryNotificationManager().ProcessTimedOutPackets();

		SendStatePacketToClient(it->second);
	}
}

void NetworkManagerServer::SendStatePacketToClient(ClientProxyPtr inClientProxy)
{
	//build state packet
	OutputMemoryBitStream	statePacket;

	//it's state!
	statePacket.Write(kStateCC);

	InFlightPacket* ifp = inClientProxy->GetDeliveryNotificationManager().WriteState(statePacket);

	WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);

	AddScoreBoardStateToPacket(statePacket);

	ReplicationManagerTransmissionData* rmtd = new ReplicationManagerTransmissionData(&inClientProxy->GetReplicationManagerServer());
	inClientProxy->GetReplicationManagerServer().Write(statePacket, rmtd);
	ifp->SetTransmissionData('RPLM', TransmissionDataPtr(rmtd));

	SendPacket(statePacket, inClientProxy->GetSocketAddress());

}

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxyPtr inClientProxy)
{
	//first, dirty?
	bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
	inOutputStream.Write(isTimestampDirty);
	if (isTimestampDirty)
	{
		inOutputStream.Write(inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
		inClientProxy->SetIsLastMoveTimestampDirty(false);
	}
}

//should we ask the server for this? or run through the world ourselves?
void NetworkManagerServer::AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream)
{
	const auto& gameObjects = World::sInstance->GetGameObjects();

	//now start writing objects- do we need to remember how many there are? we can check first...
	inOutputStream.Write(gameObjects.size());

	for (GameObjectPtr gameObject : gameObjects)
	{
		inOutputStream.Write(gameObject->GetNetworkId());
		inOutputStream.Write(gameObject->GetClassId());
		gameObject->Write(inOutputStream, 0xffffffff);
	}
}

void NetworkManagerServer::AddScoreBoardStateToPacket(OutputMemoryBitStream& inOutputStream)
{
	ScoreBoardManager::sInstance->Write(inOutputStream);
}


int NetworkManagerServer::GetNewNetworkId()
{
	int toRet = mNewNetworkId++;
	if (mNewNetworkId < toRet)
	{
		LOG("Network ID Wrap Around!!! You've been playing way too long...", 0);
	}

	return toRet;

}

void NetworkManagerServer::HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream)
{
	uint32_t moveCount = 0;
	Move move;
	inInputStream.Read(moveCount, 2);

	for (; moveCount > 0; --moveCount)
	{
		if (move.Read(inInputStream))
		{
			if (inClientProxy->GetUnprocessedMoveList().AddMoveIfNew(move))
			{
				inClientProxy->SetIsLastMoveTimestampDirty(true);
			}
		}
	}
}

ClientProxyPtr NetworkManagerServer::GetClientProxy(int inPlayerId) const
{
	auto it = mPlayerIdToClientMap.find(inPlayerId);
	if (it != mPlayerIdToClientMap.end())
	{
		return it->second;
	}

	return nullptr;
}

void NetworkManagerServer::CheckForDisconnects()
{
	vector< ClientProxyPtr > clientsToDC;

	float minAllowedLastPacketFromClientTime = Timing::sInstance.GetTimef() - mClientDisconnectTimeout;
	for (const auto& pair : mAddressToClientMap)
	{
		if (pair.second->GetLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
		{
			//can't remove from map while in iterator, so just remember for later...
			clientsToDC.push_back(pair.second);
		}
	}

	for (ClientProxyPtr client : clientsToDC)
	{
		HandleClientDisconnected(client);
	}
}

void NetworkManagerServer::HandleClientDisconnected(ClientProxyPtr client)
{
	std::cout << "[Server] Client disconnected: " << client->GetName()
		<< " (PlayerID=" << client->GetPlayerId() << ")" << std::endl;

	mPlayerIdToClientMap.erase(client->GetPlayerId());
	mAddressToClientMap.erase(client->GetSocketAddress());

	if (auto server = std::dynamic_pointer_cast<Server>(Engine::s_instance))
	{
		server->HandleLostClient(client);
	}

	if (mAddressToClientMap.empty())
	{
		std::cout << "[Server] No more clients connected. Stopping server." << std::endl;
		Engine::s_instance->SetShouldKeepRunning(false);
	}
}


void NetworkManagerServer::RegisterGameObject(GameObjectPtr inGameObject)
{
	//assign network id
	int newNetworkId = GetNewNetworkId();
	inGameObject->SetNetworkId(newNetworkId);

	//add mapping from network id to game object
	m_network_id_to_game_object_map[newNetworkId] = inGameObject;

	//tell all client proxies this is new...
	for (const auto& pair : mAddressToClientMap)
	{
		pair.second->GetReplicationManagerServer().ReplicateCreate(newNetworkId, inGameObject->GetAllStateMask());
	}
}


void NetworkManagerServer::UnregisterGameObject(GameObject* inGameObject)
{
	int networkId = inGameObject->GetNetworkId();
	m_network_id_to_game_object_map.erase(networkId);

	//tell all client proxies to STOP replicating!
	//tell all client proxies this is new...
	for (const auto& pair : mAddressToClientMap)
	{
		pair.second->GetReplicationManagerServer().ReplicateDestroy(networkId);
	}
}

void NetworkManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
	//tell everybody this is dirty
	for (const auto& pair : mAddressToClientMap)
	{
		pair.second->GetReplicationManagerServer().SetStateDirty(inNetworkId, inDirtyState);
	}
}


