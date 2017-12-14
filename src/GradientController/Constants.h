// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef GRADIENT_CONTROLLER_CONSTANTS_H
#define GRADIENT_CONTROLLER_CONSTANTS_H
#include <ConstantVariable.h>
#include <MixingValveController.h>


namespace gradient_controller
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{PROPERTY_COUNT_MAX=9};
enum{PARAMETER_COUNT_MAX=1};
enum{FUNCTION_COUNT_MAX=1};
enum{CALLBACK_COUNT_MAX=2};

enum{GRADIENT_ARG_STARTED=-1};
enum{GRADIENT_ARG_NORMAL=-1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

extern const long concentration_min;
extern const long concentration_max;

extern const double duration_min;
extern const double duration_max;

struct GradientInfo
{
  const ConstantString * state_ptr;
  double concentration;
  size_t count;
  size_t inc;
};

extern ConstantString state_string;
extern ConstantString state_gradient_not_started_string;
extern ConstantString state_pre_ramp_string;
extern ConstantString state_ramp_string;
extern ConstantString state_post_ramp_string;
extern ConstantString state_final_string;
extern ConstantString state_finished_string;

// Interrupts

// Units
extern ConstantString minute_units;

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern const long valve_count;

extern const long mix_resolution;

extern const long mix_duration;

extern ConstantString pre_ramp_concentration_property_name;
extern const long pre_ramp_concentration_default;

extern ConstantString pre_ramp_duration_property_name;
extern const double pre_ramp_duration_default;

extern ConstantString ramp_duration_property_name;
extern const double ramp_duration_default;

extern ConstantString ramp_concentration_increment_property_name;
extern const long ramp_concentration_increment_min;
extern const long ramp_concentration_increment_max;
extern const long ramp_concentration_increment_default;

extern ConstantString post_ramp_concentration_property_name;
extern const long post_ramp_concentration_default;

extern ConstantString post_ramp_duration_property_name;
extern const double post_ramp_duration_default;

extern ConstantString final_concentration_property_name;
extern const long final_concentration_default;

extern ConstantString final_duration_property_name;
extern const double final_duration_default;

extern ConstantString test_gradient_property_name;
extern const bool test_gradient_default;

// Parameters

// Functions
extern ConstantString get_ramp_timing_function_name;

// Callbacks
extern ConstantString start_gradient_callback_name;
extern ConstantString stop_gradient_callback_name;

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
