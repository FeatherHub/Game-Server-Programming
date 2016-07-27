#include "Game.h"

namespace NLogicLib
{
	Game::Game()
	{

	}

	Game::~Game()
	{

	}

	void Game::Clear()
	{
		m_state = GameState::NONE;
	}

	bool Game::CheckSelectTime()
	{
		auto curTime = std::chrono::system_clock::now();
		auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_LatestLoginCheckTime);
		
		// 60밀리초 마다 검사
		if (diffTime.count() < 60)
		{
			return;
		}
		else
		{
			m_LatestLoginCheckTime = curTime;
		}

		auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

		auto diff = curSecTime - ConnectedUserList[i].m_ConnectedTime;
		if (diff >= 3)
		{
			m_pRefLogger->Write(NServerNetLib::LOG_TYPE::L_WARN, "%s | Login Wait Time Over. sessionIndex(%d).", __FUNCTION__, i);
			m_pRefNetwork->ForcingClose(i);
		}

	}

}