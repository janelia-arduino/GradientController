// ----------------------------------------------------------------------------
// GradientController.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "GradientController.h"


using namespace gradient_controller;

GradientController::GradientController()
{
}

void GradientController::setup()
{
  // Parent Setup
  MixingValveController::setup();

  // Reset Watchdog
  resetWatchdog();

  // Variable Setup
  gradient_info_.state_ptr = &constants::state_gradient_not_started_constant_string;
  gradient_info_.state_duration = 0.0;

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware

  // Pins

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
    properties_,
    parameters_,
    functions_,
    callbacks_);

  // Properties
  modular_server::Property & valve_count_property = modular_server_.property(mixing_valve_controller::constants::valve_count_property_name);
  valve_count_property.setDefaultValue(constants::valve_count);
  valve_count_property.setRange(constants::valve_count,constants::valve_count);

  modular_server::Property & mix_resolution_property = modular_server_.property(mixing_valve_controller::constants::mix_resolution_property_name);
  mix_resolution_property.setDefaultValue(constants::mix_resolution);
  mix_resolution_property.setRange(constants::mix_resolution,constants::mix_resolution);

  modular_server::Property & mix_duration_property = modular_server_.property(mixing_valve_controller::constants::mix_duration_property_name);
  mix_duration_property.setDefaultValue(constants::mix_duration);

  modular_server::Property & pre_ramp_concentration_property = modular_server_.createProperty(constants::pre_ramp_concentration_property_name,constants::pre_ramp_concentration_default);
  pre_ramp_concentration_property.setUnits(mixing_valve_controller::constants::percent_units);
  pre_ramp_concentration_property.setRange(constants::concentration_min,constants::concentration_max);

  modular_server::Property & pre_ramp_duration_property = modular_server_.createProperty(constants::pre_ramp_duration_property_name,constants::pre_ramp_duration_default);
  pre_ramp_duration_property.setUnits(constants::minute_units);
  pre_ramp_duration_property.setRange(constants::duration_min,constants::duration_max);

  modular_server::Property & ramp_duration_property = modular_server_.createProperty(constants::ramp_duration_property_name,constants::ramp_duration_default);
  ramp_duration_property.setUnits(constants::minute_units);
  ramp_duration_property.setRange(constants::duration_min,constants::duration_max);
  ramp_duration_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&GradientController::updateRampPropertiesHandler));

  modular_server::Property & ramp_concentration_increment_property = modular_server_.createProperty(constants::ramp_concentration_increment_property_name,constants::ramp_concentration_increment_default);
  ramp_concentration_increment_property.setUnits(mixing_valve_controller::constants::percent_units);
  ramp_concentration_increment_property.setRange(constants::ramp_concentration_increment_min,constants::ramp_concentration_increment_max);

  modular_server::Property & post_ramp_concentration_property = modular_server_.createProperty(constants::post_ramp_concentration_property_name,constants::post_ramp_concentration_default);
  post_ramp_concentration_property.setUnits(mixing_valve_controller::constants::percent_units);
  post_ramp_concentration_property.setRange(constants::concentration_min,constants::concentration_max);

  modular_server::Property & post_ramp_duration_property = modular_server_.createProperty(constants::post_ramp_duration_property_name,constants::post_ramp_duration_default);
  post_ramp_duration_property.setUnits(constants::minute_units);
  post_ramp_duration_property.setRange(constants::duration_min,constants::duration_max);

  modular_server::Property & final_concentration_property = modular_server_.createProperty(constants::final_concentration_property_name,constants::final_concentration_default);
  final_concentration_property.setUnits(mixing_valve_controller::constants::percent_units);
  final_concentration_property.setRange(constants::concentration_min,constants::concentration_max);

  modular_server::Property & final_duration_property = modular_server_.createProperty(constants::final_duration_property_name,constants::final_duration_default);
  final_duration_property.setUnits(constants::minute_units);
  final_duration_property.setRange(constants::duration_min,constants::duration_max);
  final_duration_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&GradientController::updateRampPropertiesHandler));

  modular_server_.createProperty(constants::test_gradient_property_name,constants::test_gradient_default);

  updateRampPropertiesHandler();

  // Parameters

  // Functions
  modular_server::Function & get_gradient_info_function = modular_server_.createFunction(constants::get_gradient_info_function_name);
  get_gradient_info_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&GradientController::getGradientInfoHandler));
  get_gradient_info_function.setResultTypeObject();

  // Callbacks
  modular_server::Callback & start_gradient_callback = modular_server_.createCallback(constants::start_gradient_callback_name);
  start_gradient_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&GradientController::startGradientHandler));
  start_gradient_callback.attachTo(modular_device_base::constants::btn_a_pin_name,modular_server::constants::pin_mode_interrupt_falling);

  modular_server::Callback & stop_gradient_callback = modular_server_.createCallback(constants::stop_gradient_callback_name);
  stop_gradient_callback.attachFunctor(makeFunctor((Functor1<modular_server::Pin *> *)0,*this,&GradientController::stopGradientHandler));

}

void GradientController::startGradient()
{
  stopGradient();
  gradient_info_.state_duration = 0.0;
  setConcentrationHandler(constants::GRADIENT_ARG_STARTED);
}

void GradientController::stopGradient()
{
  gradient_info_.state_ptr = &constants::state_gradient_not_started_constant_string;
  gradient_info_.concentration = 0.0;
  gradient_info_.state_duration = 0.0;
  stopMixing();
}

MixingValveController::ValveValues GradientController::concentrationToRatio(double concentration)
{
  long mix_resolution;
  modular_server_.property(mixing_valve_controller::constants::mix_resolution_property_name).getValue(mix_resolution);

  ValveValues ratio;
  ratio.push_back(concentration*(mix_resolution/constants::concentration_max));
  ratio.push_back((constants::concentration_max - concentration)*(mix_resolution/constants::concentration_max));
  return ratio;
}

bool GradientController::finishMix()
{
  bool continue_mixing = true;
  if (gradient_info_.inc < gradient_info_.count)
  {
    ++gradient_info_.inc;
  }
  else
  {
    continue_mixing = false;
    setConcentrationHandler(constants::GRADIENT_ARG_NORMAL);
  }
  return continue_mixing;
}

void GradientController::writeGradientInfoToResponse()
{
  modular_server_.response().beginObject();

  modular_server_.response().write(constants::state_constant_string,gradient_info_.state_ptr);
  modular_server_.response().write(constants::state_duration_constant_string,gradient_info_.state_duration);
  modular_server_.response().write(constants::concentration_constant_string,gradient_info_.concentration);

  modular_server_.response().endObject();
}

void GradientController::updateRampPropertiesHandler()
{
}

void GradientController::getGradientInfoHandler()
{
  modular_server_.response().writeResultKey();
  writeGradientInfoToResponse();
}

void GradientController::startGradientHandler(modular_server::Pin * pin_ptr)
{
  startGradient();
}

void GradientController::stopGradientHandler(modular_server::Pin * pin_ptr)
{
  stopGradient();
}

void GradientController::setConcentrationHandler(int index)
{
  bool test_gradient;
  modular_server_.property(constants::test_gradient_property_name).getValue(test_gradient);

  const ConstantString * & state_ptr = gradient_info_.state_ptr;
  if (state_ptr == &constants::state_gradient_not_started_constant_string)
  {
    gradient_info_.state_ptr = &constants::state_pre_ramp_constant_string;

    long concentration;
    modular_server_.property(constants::pre_ramp_concentration_property_name).getValue(concentration);
    gradient_info_.concentration = concentration;

    double state_duration;
    modular_server_.property(constants::pre_ramp_duration_property_name).getValue(state_duration);
    gradient_info_.state_duration = state_duration;
    long state_duration_ms = state_duration*mixing_valve_controller::constants::seconds_per_minute*mixing_valve_controller::constants::milliseconds_per_second;

    modular_server_.property(mixing_valve_controller::constants::mix_duration_property_name).setValue(constants::mix_duration);
    long mix_duration;
    modular_server_.property(mixing_valve_controller::constants::mix_duration_property_name).getValue(mix_duration);

    gradient_info_.count = state_duration_ms/mix_duration;
    gradient_info_.inc = 1;
  }
  else if (state_ptr == &constants::state_pre_ramp_constant_string)
  {
    gradient_info_.state_ptr = &constants::state_ramp_constant_string;

    long concentration_min;
    modular_server_.property(constants::pre_ramp_concentration_property_name).getValue(concentration_min);

    long concentration_max;
    modular_server_.property(constants::post_ramp_concentration_property_name).getValue(concentration_max);

    long concentration_delta = concentration_max - concentration_min;

    double state_duration;
    modular_server_.property(constants::ramp_duration_property_name).getValue(state_duration);
    gradient_info_.state_duration = state_duration;

    long concentration_increment;
    modular_server_.property(constants::ramp_concentration_increment_property_name).getValue(concentration_increment);

    double state_duration_increment = (state_duration/concentration_delta)*concentration_increment;
    long state_duration_increment_ms = state_duration_increment*mixing_valve_controller::constants::seconds_per_minute*mixing_valve_controller::constants::milliseconds_per_second;

    long mix_duration = setMixDurationFactored(state_duration_increment_ms);

    // Add half of the increment to use segment midpoint instead of segment left most point to determine segment value
    gradient_info_.concentration = concentration_min + (double)concentration_increment/2;
    gradient_info_.count = state_duration_increment_ms/mix_duration;
    gradient_info_.inc = 1;
  }
  else if (state_ptr == &constants::state_ramp_constant_string)
  {
    long concentration_increment;
    modular_server_.property(constants::ramp_concentration_increment_property_name).getValue(concentration_increment);

    gradient_info_.concentration += concentration_increment;

    long post_ramp_concentration;
    modular_server_.property(constants::post_ramp_concentration_property_name).getValue(post_ramp_concentration);

    if (gradient_info_.concentration > post_ramp_concentration)
    {
      gradient_info_.state_ptr = &constants::state_post_ramp_constant_string;

      gradient_info_.concentration = post_ramp_concentration;

      double state_duration;
      modular_server_.property(constants::post_ramp_duration_property_name).getValue(state_duration);
      gradient_info_.state_duration = state_duration;
      long state_duration_ms = state_duration*mixing_valve_controller::constants::seconds_per_minute*mixing_valve_controller::constants::milliseconds_per_second;

      modular_server_.property(mixing_valve_controller::constants::mix_duration_property_name).setValue(constants::mix_duration);
      long mix_duration;
      modular_server_.property(mixing_valve_controller::constants::mix_duration_property_name).getValue(mix_duration);

      gradient_info_.count = state_duration_ms/mix_duration;
    }
    gradient_info_.inc = 1;
  }
  else if (state_ptr == &constants::state_post_ramp_constant_string)
  {
    gradient_info_.state_ptr = &constants::state_final_constant_string;

    long concentration;
    modular_server_.property(constants::final_concentration_property_name).getValue(concentration);
    gradient_info_.concentration = concentration;

    double state_duration;
    modular_server_.property(constants::final_duration_property_name).getValue(state_duration);
    gradient_info_.state_duration = state_duration;
    long state_duration_ms = state_duration*mixing_valve_controller::constants::seconds_per_minute*mixing_valve_controller::constants::milliseconds_per_second;

    long mix_duration;
    modular_server_.property(mixing_valve_controller::constants::mix_duration_property_name).getValue(mix_duration);

    gradient_info_.count = state_duration_ms/mix_duration;
    gradient_info_.inc = 1;
  }
  else
  {
    gradient_info_.state_ptr = &constants::state_finished_constant_string;
    stopMixing();
    gradient_info_.concentration = 0.0;
    gradient_info_.state_duration = 0.0;
    return;
  }

  ValveValues mix_ratio = concentrationToRatio(gradient_info_.concentration);

  startMixing(mix_ratio);
}
