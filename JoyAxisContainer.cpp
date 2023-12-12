#pragma once
#include <string>
#include "IJoyDescriptor.hpp"
#include <memory>

namespace UserInput
{
	class JoyAxisContainer
	{
	public:
		AxisAction action = AxisAction::AA_NONE;
		int axisId = -1;
		std::shared_ptr< IJoyDescriptor> joyDescriptor = nullptr;
		double value = 0;
		double minVal = -32768;
		double maxVal = 32767;
		double extMinVal = -32768;
		double extMaxVal = 32767;
		double deadZoneMinVal = 0;
		double deadZoneMaxVal = 0;

		JoyAxisContainer& operator =(const JoyAxisContainer& p)
		{
			action = p.action;
			axisId = p.axisId;
			joyDescriptor = p.joyDescriptor;
			value = p.value;
			minVal = p.minVal;
			maxVal = p.maxVal;
			extMinVal = p.extMinVal;
			extMaxVal = p.extMaxVal;
			deadZoneMinVal = p.deadZoneMinVal;
			deadZoneMaxVal = p.deadZoneMaxVal;
			return *this;
		}
		bool operator ==(const JoyAxisContainer& p) const
		{
			return action == p.action &&
				axisId == p.axisId &&
				joyDescriptor == p.joyDescriptor &&
				value == p.value &&
				minVal == p.minVal &&
				maxVal == p.maxVal &&
				extMinVal == p.extMinVal &&
				extMaxVal == p.extMaxVal &&
				deadZoneMinVal == p.deadZoneMinVal &&
				deadZoneMaxVal == p.deadZoneMaxVal;
		}
	};
}
