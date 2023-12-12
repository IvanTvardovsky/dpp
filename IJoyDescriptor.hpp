#pragma once
#include <string> 
#include "IJoyDescriptor.hpp"

class JoyDescriptor:public IJoyDescriptor
{
public:
	int GetIndex();
	std::string GetName();
	SDL_Joystick* GetJoystic();

	void SetIndex(int id);
	void SetName(std::string name);
	void SetJoystic(SDL_Joystick* joy);
	bool isEmpty();

	JoyDescriptor& operator =(const JoyDescriptor& p);
	bool operator ==(const JoyDescriptor& p) const;
};
