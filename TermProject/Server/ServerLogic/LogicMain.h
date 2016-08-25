#pragma once

namespace NNetworkLib
{
	class SelectNetwork;
}

class UserManager;
class PacketProcArray;

class LogicMain
{
	using Network = NNetworkLib::SelectNetwork;
	
public:
	LogicMain() = default;

	void Init();
	void Run();
	void StopRun() { m_isRun = false; }

private:
	void PostProcess();
private:
	Network* m_pNetwork;
	PacketProcArray* m_pktProcArr;
	UserManager* m_pUserMgr;

	bool m_isRun;
};