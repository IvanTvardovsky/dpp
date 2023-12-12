#pragma once
#include <list>
#include "JoyAxisContainer.hpp"
#include "ButtonActionContainer.hpp"
#include "SDL/SDL.h"
#include "ButtonAction.hpp"
#include "AxisAction.hpp"
#include <memory>
#include "KeyEventHandler.hpp"
#include <qobject.h>
#include <Windows.h>
#include <conio.h>
#include "DeviceHandler.hpp"

namespace UserInput {
	class ActionController
		:public QObject
	{
		Q_OBJECT

		std::shared_ptr < std::list < JoyAxisContainer>> axisActions;
		std::shared_ptr < std::list < ButtonActionContainer>> buttonActions;

		std::shared_ptr < DeviceHandler> deviceHandler;

	public slots:
		void CheckActionsJoystick();

		void UpdateKeyButtonEvent(std::shared_ptr<KeyDescriptor> keyDescriptor);
		void UpdateJoyButtonEvent(SDL_JoyButtonEvent joyButtonEvent);
		void UpdateJoyAxisEvent(SDL_JoyAxisEvent joyAxisEvent);

	public:
		ActionController(std::shared_ptr<DeviceHandler> devHandler, QObject* parent = nullptr);

		bool AddAxisAction(const JoyAxisContainer& container);
		bool RemoveAxisAction(const JoyAxisContainer& container);
		bool UpdateAxisAction(const JoyAxisContainer& container);
		bool AddButtonAction(const ButtonActionContainer& container);
		bool RemoveButtonAction(const ButtonActionContainer& container);
		bool UpdateButtonAction(const ButtonActionContainer& container);

		std::shared_ptr<std::list < JoyAxisContainer>> GetAxisActions();
		std::shared_ptr<std::list < ButtonActionContainer>> GetButtonActions();

		bool GetActionState(ButtonAction action);
		double GetAxisValue(AxisAction action);
	};

}
