 #pragma once
#include <QtWidgets/QMainWindow>
#include <memory>
#include <QStringListModel>
#include <qstandarditemmodel.hpp>
#include "SDL/SDL.hpp"
#include "AxisAction.hpp"
#include "IJoyDescriptor.hpp"
#include "ButtonAction.hpp"
#include "DialogWaitKey.hpp"
#include "IDeviceHandler.hpp"
#include "JoyAxisContainer.hpp"
#include "ButtonActionContainer.hpp"
#include "IInputController.hpp"
#include "IInputConfigManager.hpp"
#include "UserInputInputs.hpp"
#include "ui_UI.hpp"
#include "JoyWidget.hpp"
#include "DialogAxisBinding.hpp"
#include "ButtonActionItemModel.hpp"
#include "AxisActionItemModel.hpp"
#include "JoyAxisTransporter.hpp"

class UI : public QMainWindow
{
	Q_OBJECT
public:
	UI(QWidget* parent = nullptr);

	~UI();

public slots:
	void onKeyActionTableClicked(const QModelIndex& index);
	void onAxisActionTableClicked(const QModelIndex& index);

	void JoyDeviceRemoved();
	void JoyDeviceAdded();
	void JoyButtonDown(SDL_JoyButtonEvent joyButtonEvent);
	void JoyButtonUp(SDL_JoyButtonEvent joyButtonEvent);
	void KeyButtonEvent(std::shared_ptr< KeyDescriptor> keyDescriptor);
	void AxisMotion(SDL_JoyAxisEvent joyAxisEvent);

	void ButtonActionsUpdate();
	void AxisActionsUpdate();

	void UpdateAxisActionBinding(UserInput::JoyAxisContainer& joyAxisContainer);

	void ApplyChanges();

private:
	Ui::UIClass ui;

	std::shared_ptr< UserInput::IInputController> inputController;
	std::shared_ptr< UserInput::IDeviceHandler> deviceHandler;

	QStringListModel* listDevicesModel = nullptr;

	std::shared_ptr< UserInput::Managers::IInputConfigManager> inputConfigManager;

	void CreateDevicesView();
	void UpdateDevicesList();
	void CreateButtonActionView();
	void CreateAxisActionView();
	void CreateButtonActionsItemModel();
	void CreateAxisActionsItemModel();

	std::list<std::shared_ptr< IJoyDescriptor>> memoryJoyDescriptor;

	ButtonActionItemModel* buttonActionModel = nullptr;
	AxisActionItemModel* axisActionModel = nullptr;

	DialogWaitKey* waitKeyUI = nullptr;
	DialogAxisBinding* axisBindingUI = nullptr;

	bool isWaitKey = false;
	std::shared_ptr< UserInput::ButtonActionContainer> selectedButtonAction;
	std::shared_ptr< UserInput::JoyAxisContainer> selectedAxisAction;

	UserInput::UserInputInputs in;
}; 
