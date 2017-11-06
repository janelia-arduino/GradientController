// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Constants.h"


namespace gradient_controller
{
namespace constants
{
CONSTANT_STRING(device_name,"gradient_controller");

CONSTANT_STRING(firmware_name,"GradientController");
// Use semantic versioning http://semver.org/
const modular_server::FirmwareInfo firmware_info =
  {
    .name_ptr=&firmware_name,
    .version_major=1,
    .version_minor=0,
    .version_patch=0,
  };

const double concentration_min = 0.0;
const double concentration_max = 100.0;

const double duration_min = 0.0;
const double duration_max = 100.0;

CONSTANT_STRING(state_string,"state");
CONSTANT_STRING(state_gradient_not_started_string,"GRADIENT_NOT_STARTED");
CONSTANT_STRING(state_pre_ramp_string,"PRE_RAMP");
CONSTANT_STRING(state_ramp_string,"RAMP");
CONSTANT_STRING(state_post_ramp_string,"POST_RAMP");
CONSTANT_STRING(state_final_string,"FINAL");
CONSTANT_STRING(state_finished_string,"FINISHED");

// Interrupts

// Units
CONSTANT_STRING(percent_units,"%");
CONSTANT_STRING(minute_units,"min");

// Properties
const long valve_count = 2;

const long mix_resolution_min = 100;
const long mix_resolution_max = 1000;
const long mix_resolution_default = 200;

CONSTANT_STRING(ramp_resolution_property_name,"rampResolution");
const long ramp_resolution_min = 10;
const long ramp_resolution_max = 1000;
const long ramp_resolution_default = 100;

CONSTANT_STRING(pre_ramp_concentration_property_name,"preRampConcentration");
const double pre_ramp_concentration_default = 10;

CONSTANT_STRING(pre_ramp_duration_property_name,"preRampDuration");
const double pre_ramp_duration_default = 1.5;

CONSTANT_STRING(ramp_duration_property_name,"rampDuration");
const double ramp_duration_default = 18.0;

CONSTANT_STRING(post_ramp_concentration_property_name,"postRampConcentration");
const double post_ramp_concentration_default = 95;

CONSTANT_STRING(post_ramp_duration_property_name,"postRampDuration");
const double post_ramp_duration_default = 1.0;

CONSTANT_STRING(final_concentration_property_name,"finalConcentration");
const double final_concentration_default = 10;

CONSTANT_STRING(final_duration_property_name,"finalDuration");
const double final_duration_default = 2.5;

CONSTANT_STRING(test_gradient_property_name,"testGradient");
const bool test_gradient_default = false;

// Parameters

// Functions
CONSTANT_STRING(get_ramp_timing_function_name,"getRampTiming");

// Callbacks
CONSTANT_STRING(start_gradient_callback_name,"startGradient");
CONSTANT_STRING(stop_gradient_callback_name,"stopGradient");

// Errors
}
}
