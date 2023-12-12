#pragma once
#include "ButtonAction.hpp"
#include <string>
#include "IJoyDescriptor.hpp"
#include <memory>

namespace UserInput
{
	class ButtonActionContainer
	{
	public:
		ButtonAction action = ButtonAction::BA_NONE;
		bool isReaded = true;
		int buttonId = -1;
		std::shared_ptr< IJoyDescriptor> joyDescriptor;
		bool isPressed = false;

		bool operator ==(const ButtonActionContainer& p) const
		{
			return action == p.action &&
				isReaded == p.isReaded &&
				buttonId == p.buttonId &&
				joyDescriptor == p.joyDescriptor &&
				isPressed == p.isPressed;
		}
		ButtonActionContainer& operator =(const ButtonActionContainer& p)
		{
			action = p.action;
			isReaded = p.isReaded;
			buttonId = p.buttonId;
			joyDescriptor = p.joyDescriptor;
			isPressed = p.isPressed;
			return *this;
		}
	};
}
