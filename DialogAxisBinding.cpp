#include "DialogAxisBinding.hpp"


DialogAxisBinding::DialogAxisBinding(std::shared_ptr< UserInput::IDeviceHandler> devHandler, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	deviceHandler = devHandler;

	devicesModel = new DevicesItemModel();
	UpdateDevices();
	ui.SelectedDevice_comboBox->setModel(devicesModel);

	QObject::connect(deviceHandler.get(), SIGNAL(JoyDeviceRemoved()), this, SLOT(JoyDeviceRemoved()));
	QObject::connect(deviceHandler.get(), SIGNAL(JoyDeviceAdded()), this, SLOT(JoyDeviceAdded()));
	QObject::connect(deviceHandler.get(), SIGNAL(AxisMotion(SDL_JoyAxisEvent)), this, SLOT(AxisMotion(SDL_JoyAxisEvent)));

}

void DialogAxisBinding::Init(const UserInput::JoyAxisContainer& joyAxisContainer)
{
	selectedJoyAction = joyAxisContainer;
	ui.ActionName_label->setText(QString::fromStdString(UserInput::AxisActionConv::AxisActionToStr(selectedJoyAction.action)));

	ui.ExtMin_lineEdit->setText(QString::number(selectedJoyAction.extMinVal));
	ui.ExtMax_lineEdit->setText(QString::number(selectedJoyAction.extMaxVal));
	ui.DeadZoneMin_lineEdit->setText(QString::number(selectedJoyAction.deadZoneMinVal));
	ui.DeadZoneMax_lineEdit->setText(QString::number(selectedJoyAction.deadZoneMaxVal));

	if (selectedJoyAction.joyDescriptor == nullptr || selectedJoyAction.joyDescriptor->isEmpty())
	{
		SelectJoyDescriptor(nullptr);
		ui.SelectedDevice_comboBox->setCurrentIndex(0);
	}
	else
	{
		SelectJoyDescriptor(selectedJoyAction.joyDescriptor);
		ui.SelectedDevice_comboBox->setCurrentIndex(devicesModel->GetIdByDevice(selectedJoyAction.joyDescriptor));
	}
//	UpdateAxisWidgetArea();
}

DialogAxisBinding::~DialogAxisBinding()
{
}

void DialogAxisBinding::AxisMoved(const JoyAxisTransporter& joyAxisTransporter)
{
	if (selectedJoyAction.joyDescriptor != nullptr && !selectedJoyAction.joyDescriptor->isEmpty())
	{
		if (joyAxisTransporter.descriptor == selectedJoyAction.joyDescriptor)
		{
			if (joyAxisTransporter.id >= 0)
			{
				listJoyAxis[joyAxisTransporter.id]->SetValue(joyAxisTransporter.value);
			}
		}

	}
}

void DialogAxisBinding::JoyDeviceRemoved()
{
	ui.SelectedDevice_comboBox->update();
	UpdateDevices();
	SelectJoyDescriptor(devicesModel->GetDescriptorById(ui.SelectedDevice_comboBox->currentIndex()));
}

void DialogAxisBinding::JoyDeviceAdded()
{
	UpdateDevices();
}

void DialogAxisBinding::reject()
{
	QDialog::reject();
}

void DialogAxisBinding::accept()
{
	if (selectedJoyAction.joyDescriptor == nullptr || selectedJoyAction.joyDescriptor->isEmpty()|| selectedJoyAction.axisId<0)
	{
		selectedJoyAction.joyDescriptor = nullptr;
		selectedJoyAction.axisId = -1;
	}
	emit UpdateAxisActionBinding(selectedJoyAction);
	QDialog::accept();
}

void DialogAxisBinding::BindAxis(int joyAxisId)
{
	selectedJoyAction.axisId = joyAxisId;
	UpdateAxisBindings();
}

void DialogAxisBinding::SelectedDeviceChanged(int index)
{
	auto selectedDevice = devicesModel->GetDescriptorById(index);
	SelectJoyDescriptor(devicesModel->GetDescriptorById(index));

}

void DialogAxisBinding::ClearListAxis()
{
	for (auto item : listJoyAxis)
	{
		item->disconnect();
	}
	listJoyAxis.clear();
}

void DialogAxisBinding::UpdateListAxis()
{
	ClearListAxis();
	if (selectedJoyAction.joyDescriptor != nullptr)
	{
		int count = deviceHandler->GetAxesCount(selectedJoyAction.joyDescriptor);
		for (int i = 0; i < count; i++)
		{
			auto joyWidg = std::make_shared< JoyWidget>(i, this);
			listJoyAxis.push_back(joyWidg);

			QObject::connect(joyWidg.get(), SIGNAL(Bind(int)), this, SLOT(BindAxis(int)));
		}
		UpdateAxisBindings();
	}
}

void DialogAxisBinding::UpdateAxisBindings()
{
	int count = deviceHandler->GetAxesCount(selectedJoyAction.joyDescriptor);
	for (int i = 0; i < count; i++)
	{
		if (selectedJoyAction.joyDescriptor != nullptr && !selectedJoyAction.joyDescriptor->isEmpty())
		{
			if (selectedJoyAction.axisId == i)
			{
				listJoyAxis[i]->Select();

			}
			else
			{
				listJoyAxis[i]->Deselect();
			}
		}
	}
}

void DialogAxisBinding::UpdateAxisWidgetArea()
{
//	ui.AxesScrollAreaWidgetContents->layout()->
	for (auto item : listJoyAxis)
	{
		ui.AxesScrollAreaWidgetContents->layout()->addWidget(item.get());
	}
}

void DialogAxisBinding::SelectJoyDescriptor(std::shared_ptr < IJoyDescriptor> joyDescriptor)
{
	selectedJoyAction.joyDescriptor = joyDescriptor;
	if (selectedJoyAction.joyDescriptor!=nullptr)
	{
		ui.DeviceName_label->setText(QString::fromStdString(selectedJoyAction.joyDescriptor->GetName()));
	}
	else
	{
		ui.DeviceName_label->setText("");
	}
	UpdateListAxis();
	UpdateAxisWidgetArea();
}

void DialogAxisBinding::UpdateDevices()
{
	devicesModel->UpdateDevices(deviceHandler->GetJoysDescriptors());
} 
