#pragma once
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <Qfile>
#include <Qdir>
#include <QjsonObject>
#include <QjsonArray>
#include <QjsonDocument>
#include <thread>
#include “ButtonAction.hpp”
#include “AxisCfg.hpp”
#include “JoyAxisContainer.hpp”
#include “ButtonActionContainer.hpp”
#include “IdeviceHandler.hpp”
#include “pch.h”
#include “InputConfigManager.h”



#ifdef USERINPUT_EXPORTS
#define USERINPUT_API __declspec(dllexport) 
#else
#define USERINPUT_API __declspec(dllimport) 
#endif

namespace UserInput {
	namespace Managers
	{
		class USERINPUT_API IinputConfigManager :
			public Qobject
		{

		public:
			virtual std::shared_ptr<IdeviceHandler> GetDeviceHandler() = 0;
			virtual bool LoadFromFile(std::string Filename) = 0;
			virtual void SaveToFile(std::string Filename) = 0;

			virtual void AddAxisAction(const JoyAxisContainer& container) = 0;
			virtual void RemoveAxisAction(const JoyAxisContainer& container) = 0;
			virtual void UpdateAxisAction(const JoyAxisContainer& container) = 0;
			virtual void AddButtonAction(const ButtonActionContainer& container) = 0;
			virtual void RemoveButtonAction(const ButtonActionContainer& container) = 0;
			virtual void UpdateButtonAction(const ButtonActionContainer& container) = 0;
			virtual void ApplyChanges() = 0;

			//	signals:
			virtual void ButtonActionsUpdated() = 0;
			virtual void AxisActionsUpdated() = 0;


			virtual const std::list<std::shared_ptr< JoyDescriptor>>& GetJoysDescriptors() = 0;

			virtual std::shared_ptr < std::vector<double>> GetAxesValues(int id) = 0;
			virtual void UpdateAxes() = 0;
			virtual void CloseAxes() = 0;

			virtual void SetAxisDependency(AxisCfg axisCfg) = 0;

			virtual const std::shared_ptr<std::list < JoyAxisContainer>> GetAxisActions() = 0;
			virtual const std::shared_ptr<std::list < ButtonActionContainer>> GetButtonActions() = 0;

		};
	}
}

Q_DECLARE_INTERFACE(UserInput::Managers:: IinputConfigManager, “QinputLibrary”)

using namespace UserInput;
using namespace UserInput::Managers;

bool UserInput::Managers::InputConfigManager::CheckInput(std::shared_ptr < std::list < std::string>> loadedDevices)
{
	std::list<std::string> tmp;
	auto joyDescriptors = deviceHandler->GetJoysDescriptors();
	for (auto it : joyDescriptors)
	{
		tmp.push_back(it->GetName());
	}
	return *loadedDevices.get() == tmp;
}

std::shared_ptr<ActionController> UserInput::Managers::InputConfigManager::LoadDefaultConfig()
{
	auto controller = std::make_shared<ActionController>(deviceHandler);

	for (int i = 0; i < ButtonActionConv::size(); i++)
	{
		ButtonActionContainer container;
		container.action = ButtonActionConv::At(i);
		controller->AddButtonAction(container);
	}

	for (int i = 0; i < AxisActionConv::size(); i++)
	{
		JoyAxisContainer container;
		container.action = AxisActionConv::At(i);
		controller->AddAxisAction(container);
	}
	return controller;
}
UserInput::Managers::InputConfigManager::InputConfigManager(std::shared_ptr<DeviceHandler> devHandler)
{
	deviceHandler = devHandler;
	Load();
}

UserInput::Managers::InputConfigManager::~InputConfigManager()
{

}

std::shared_ptr<ActionController> UserInput::Managers::InputConfigManager::GetConstructedActionController()
{
	Load();
	return actionController;
}

std::shared_ptr<IdeviceHandler> UserInput::Managers::InputConfigManager::GetDeviceHandler()
{
	return deviceHandler;
}

void UserInput::Managers::InputConfigManager::Load()
{
	Qstring val;
	Qfile file;
	file.setFileName(Qstring::fromStdString(StdCfgFilename));
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	val = file.readAll();
	file.close();

	QjsonParseError error;
	QjsonDocument doc = QjsonDocument::fromJson(val.toUtf8(), &error);

	std::string filepath;
	if (doc.isObject())
	{
		QjsonObject json = doc.object();
		filepath = (json[“LastConfigPath”].toString()).toStdString();
	}

	bool isLoaded = false;
	if (filepath != “”)
	{
		isLoaded = LoadFromFile(filepath);
	}
	else
	{
		isLoaded = LoadFromFile(StdCfgFilename);
	}

	if (!isLoaded)
	{
		actionController = LoadDefaultConfig();
	}
}

void UserInput::Managers::InputConfigManager::StdSave()
{
	auto json = CreateJsonOutput();

	json->insert(“LastConfigPath”, Qstring::fromStdString(LastFilename));

	QjsonDocument doc(*json.get());
	Qstring jsonString = doc.toJson(QjsonDocument::Indented);
	Qfile file;
	file.setFileName(Qstring::fromStdString(StdCfgFilename));
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QtextStream stream(&file);
	stream << jsonString;
	file.close();
}

bool UserInput::Managers::InputConfigManager::LoadFromFile(std::string Filename)
{
	bool result = true;
	std::shared_ptr< ActionController> controller = std::make_shared<ActionController>(deviceHandler);

	Qstring val;
	Qfile file;
	file.setFileName(Qstring::fromStdString(Filename));
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	val = file.readAll();
	file.close();

	QjsonParseError error;
	QjsonDocument doc = QjsonDocument::fromJson(val.toUtf8(), &error);

	auto LoadedJoysticNames = std::make_shared < std::list < std::string>>();
	if (doc.isObject())
	{
		QjsonObject json = doc.object();
		QjsonArray jsonArray;

		Qlist<Qvariant> joyNames = json[“Joystics”].toArray().toVariantList();

		foreach(const Qvariant & value, joyNames)
		{

			std::string tmpStr;
			tmpStr = value.toString().toStdString();
			LoadedJoysticNames->push_back(tmpStr);
		}

		result = CheckInput(LoadedJoysticNames);
		jsonArray = json[“axisActions”].toArray();

		foreach(const QjsonValue & value, jsonArray)
		{
			if (value.isObject())
			{
				JoyAxisContainer container;

				QjsonObject obj = value.toObject();
				container.action = static_cast <UserInput::AxisAction> (obj[“action”].toInt());
				container.axisId = obj[“axisId”].toInt();
				container.value = obj[“value”].toDouble();
				container.minVal = obj[“minVal”].toDouble();
				container.maxVal = obj[“maxVal”].toDouble();
				container.extMinVal = obj[“extMinVal”].toDouble();
				container.extMaxVal = obj[“extMaxVal”].toDouble();
				container.deadZoneMinVal = obj[“deadZoneMinVal”].toDouble();
				container.deadZoneMaxVal = obj[“deadZoneMaxVal”].toDouble();

				auto descriptor = deviceHandler->GetDescriptorById(obj[“joyId”].toInt());
				if (descriptor != nullptr)
				{
					if (obj[“joyName”].toString().toStdString() == descriptor->GetName())
					{
						container.joyDescriptor = descriptor;
					}
				}
				controller->AddAxisAction(container);
			}
		}

		jsonArray = json[“buttonActions”].toArray();
		foreach(const QjsonValue & value, jsonArray)
		{
			if (value.isObject())
			{
				ButtonActionContainer container;

				QjsonObject obj = value.toObject();
				container.action = static_cast <UserInput::ButtonAction> (obj[“action”].toInt());
				container.isReaded = obj[“isReaded”].toBool();
				container.buttonId = obj[“buttonId”].toInt();
				container.isPressed = obj[“isPressed”].toBool();

				auto descriptor = deviceHandler->GetDescriptorById(obj[“joyId”].toInt());
				if (descriptor != nullptr)
				{
					if (obj[“joyName”].toString().toStdString() == descriptor->GetName())
					{
						container.joyDescriptor = descriptor;
					}
				}

				controller->AddButtonAction(container);
			}
		}
		if (controller->GetAxisActions()->size() != AxisActionConv::size() ||
			controller->GetButtonActions()->size() != ButtonActionConv::size())
		{
			result = false;
		}
	}

	if (result)
	{
		actionController = controller;

	}

	return result;
}

void UserInput::Managers::InputConfigManager::SaveToFile(std::string Filename)
{
	auto json = CreateJsonOutput();

	QjsonDocument doc(*json.get());
	Qstring jsonString = doc.toJson(QjsonDocument::Indented);
	Qfile file;
	file.setFileName(Qstring::fromStdString(Filename));
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QtextStream stream(&file);
	stream << jsonString;
	file.close();

}

std::shared_ptr < QjsonObject> UserInput::Managers::InputConfigManager::CreateJsonOutput()
{
	std::shared_ptr<QjsonObject> json = std::make_shared<QjsonObject>();
	QjsonArray jsonArr;

	auto joyDescriptors = deviceHandler->GetJoysDescriptors();
	for (auto item : joyDescriptors)
	{
		jsonArr.append(Qstring::fromStdString(item->GetName()));
	}
	json->insert(“Joystics”, jsonArr);

	jsonArr = QjsonArray();
	auto& axisActions = *actionController->GetAxisActions();
	for (auto& item : axisActions)
	{
		QjsonObject tmpObj;
		tmpObj.insert(“action”, QjsonValue::fromVariant((int)item.action));
		tmpObj.insert(“axisId”, QjsonValue::fromVariant(item.axisId));
		if (item.joyDescriptor != nullptr && !item.joyDescriptor->isEmpty())
		{
			tmpObj.insert(“joyId”, QjsonValue::fromVariant(item.joyDescriptor->GetIndex()));
			tmpObj.insert(“joyName”, Qstring::fromStdString(item.joyDescriptor->GetName()));
		}

		tmpObj.insert(“minVal”, QjsonValue::fromVariant(item.minVal));
		tmpObj.insert(“maxVal”, QjsonValue::fromVariant(item.maxVal));
		tmpObj.insert(“extMinVal”, QjsonValue::fromVariant(item.extMinVal));
		tmpObj.insert(“extMaxVal”, QjsonValue::fromVariant(item.extMaxVal));
		tmpObj.insert(“deadZoneMinVal”, QjsonValue::fromVariant(item.deadZoneMinVal));
		tmpObj.insert(“deadZoneMaxVal”, QjsonValue::fromVariant(item.deadZoneMaxVal));

		jsonArr.append(tmpObj);
	}
	json->insert(“axisActions”, jsonArr);

	jsonArr = QjsonArray();
	auto& buttonActions = *actionController->GetButtonActions();
	for (auto& item : buttonActions)
	{
		QjsonObject tmpObj;
		tmpObj.insert(“action”, QjsonValue::fromVariant((int)item.action));
		tmpObj.insert(“buttonId”, QjsonValue::fromVariant(item.buttonId));
		if (item.joyDescriptor != nullptr && !item.joyDescriptor->isEmpty())
		{
			tmpObj.insert(“joyId”, QjsonValue::fromVariant(item.joyDescriptor->GetIndex()));
			tmpObj.insert(“joyName”, Qstring::fromStdString(item.joyDescriptor->GetName()));
		}
		jsonArr.append(tmpObj);
	}
	json->insert(“buttonActions”, jsonArr);

	return json;
}

void UserInput::Managers::InputConfigManager::AddAxisAction(const JoyAxisContainer& container)
{
	if (actionController->AddAxisAction(container))
	{
		emit AxisActionsUpdated();
	}
}

void UserInput::Managers::InputConfigManager::RemoveAxisAction(const JoyAxisContainer& container)
{
	if (actionController->RemoveAxisAction(container))
	{
		emit AxisActionsUpdated();
	}
}

void UserInput::Managers::InputConfigManager::UpdateAxisAction(const JoyAxisContainer& container)
{
	if (actionController->UpdateAxisAction(container))
	{
		emit AxisActionsUpdated();
	}
}

void UserInput::Managers::InputConfigManager::AddButtonAction(const ButtonActionContainer& container)
{
	if (actionController->AddButtonAction(container))
	{
		emit ButtonActionsUpdated();
	}
}

void UserInput::Managers::InputConfigManager::RemoveButtonAction(const ButtonActionContainer& container)
{
	if (actionController->RemoveButtonAction(container))
	{
		emit ButtonActionsUpdated();
	}
}

void UserInput::Managers::InputConfigManager::UpdateButtonAction(const ButtonActionContainer& container)
{
	if (actionController->UpdateButtonAction(container))
	{
		emit ButtonActionsUpdated();
	}
}

void UserInput::Managers::InputConfigManager::ApplyChanges()
{
	StdSave();
}

const std::shared_ptr<std::list<JoyAxisContainer>> UserInput::Managers::InputConfigManager::GetAxisActions()
{
	return actionController->GetAxisActions();
}

const std::shared_ptr<std::list<ButtonActionContainer>> UserInput::Managers::InputConfigManager::GetButtonActions()
{
	return actionController->GetButtonActions(); }
