#pragma once

namespace NNetworkLib
{
	class SelectNetwork;
}

class PacketManager;

class LogicMain
{
	using Network = NNetworkLib::SelectNetwork;
	
public:
	LogicMain() = default;

	void Init();
	void Run();
	void StopRun() { m_isRun = false; }

private:
	Network* m_network;
	PacketManager* m_pktMgr;

	bool m_isRun;
};