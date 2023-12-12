#include “UserInputControllerMU.h”

void UserInput::UserInputControllerMU::Initialize()
{
	deviceHandler = std::make_shared<DeviceHandler>();
	configManager = std::make_shared<UserInput::Managers::InputConfigManager>(deviceHandler);

	actController = configManager->GetConstructedActionController();
}

UserInput::UserInputControllerMU::UserInputControllerMU(const UserInputInputs& in) :in(in)
{
	Initialize();
}

const Core::IIOInterface& UserInput::UserInputControllerMU::GetInputInterface() const
{
	return in;
}

const Core::IIOInterface& UserInput::UserInputControllerMU::GetOutputInterface() const
{
	return out;
}

void UserInput::UserInputControllerMU::Dispose()
{
		configManager->StdSave();
}

void UserInput::UserInputControllerMU::Step(double dT)
{
	out.PU_ASR = actController->GetActionState(ButtonAction::BA_PU_ASR);
	out.PU_ESC = actController->GetActionState(ButtonAction::BA_PU_ESC);
	out.PU_AP = actController->GetActionState(ButtonAction::BA_PU_AP);
	out.PU_H = actController->GetActionState(ButtonAction::BA_PU_H);
	out.PU_GFL = actController->GetActionState(ButtonAction::BA_PU_GFL);
	out.PU_MAIN_MODE = actController->GetActionState(ButtonAction::BA_PU_MAIN_MODE);
	out.PU_FDC = actController->GetActionState(ButtonAction::BA_PU_FDC);
	out.PU_V = actController->GetActionState(ButtonAction::BA_PU_V);
	out.PU_ND_UP = actController->GetActionState(ButtonAction::BA_PU_ND_UP);
	out.PU_ND_DOWN = actController->GetActionState(ButtonAction::BA_PU_ND_DOWN);
	out.PU_FL_UP = actController->GetActionState(ButtonAction::BA_PU_FL_UP);
	out.PU_FL_LDG = actController->GetActionState(ButtonAction::BA_PU_FL_LDG);
	out.FAILURE_AIL_L = actController->GetActionState(ButtonAction::BA_FAILURE_AIL_L);
	out.FAILURE_AIL_R = actController->GetActionState(ButtonAction::BA_FAILURE_AIL_R);
	out.FAILURE_WX = actController->GetActionState(ButtonAction::BA_FAILURE_WX);
	out.FAILURE_WY = actController->GetActionState(ButtonAction::BA_FAILURE_WY);
	out.FAILURE_WZ = actController->GetActionState(ButtonAction::BA_FAILURE_WZ);
	out.FAILURE_ALPHA_MLF = actController->GetActionState(ButtonAction::BA_FAILURE_ALPHA_MLF);
	out.FAILURE_ALPHA_MRF = actController->GetActionState(ButtonAction::BA_FAILURE_ALPHA_MRF);
	out.BUTTON_CC = actController->GetActionState(ButtonAction::BA_BUTTON_CC);
	out.BUTTON_LVL = actController->GetActionState(ButtonAction::BA_BUTTON_LVL);
	out.BUTTON_AP_OFF = actController->GetActionState(ButtonAction::BA_BUTTON_AP_OFF);
	out.BUTTON_GROUND_BRAKE = actController->GetActionState(ButtonAction::BA_BUTTON_GROUND_BRAKE);
	out.BUTTON_SB = actController->GetActionState(ButtonAction::BA_BUTTON_SB);
	out.BUTTON_GEARS_POS = actController->GetActionState(ButtonAction::BA_BUTTON_GEARS_POS);
	out.BUTTON_REPROG = actController->GetActionState(ButtonAction::BA_BUTTON_REPROG);
	out.BUTTON_NK = actController->GetActionState(ButtonAction::BA_BUTTON_NK);
	out.BUTTON_BS = actController->GetActionState(ButtonAction::BA_BUTTON_BS);
	out.BUTTON_MNV = actController->GetActionState(ButtonAction::BA_BUTTON_MNV);

	out.Xv = actController->GetAxisValue(AxisAction::AA_PITCH);
	out.Xail = actController->GetAxisValue(AxisAction::AA_ROLL);
	out.Xrud = actController->GetAxisValue(AxisAction::AA_YAW);
	out.AlphaThrust_L = actController->GetAxisValue(AxisAction::AA_THRUST_LEFT);
	out.AlphaThrust_R = actController->GetAxisValue(AxisAction::AA_THRUST_RIGHT);
}

void UserInput::UserInputControllerMU::Reset()
{
	actController = configManager->GetConstructedActionController();
}

const UserInput::UserInputOutputs& UserInput::UserInputControllerMU::GetOutputs() const
{
	return out;
}

const std::shared_ptr<UserInput::Managers::IinputConfigManager> UserInput::UserInputControllerMU::GetInputConfigManager()
{
	return configManager;
}
