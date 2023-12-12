#pragma once
#include <Windows.h>
#include <vector>
#include <memory>

class KeyDescriptor
{
public:
	bool isPressed = false;
	int key = -1;
};

class KeyEventHandler
{
private:
	bool keysState[255];
	std::vector<std::shared_ptr< KeyDescriptor>> eventQuery;

public:
	void CheckState();
	std::shared_ptr< KeyDescriptor> PoolEvent();
};
