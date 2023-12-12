#pragma once
#include “UserInputInputs.hpp”
#include “UserInputOutputs.hpp”
#include “ButtonAction.hpp”
#include “ActionController.hpp”
#include “InputConfigManager.hpp”
#include “IinputController.hpp”
#include “DeviceHandler.hpp”

namespace UserInput {
	class UserInputControllerMU :
		public Core::BaseModelUnit, IinputController

	{
	private:
		const UserInputInputs& in;
		UserInputOutputs out;

		std::shared_ptr <DeviceHandler> deviceHandler;
		std::shared_ptr <ActionController> actController;
		std::shared_ptr <UserInput::Managers::InputConfigManager> configManager;

		

		void Initialize();

	public:

		UserInputControllerMU(const UserInputInputs& in);
		const Core::IIOInterface& GetInputInterface() const;
		const Core::IIOInterface& GetOutputInterface() const;

		void Dispose();
		void Step(double dT);
		void Reset();
		const UserInputOutputs& GetOutputs() const;
		const std::shared_ptr <UserInput::Managers::IinputConfigManager> GetInputConfigManager();
	};
