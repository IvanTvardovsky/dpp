#pragma once
#include "JoyDescriptor.hpp"
#include "KeyEventHandler.hpp"
#include <list>
#include <memory>
#include <vector>
#include "IDeviceHandler.hpp"

namespace UserInput {
	class DeviceHandler :
		public IDeviceHandler
	{
		Q_OBJECT
		Q_INTERFACES(UserInput::IDeviceHandler)

	public:
		DeviceHandler(QObject* parent = nullptr);
		~DeviceHandler();

		const std::list < std::shared_ptr<IJoyDescriptor>>& GetJoysDescriptors();
		const std::shared_ptr<IJoyDescriptor> GetDescriptorById(int id);
		void StartListenEvents();
		int GetAxesCount(std::shared_ptr<IJoyDescriptor> joy);

	signals:
		void JoyDeviceRemoved();
		void JoyDeviceAdded();
		void JoyButtonDown(SDL_JoyButtonEvent joyButtonEvent);
		void JoyButtonUp(SDL_JoyButtonEvent joyButtonEvent);
		void KeyButtonEvent(std::shared_ptr< KeyDescriptor> keyDescriptor);
		void AxisMotion(SDL_JoyAxisEvent joyAxisEvent);

	private:
		std::list < std::shared_ptr< JoyDescriptor>> joysDescriptors;
		std::list < std::shared_ptr< IJoyDescriptor>> IJoysDescriptors;

		bool isEventsListen = false;
		SDL_Event SDLevent;
		KeyEventHandler keyEventHandler;
		void AddJoystic(Sint32 which);
		void RemoveJoystic(Sint32 which);
		void UpdateEvents();
		void EventHandler();
	};
} 
