#pragma once

#include "Constants.h"
#include <string>

class Client
{
public:
	Client() = default;
	~Client() {}
	
	void SetNewKey() { m_userKey = USER_SERIAL_INDEX_KEY++; }
	int GetKey() const { return m_userKey; }
	
	std::string GetName() const { return m_name; }
	void SetName(const std::string& val) { m_name = val; }
	
	int GetClientIdx() const { return m_clientIdx; }
	void SetClientIdx(const int val) { m_clientIdx = val; }
	
	bool Connected() { return m_connected; }
	void Disconnect() { m_connected = false; }
	void Connect() { m_connected = true; }
private:
	static int USER_SERIAL_INDEX_KEY;
	int m_userKey = -1;
	std::string m_name;
	int m_clientIdx = -1;
	bool m_connected = false;
};