#pragma once
#include <qobject.h>
#include "SDL/SDL.h"
#include <list>
#include "IJoyDescriptor.hpp"
#include "KeyEventHandler.hpp"
#include "DeviceHandler.hpp"
#include <thread>


#ifdef USERINPUT_EXPORTS
#define USERINPUT_API __declspec(dllexport) 
#else
#define USERINPUT_API __declspec(dllimport) 
#endif

namespace UserInput {
	class USERINPUT_API IDeviceHandler :
		public QObject
	{
	public:
		virtual const std::list < std::shared_ptr<IJoyDescriptor>>& GetJoysDescriptors() = 0;
		virtual void StartListenEvents() = 0;
		virtual ~IDeviceHandler() = default;
		virtual int GetAxesCount(std::shared_ptr<IJoyDescriptor> joy) = 0;
		virtual const std::shared_ptr<IJoyDescriptor> GetDescriptorById(int id) = 0;

		//	signals:
		virtual void JoyDeviceRemoved() = 0;
		virtual void JoyDeviceAdded() = 0;
		virtual void JoyButtonDown(SDL_JoyButtonEvent joyButtonEvent) = 0;
		virtual void JoyButtonUp(SDL_JoyButtonEvent joyButtonEvent) = 0;
		virtual void KeyButtonEvent(std::shared_ptr< KeyDescriptor> keyDescriptor) = 0;
		virtual void AxisMotion(SDL_JoyAxisEvent joyAxisEvent) = 0;
	};
}

Q_DECLARE_INTERFACE(UserInput::IDeviceHandler, "QInputLibrary")

UserInput::DeviceHandler::DeviceHandler(QObject* parent)

{
	SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
	SDL_JoystickEventState(SDL_ENABLE);

	EventHandler();
	StartListenEvents();
}

UserInput::DeviceHandler::~DeviceHandler()
{
	isEventsListen = false;
	for(auto &item: joysDescriptors)
	{
		if (!SDL_JoystickGetAttached(item->GetJoystic())) {
			SDL_JoystickClose(item->GetJoystic());
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
const std::list<std::shared_ptr<IJoyDescriptor>>& UserInput::DeviceHandler::GetJoysDescriptors()
{
	IJoysDescriptors.clear();
	for (auto item : joysDescriptors)
	{
		IJoysDescriptors.push_back(item);
	}

	return IJoysDescriptors;
}

const std::shared_ptr<IJoyDescriptor> UserInput::DeviceHandler::GetDescriptorById(int id)
{
	std::shared_ptr<IJoyDescriptor> result = nullptr;
	for (auto item : joysDescriptors)
	{
		if (item->GetIndex() == id)
		{
			result = item;
			break;
		}
	}
	
	return result;
}

void UserInput::DeviceHandler::StartListenEvents()
{
	std::thread thr(&UserInput::DeviceHandler::UpdateEvents, this);
	thr.detach();
}

void UserInput::DeviceHandler::AddJoystic(Sint32 which)
{
	auto item = std::make_shared<JoyDescriptor>();

	item->SetJoystic(SDL_JoystickOpen(which));
	item->SetName ( SDL_JoystickName(item->GetJoystic()));
	item->SetIndex( SDL_JoystickInstanceID(item->GetJoystic()));
	joysDescriptors.push_back(item);
}

void UserInput::DeviceHandler::RemoveJoystic(Sint32 which)
{
	auto numJoy = SDL_NumJoysticks();
	std::shared_ptr<JoyDescriptor> shouldRemove;
	for (auto item : joysDescriptors)
	{
		if (!SDL_JoystickGetAttached(item->GetJoystic())) {
			SDL_JoystickClose(item->GetJoystic());
			shouldRemove = item;
		}
	}
	joysDescriptors.remove(shouldRemove);
}
void UserInput::DeviceHandler::UpdateEvents()
{
	int Length;
	const Uint8* KeyboardState = SDL_GetKeyboardState(&Length);
	int i;
	isEventsListen = true;
	std::string str;

	while (isEventsListen)
	{
		EventHandler();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void UserInput::DeviceHandler::EventHandler()
{
	keyEventHandler.CheckState();
	while (auto e = keyEventHandler.PoolEvent())
	{
		if (e->key != VK_LBUTTON && e->key != VK_RBUTTON)
			emit KeyButtonEvent(e);
	}
	while (SDL_PollEvent(&SDLevent))
	{
		switch (SDLevent.type)
		{
		case SDL_JOYDEVICEREMOVED:
			RemoveJoystic(SDLevent.jdevice.which);
			emit JoyDeviceRemoved();
			break;

		case SDL_JOYDEVICEADDED:
			AddJoystic(SDLevent.jdevice.which);
			emit JoyDeviceAdded();
			break;

		case SDL_JOYBUTTONDOWN:
			emit JoyButtonDown(SDLevent.jbutton);
			break;

		case SDL_JOYBUTTONUP:
			emit JoyButtonUp(SDLevent.jbutton);
			break;

		case SDL_JOYAXISMOTION:
			emit AxisMotion(SDLevent.jaxis);
			break;

		case SDL_QUIT:
			break;
		default:
			isEventsListen = true;
			break;
		}
	}
}

int UserInput::DeviceHandler::GetAxesCount(std::shared_ptr<IJoyDescriptor> joy)
{
	return joy.get()==nullptr?0: SDL_JoystickNumAxes(joy->GetJoystic());
}
