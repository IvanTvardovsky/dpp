#pragma once

#ifdef USERINPUT_EXPORTS
#define USERINPUT_API __declspec(dllexport) 
#else
#define USERINPUT_API __declspec(dllimport) 
#endif
#include “BaseModelShell.hpp”
#include “UserInputInputs.hpp”
#include “UserInputOutputs.hpp”
#include “IInputController.hpp”

namespace UserInput {
	class USERINPUT_API UserInputModelShell :
		public Core::BaseModelShell, public IinputController
	{
	public:
		UserInputModelShell(const UserInputInputs& in);

		~UserInputModelShell();
		const UserInputOutputs& GetOutputs();
		const std::shared_ptr <UserInput::Managers::IinputConfigManager> GetInputConfigManager()
	};
} 
