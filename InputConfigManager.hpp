#pragma once
#include “ActionController.hpp”
#include “AxisCfg.hpp”
#include <memory>
#include <vector>
#include “IinputConfigManager.hpp”
#include “IdeviceHandler.hpp”
#include “ButtonActionContainer.hpp”
#include “JoyAxisContainer.hpp”

namespace UserInput {
	namespace Managers
	{
		class InputConfigManager :
			public IinputConfigManager
		{

			Q_OBJECT
			Q_INTERFACES(UserInput::Managers::IinputConfigManager)

		signals:
			void AxisActionsUpdated();
			void ButtonActionsUpdated();

		public:
	InputConfigManager(std::shared_ptr<DeviceHandler> devHandler);
			~InputConfigManager();
			std::shared_ptr< ActionController> GetConstructedActionController();

			std::shared_ptr<IdeviceHandler> GetDeviceHandler();

			void Load();
			void StdSave();
			bool LoadFromFile(std::string Filename);
			void SaveToFile(std::string Filename);

			void AddAxisAction(const JoyAxisContainer& container);
			void RemoveAxisAction(const JoyAxisContainer& container);
			void UpdateAxisAction(const JoyAxisContainer& container);
			void AddButtonAction(const ButtonActionContainer& container);
			void RemoveButtonAction(const ButtonActionContainer& container);
			void UpdateButtonAction(const ButtonActionContainer& container);
			void ApplyChanges();

			std::shared_ptr < std::vector<double>> GetAxesValues(int id);
			void UpdateAxes();
			void CloseAxes();

			void SetAxisDependency(AxisCfg axisCfg);

			const std::list<std::shared_ptr< JoyDescriptor>>& GetJoysDescriptors();

			const std::shared_ptr<std::list < JoyAxisContainer>> GetAxisActions();
			const std::shared_ptr<std::list < ButtonActionContainer>> GetButtonActions();

		private:
			std::shared_ptr< ActionController> actionController;

			std::shared_ptr<DeviceHandler> deviceHandler;

			std::string StdCfgFilename = “BindingCfgFile”;
			std::string LastFilename = “”;
			bool isEventsListen = false;

			bool CheckInput(std::shared_ptr < std::list < std::string>> loadedDevices);
			std::shared_ptr< ActionController> LoadDefaultConfig();
			std::shared_ptr < QjsonObject> CreateJsonOutput();
			SDL_Event SDLevent;
			SDL_Joystick* joy;

			std::shared_ptr<std::vector<double>> currentAxexVal;

		};
	}
}
