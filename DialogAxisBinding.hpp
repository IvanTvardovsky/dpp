#pragma once
#include <QDialog>
#include "ui_DialogAxisBinding.hpp"
#include "IDeviceHandler.hpp"
#include "AxisAction.hpp"
#include "JoyWidget.pph"
#include <vector>
#include "JoyAxisContainer.hpp"
#include "DevicesItemModel.hpp"
#include "JoyAxisTransporter.hpp"

class DialogAxisBinding : public QDialog
{
	Q_OBJECT

public:
	DialogAxisBinding(std::shared_ptr< UserInput::IDeviceHandler> devHandler, QWidget* parent = nullptr);
	void Init(const UserInput::JoyAxisContainer& joyAxisContainer);
	~DialogAxisBinding();
	void AxisMoved(const JoyAxisTransporter& joyAxisTransporter);

signals:
	void UpdateAxisActionBinding(UserInput::JoyAxisContainer& joyAxisContainer);

public slots:
	void JoyDeviceRemoved();
	void JoyDeviceAdded();

	void reject();
	void accept();

	void BindAxis(int joyAxisId);
	void SelectedDeviceChanged(int index);
private:
	void ClearListAxis();
	void UpdateListAxis();
	void UpdateAxisBindings();
	void UpdateAxisWidgetArea();
	void SelectJoyDescriptor(std::shared_ptr < IJoyDescriptor> joyDescriptor);
	void UpdateDevices();
	Ui::DialogAxisBindingClass ui;
	std::shared_ptr< UserInput::IDeviceHandler> deviceHandler;

	std::vector<std::shared_ptr<JoyWidget>> listJoyAxis;
	UserInput::JoyAxisContainer selectedJoyAction;
	DevicesItemModel* devicesModel = nullptr;
};
