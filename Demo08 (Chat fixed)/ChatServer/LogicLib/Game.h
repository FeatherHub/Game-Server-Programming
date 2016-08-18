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
	};
}