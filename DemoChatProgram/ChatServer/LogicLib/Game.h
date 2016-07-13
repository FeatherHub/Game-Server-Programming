#pragma once

#include <chrono>

namespace NLogicLib
{
	enum class GameState 
	{
		NONE,
		STARTING, //요청하고 시작 준비 중
		PLAYING,
		END //결과창 보여주는 경우
	};

	class Game
	{
	public:
		Game();
		virtual ~Game(); //Unit test를 위해

		void Clear();

		GameState GetState() { return m_state; }

		void SetState(const GameState state) { m_state = state; }

		bool CheckSelectTime();
	private:
		GameState m_state = GameState::NONE;

		//상대가 고르면 갱신된다
		//둘 다 시간이 지나면 무승부
		//시간이 지나면 턴이 넘어간다
		std::chrono::system_clock::time_point m_selectTime 
			= std::chrono::system_clock::now();

		__int64 m_
		int m_select1; //가 0 바 1 보 2 무 3
		int m_select2;
	};
}