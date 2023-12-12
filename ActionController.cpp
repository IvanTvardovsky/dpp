#include "ActionController.hpp"
#include "MathLibrary.hpp"
using namespace UserInput;

UserInput::ActionController::ActionController(std::shared_ptr<DeviceHandler> devHandler, QObject* parent)
	:QObject(parent)
{
	deviceHandler = devHandler;
	axisActions = std::make_shared<std::list<JoyAxisContainer>>();
	buttonActions = std::make_shared<std::list<ButtonActionContainer>>();
}

bool UserInput::ActionController::AddAxisAction(const JoyAxisContainer& container)
{
	bool result = true;
	axisActions->push_back(container);
	return result;
}

bool UserInput::ActionController::RemoveAxisAction(const JoyAxisContainer& container)
{
	bool result = false;
	for (auto item : *axisActions)
	{
		if (item.action == container.action)
		{
			axisActions->remove(item);
			result = true;
			break;
		}
	}
	return result;
}

bool UserInput::ActionController::UpdateAxisAction(const JoyAxisContainer& container)
{
	bool result = false;
	for (auto& item : *axisActions)
	{
		if (item.action == container.action)
		{
			item.axisId = container.axisId;
			item.joyDescriptor = container.joyDescriptor;
			item.extMinVal = container.extMinVal;
			item.extMaxVal = container.extMaxVal;
			item.deadZoneMinVal = container.deadZoneMinVal;
			item.deadZoneMaxVal = container.deadZoneMaxVal;
			result = true;
			break;
		}
	}
	return result;
}

bool UserInput::ActionController::AddButtonAction(const ButtonActionContainer& container)
{
	bool result = true;
	buttonActions->push_back(container);
	return result;
}

bool UserInput::ActionController::RemoveButtonAction(const ButtonActionContainer& container)
{
	bool result = false;
	for (auto item : *buttonActions.get())
	{
		if (item.action == container.action)
		{
			buttonActions->remove(item);
			result = true;
			break;
		}
	}
	return result;
}

bool UserInput::ActionController::UpdateButtonAction(const ButtonActionContainer& container)
{
	bool result = false;
	for (auto& item : *buttonActions.get())
	{
		if (item.action == container.action)
		{
			item.joyDescriptor = container.joyDescriptor;
			item.buttonId = container.buttonId;
			result = true;
			break;
		}
	}
	return result;
}

std::shared_ptr<std::list<JoyAxisContainer>> UserInput::ActionController::GetAxisActions()
{
	return axisActions;
}

std::shared_ptr<std::list<ButtonActionContainer>> UserInput::ActionController::GetButtonActions()
{
	return buttonActions;
}

void UserInput::ActionController::CheckActionsJoystick()
{
	auto joysDescriptors = deviceHandler->GetJoysDescriptors();
	for (auto item : *axisActions.get())
	{
		for (auto descriptor : joysDescriptors)
		{
			if (item.joyDescriptor != descriptor)
			{
				axisActions->remove(item);
				break;
			}
		}
	}
	for (auto item : *buttonActions.get())
	{
		if (item.joyDescriptor != nullptr && !item.joyDescriptor->isEmpty())
		{
			for (auto descriptor : joysDescriptors)
			{
				if (item.joyDescriptor != descriptor)
				{
					buttonActions->remove(item);
					break;
				}
			}
		}
	}
}

void UserInput::ActionController::UpdateKeyButtonEvent(std::shared_ptr<KeyDescriptor> keyDescriptor)
{
	for (auto item : *buttonActions.get())
	{
		if (item.joyDescriptor != nullptr && !item.joyDescriptor->isEmpty())
		{
			if (item.buttonId != -1)
			{
				if (item.buttonId == keyDescriptor->key)
				{
					if (!keyDescriptor->isPressed)
					{
						if (item.isReaded)
						{
							item.isPressed = false;
							item.isReaded = false;
						}
					}
					else
					{
						item.isPressed = true;
						item.isReaded = false;
					}
					break;
				}
			}
		}
	}
}

void UserInput::ActionController::UpdateJoyButtonEvent(SDL_JoyButtonEvent joyButtonEvent)
{
	for (auto item : *buttonActions.get())
	{
		if (item.joyDescriptor != nullptr && !item.joyDescriptor->isEmpty())
		{
			if (item.joyDescriptor->GetIndex() == joyButtonEvent.which)
			{
				if (item.buttonId == joyButtonEvent.button)
				{
					if (joyButtonEvent.type == SDL_JOYBUTTONUP)
					{
						if (item.isReaded)
						{
							item.isPressed = false;
							item.isReaded = false;
						}
					}
					else
					{
						item.isPressed = true;
						item.isReaded = false;
					}
					break;
				}
			}
		}
	}
}

void UserInput::ActionController::UpdateJoyAxisEvent(SDL_JoyAxisEvent joyAxisEvent)
{
	for (auto item : *axisActions.get())
	{
		if (item.joyDescriptor->GetIndex() == joyAxisEvent.which)
		{
			if (item.axisId == joyAxisEvent.axis)
			{
				item.value = joyAxisEvent.value;
				break;
			}
		}
	}
}

bool UserInput::ActionController::GetActionState(ButtonAction action)
{
	double result = false;
	for (auto item : *buttonActions.get())
	{
		if (item.action == action)
		{
			result = item.isPressed;
			item.isReaded = true;
			break;
		}
	}
	return result;
}

double UserInput::ActionController::GetAxisValue(AxisAction action)
{
	double result = 0;
	for (auto item : *axisActions.get())
	{
		if (item.action == action)
		{
			result = (item.value + 32768) * (item.extMaxVal - item.extMinVal) / 65535 + item.extMinVal;
			result = MathLibrary::DeadZone(result, item.deadZoneMaxVal, item.deadZoneMinVal);
			break;
		}
	}
	return result;
}
