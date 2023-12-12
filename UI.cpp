#include "UI.hpp"
#include "UserInputModelShell.hpp"
#include <thread>

using namespace UserInput;

UI::UI(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	auto tmp = std::make_shared< UserInput::UserInputModelShell>(in);
	inputController = std::static_pointer_cast<UserInput::IInputController>(tmp);
	inputConfigManager = inputController->GetInputConfigManager();
	deviceHandler = inputConfigManager->GetDeviceHandler();

	CreateDevicesView();

	CreateButtonActionView();
	CreateAxisActionView();


	waitKeyUI = new DialogWaitKey(this);
	waitKeyUI->setModal(true);
	axisBindingUI = new DialogAxisBinding(deviceHandler, this);
	axisBindingUI->setModal(true);

	QObject::connect(deviceHandler.get(), SIGNAL(JoyDeviceRemoved()), this, SLOT(JoyDeviceRemoved()));
	QObject::connect(deviceHandler.get(), SIGNAL(JoyDeviceAdded()), this, SLOT(JoyDeviceAdded()));
	QObject::connect(deviceHandler.get(), SIGNAL(JoyButtonDown(SDL_JoyButtonEvent)), this, SLOT(JoyButtonDown(SDL_JoyButtonEvent)));
	QObject::connect(deviceHandler.get(), SIGNAL(JoyButtonUp(SDL_JoyButtonEvent)), this, SLOT(JoyButtonUp(SDL_JoyButtonEvent)));
	QObject::connect(deviceHandler.get(), SIGNAL(KeyButtonEvent(std::shared_ptr< KeyDescriptor>)), this, SLOT(KeyButtonEvent(std::shared_ptr< KeyDescriptor>)));
	QObject::connect(deviceHandler.get(), SIGNAL(AxisMotion(SDL_JoyAxisEvent)), this, SLOT(AxisMotion(SDL_JoyAxisEvent)));

	QObject::connect(inputConfigManager.get(), SIGNAL(ButtonActionsUpdated()), this, SLOT(ButtonActionsUpdate()));
	QObject::connect(inputConfigManager.get(), SIGNAL(AxisActionsUpdated()), this, SLOT(AxisActionsUpdate()));

	QObject::connect(axisBindingUI, SIGNAL(UpdateAxisActionBinding(UserInput::JoyAxisContainer&)), this, SLOT(UpdateAxisActionBinding(UserInput::JoyAxisContainer&)));
}

void UI::CreateDevicesView()
{
	listDevicesModel = new QStringListModel();
	ui.ListDevices_listView->setModel(listDevicesModel);
	UpdateDevicesList();
}

void UI::CreateButtonActionView()
{
	CreateButtonActionsItemModel();
	ui.Key_tableView->setModel(buttonActionModel);
	ui.Key_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UI::CreateAxisActionView()
{
	CreateAxisActionsItemModel();
	ui.Axis_tableView->setModel(axisActionModel);
	ui.Axis_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

UI::~UI()
{
	if (listDevicesModel != nullptr)
		delete listDevicesModel;
	if (waitKeyUI != nullptr)
		delete waitKeyUI;

}
void UI::JoyDeviceRemoved()
{
	UpdateDevicesList();
	axisBindingUI->JoyDeviceRemoved();

}

void UI::JoyDeviceAdded()
{
	UpdateDevicesList();
	axisBindingUI->JoyDeviceAdded();
}

void UI::JoyButtonDown(SDL_JoyButtonEvent joyButtonEvent)
{
	if (isWaitKey && selectedButtonAction != nullptr)
	{
		selectedButtonAction->isPressed = false;
		selectedButtonAction->isReaded = true;
		selectedButtonAction->joyDescriptor = deviceHandler->GetDescriptorById(joyButtonEvent.which);
		selectedButtonAction->buttonId = joyButtonEvent.button;

		inputConfigManager->UpdateButtonAction(*selectedButtonAction.get());
		isWaitKey = false;
		selectedButtonAction = nullptr;
		waitKeyUI->hide();
	}
}

void UI::JoyButtonUp(SDL_JoyButtonEvent joyButtonEvent)
{
}

void UI::KeyButtonEvent(std::shared_ptr<KeyDescriptor> keyDescriptor)
{
	if (isWaitKey && selectedButtonAction != nullptr)
	{
		if (keyDescriptor->isPressed)
		{
			selectedButtonAction->isPressed = false;
			selectedButtonAction->isReaded = true;
			selectedButtonAction->buttonId = keyDescriptor->key;
			selectedButtonAction->joyDescriptor = nullptr;
			inputConfigManager->UpdateButtonAction(*selectedButtonAction.get());
			isWaitKey = false;
			selectedButtonAction = nullptr;
			waitKeyUI->hide();
		}
	}
}

void UI::AxisMotion(SDL_JoyAxisEvent joyAxisEvent)
{
	JoyAxisTransporter transporter;
	transporter.descriptor= deviceHandler->GetDescriptorById(joyAxisEvent.which);
	transporter.id = joyAxisEvent.axis;
	transporter.value = joyAxisEvent.value;
	axisBindingUI->AxisMoved(transporter);
}

void UI::ButtonActionsUpdate()
{
	auto& buttonActions = *inputConfigManager->GetButtonActions();
	for (auto item : buttonActions)
	{
		if (item.action != ButtonAction::BA_NONE)
		{
			buttonActionModel->UpdateButtonAction(item);
		}
	}
}
void UI::AxisActionsUpdate()
{
	auto& axisActions = *inputConfigManager->GetAxisActions();
	for (auto item : axisActions)
	{
		if (item.action != AxisAction::AA_NONE)
		{
			axisActionModel->UpdateAxisAction(item);
		}
	}
}

void UI::UpdateAxisActionBinding(UserInput::JoyAxisContainer& joyAxisContainer)
{
	auto& axisActions = *inputConfigManager->GetAxisActions();
	for (auto item : axisActions)
	{
		if (item.action == joyAxisContainer.action)
		{
			inputConfigManager->UpdateAxisAction(joyAxisContainer);
		}
	}
}

void UI::ApplyChanges()
{
	inputConfigManager->ApplyChanges();
}
void UI::onAxisActionTableClicked(const QModelIndex& index)
{
	selectedAxisAction = std::make_shared<UserInput::JoyAxisContainer>(axisActionModel->GetActionByIndex(index.row()));

	axisBindingUI->Init(*selectedAxisAction.get());
	axisBindingUI->show();
}

void UI::onKeyActionTableClicked(const QModelIndex& index)
{
	isWaitKey = true;
	selectedButtonAction = std::make_shared<UserInput::ButtonActionContainer>(buttonActionModel->GetActionByIndex(index.row()));
	waitKeyUI->show();
}

void UI::UpdateDevicesList()
{
	auto joysDescriptors = deviceHandler->GetJoysDescriptors();
	if (!(memoryJoyDescriptor == joysDescriptors))
	{
		QString str("%1 %2");
		memoryJoyDescriptor.clear();
		listDevicesModel->removeRows(0, listDevicesModel->rowCount());

		listDevicesModel->insertRow(listDevicesModel->rowCount());
		listDevicesModel->setData(listDevicesModel->index(listDevicesModel->rowCount() - 1), "Index\tName");
		for (auto it : joysDescriptors)
		{
			memoryJoyDescriptor.push_back(it);
			listDevicesModel->insertRow(listDevicesModel->rowCount());
			QModelIndex index = listDevicesModel->index(listDevicesModel->rowCount() - 1);
			listDevicesModel->setData(index, str.arg(it->GetIndex()).arg(QString::fromStdString(it->GetName())));
		}
		ui.ListDevices_listView->update();
	}
}

void UI::CreateButtonActionsItemModel()
{
	buttonActionModel = new ButtonActionItemModel();
	auto& buttonActions = *inputConfigManager->GetButtonActions();
	for (auto item : buttonActions)
	{
		if (item.action != ButtonAction::BA_NONE)
		{
			buttonActionModel->AddButtonAction(item);
		}
	}
}

void UI::CreateAxisActionsItemModel()
{
	axisActionModel = new AxisActionItemModel();
	auto& axisActions = *inputConfigManager->GetAxisActions();
	for (auto item : axisActions)
	{
		if (item.action != AxisAction::AA_NONE)
		{
			axisActionModel->AddAxisAction(item);
		}
	}
} 
