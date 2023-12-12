#include “UserInputModelShell.hpp”
#include “UserInputControllerMU.hpp”

UserInput::UserInputModelShell::UserInputModelShell(const UserInputInputs& in)
	: Core::BaseModelShell(std::make_shared<UserInput::UserInputControllerMU>(in))
{
}

const UserInput::UserInputOutputs& UserInput::UserInputModelShell::GetOutputs()
{
	return (static_cast <UserInput::UserInputControllerMU*> (currentModel.get()))->GetOutputs();
}

const std::shared_ptr<UserInput::Managers::IinputConfigManager> UserInput::UserInputModelShell::GetInputConfigManager()
{
	return static_cast<UserInput::UserInputControllerMU*>(currentModel.get())->GetInputConfigManager();
} 
