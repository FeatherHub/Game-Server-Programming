#pragma once

#include "Constants.h"
#include <string>

class User
{
public:
	User() = default;
	void SetKey() { m_userKey = USER_SERIAL_INDEX_KEY++; }
	int GetKey() { return m_userKey; }
	std::string GetName() const { return m_name; }
	void SetName(std::string val) { m_name = val; }
	int GetClientIdx() const { return m_clientIdx; }
	void SetClientIdx(int val) { m_clientIdx = val; }

private:
	static int USER_SERIAL_INDEX_KEY;
	int m_userKey;
	std::string m_name;
	int m_clientIdx;
};