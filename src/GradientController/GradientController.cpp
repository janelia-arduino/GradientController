// ----------------------------------------------------------------------------
// GradientController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "../GradientController.h"


using namespace gradient_controller;

GradientController::GradientController()
{
}

void GradientController::setup()
{
  // Parent Setup
  MixingValveController::setup();

  // Variable Setup
  gradient_info_.state_ptr = &constants::state_gradient_not_started_string;

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware

  // Interrupts

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
  mix_resolution_property.setDefaultValue(constants::mix_resolution_default);
  mix_resolution_property.setRange(constants::mix_resolution_min,constants::mix_resolution_max);

  modular_server::Property & ramp_resolution_property = modular_server_.createProperty(constants::ramp_resolution_property_name,constants::ramp_resolution_default);
  ramp_resolution_property.setRange(constants::ramp_resolution_min,constants::ramp_resolution_max);
  ramp_resolution_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&GradientController::updateRampTimingHandler));

  modular_server::Property & pre_ramp_concentration_property = modular_server_.createProperty(constants::pre_ramp_concentration_property_name,constants::pre_ramp_concentration_default);
  pre_ramp_concentration_property.setUnits(constants::percent_units);
  pre_ramp_concentration_property.setRange(constants::concentration_min,constants::concentration_max);

  modular_server::Property & pre_ramp_duration_property = modular_server_.createProperty(constants::pre_ramp_duration_property_name,constants::pre_ramp_duration_default);
  pre_ramp_duration_property.setUnits(constants::minute_units);
  pre_ramp_duration_property.setRange(constants::duration_min,constants::duration_max);

  modular_server::Property & ramp_duration_property = modular_server_.createProperty(constants::ramp_duration_property_name,constants::ramp_duration_default);
  ramp_duration_property.setUnits(constants::minute_units);
  ramp_duration_property.setRange(constants::duration_min,constants::duration_max);
  ramp_duration_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&GradientController::updateRampTimingHandler));

  modular_server::Property & post_ramp_concentration_property = modular_server_.createProperty(constants::post_ramp_concentration_property_name,constants::post_ramp_concentration_default);
  post_ramp_concentration_property.setUnits(constants::percent_units);
  post_ramp_concentration_property.setRange(constants::concentration_min,constants::concentration_max);

  modular_server::Property & post_ramp_duration_property = modular_server_.createProperty(constants::post_ramp_duration_property_name,constants::post_ramp_duration_default);
  post_ramp_duration_property.setUnits(constants::minute_units);
  post_ramp_duration_property.setRange(constants::duration_min,constants::duration_max);

  modular_server::Property & final_concentration_property = modular_server_.createProperty(constants::final_concentration_property_name,constants::final_concentration_default);
  final_concentration_property.setUnits(constants::percent_units);
  final_concentration_property.setRange(constants::concentration_min,constants::concentration_max);

  modular_server_.createProperty(constants::test_gradient_property_name,constants::test_gradient_default);

  updateRampTimingHandler();

  // Parameters

  // Functions

  // Callbacks
  modular_server::Callback & start_gradient_callback = modular_server_.createCallback(constants::start_gradient_callback_name);
  start_gradient_callback.attachFunctor(makeFunctor((Functor1<modular_server::Interrupt *> *)0,*this,&GradientController::startGradientHandler));
#if !defined(__AVR_ATmega2560__)
  start_gradient_callback.attachTo(modular_device_base::constants::btn_a_interrupt_name,modular_server::interrupt::mode_falling);
#endif

  modular_server::Callback & stop_gradient_callback = modular_server_.createCallback(constants::stop_gradient_callback_name);
  stop_gradient_callback.attachFunctor(makeFunctor((Functor1<modular_server::Interrupt *> *)0,*this,&GradientController::stopGradientHandler));

}

void GradientController::startGradient()
{
  stopGradient();
  setConcentrationHandler(-1);
}

void GradientController::stopGradient()
{
  gradient_info_.state_ptr = &constants::state_gradient_not_started_string;
  event_controller_.remove(gradient_info_.event_id);
  stopMixing();
}

MixingValveController::Ratio GradientController::concentrationToRatio(const double concentration)
{
  long ramp_resolution;
  modular_server_.property(constants::ramp_resolution_property_name).getValue(ramp_resolution);

  Ratio ratio;
  ratio.push_back(concentration*(ramp_resolution/constants::concentration_max));
  ratio.push_back((constants::concentration_max - concentration)*(ramp_resolution/constants::concentration_max));
  return ratio;
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray *
// ArduinoJson::JsonObject *
//
// For more info read about ArduinoJson parsing https://github.com/janelia-arduino/ArduinoJson
//
// modular_server_.property(property_name).getValue(value) value type must match the property default type
// modular_server_.property(property_name).setValue(value) value type must match the property default type
// modular_server_.property(property_name).getElementValue(element_index,value) value type must match the property array element default type
// modular_server_.property(property_name).setElementValue(element_index,value) value type must match the property array element default type

void GradientController::updateRampTimingHandler()
{
}

void GradientController::startGradientHandler(modular_server::Interrupt * interrupt_ptr)
{
  startGradient();
}

void GradientController::stopGradientHandler(modular_server::Interrupt * interrupt_ptr)
{
  stopGradient();
}

void GradientController::setConcentrationHandler(int index)
{
  if (event_controller_.eventsAvailable())
  {
    double concentration;
    double duration;

    const ConstantString * & state_ptr = gradient_info_.state_ptr;
    if (state_ptr == &constants::state_gradient_not_started_string)
    {
      gradient_info_.state_ptr = &constants::state_pre_ramp_string;
      modular_server_.property(constants::pre_ramp_concentration_property_name).getValue(concentration);
      modular_server_.property(constants::pre_ramp_duration_property_name).getValue(duration);
    }
    else if (state_ptr == &constants::state_pre_ramp_string)
    {
      // gradient_info_.state_ptr = &constants::state_ramp_string;
      modular_server_.property(constants::pre_ramp_concentration_property_name).getValue(concentration);
      duration = 10;
    }

    bool test_gradient;
    modular_server_.property(constants::test_gradient_property_name).getValue(test_gradient);

    Ratio mix_ratio = concentrationToRatio(concentration);
    long duration_ms;
    if (!test_gradient)
    {
      duration_ms = duration*mixing_valve_controller::constants::seconds_per_minute*mixing_valve_controller::constants::milliseconds_per_second;
    }
    else
    {
      duration_ms = duration*mixing_volume_fill_duration_;
    }

    Serial << "concentration = " << concentration << ", mix_ratio = " << mix_ratio << "\n";
    Serial << "duration = " << duration << ", duration_ms = " << duration_ms << "\n";

    startMixing(mix_ratio);

    gradient_info_.event_id = event_controller_.addEventUsingDelay(makeFunctor((Functor1<int> *)0,*this,&GradientController::setConcentrationHandler),
                                                                   duration_ms);

    event_controller_.enable(gradient_info_.event_id);
  }
}
