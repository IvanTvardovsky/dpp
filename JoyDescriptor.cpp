#pragma once
#include <string> 
#include "JoyDescriptor.hpp"

#ifdef USERINPUT_EXPORTS
#define USERINPUT_API __declspec(dllexport) 
#else
#define USERINPUT_API __declspec(dllimport) 
#endif

class USERINPUT_API IJoyDescriptor
{
public:
	virtual int GetIndex() = 0;
	virtual std::string GetName() = 0;
	virtual SDL_Joystick* GetJoystic() = 0;

	virtual bool isEmpty() = 0;

	virtual IJoyDescriptor& operator =(const IJoyDescriptor& p) ;
	virtual bool operator ==(const IJoyDescriptor& p) const ;
protected:
	int index = -1;
	std::string Name = "";
	SDL_Joystick* joystic = nullptr;
};

int JoyDescriptor::GetIndex()
{
 return index;
}

std::string JoyDescriptor::GetName()
{
 return Name;
}

SDL_Joystick* JoyDescriptor::GetJoystic()
{
 return joystic;
}

void JoyDescriptor::SetIndex(int id)
{
 index = id;
}

void JoyDescriptor::SetName(std::string name)
{
 Name = name;
}
void JoyDescriptor::SetJoystic(SDL_Joystick* joy)
{
 joystic = joy;
}

bool JoyDescriptor::isEmpty()
{
 return index == -1 || Name == "" || joystic == nullptr;
}

JoyDescriptor& JoyDescriptor::operator=(const JoyDescriptor& p)
{
 index = p.index;
 Name = p.Name;
 joystic = p.joystic;
 return *this;
}

bool JoyDescriptor::operator==(const JoyDescriptor& p) const
{
 return index == p.index &&
 Name == p.Name &&
 joystic == p.joystic;
} 
