// ----------------------------------------------------------------------------
// GradientController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef GRADIENT_CONTROLLER_H
#define GRADIENT_CONTROLLER_H
#include <Flash.h>
#include <EEPROM.h>
#include <Streaming.h>
#include <ArduinoJson.h>
#include <JsonSanitizer.h>
#include <JsonStream.h>
#include <Array.h>
#include <Vector.h>
#include <MemoryFree.h>
#include <ConstantVariable.h>
#include <SavedVariable.h>
#include <Functor.h>
#include <IndexedContainer.h>
#include <FunctorCallbacks.h>
#include <Watchdog.h>

#include <SPI.h>
#include <TLE72X.h>
#include <EventController.h>

#include <ModularClient.h>

#include <ModularServer.h>
#include <ModularDeviceBase.h>
#include <PowerSwitchController.h>
#include <MixingValveController.h>

#include "GradientController/Constants.h"


class GradientController : public MixingValveController
{
public:
  GradientController();
  virtual void setup();

  void startGradient();
  void stopGradient();

  Ratio concentrationToRatio(const double concentration);

protected:
  virtual bool finishMix();

private:
  modular_server::Property properties_[gradient_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[gradient_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[gradient_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[gradient_controller::constants::CALLBACK_COUNT_MAX];

  gradient_controller::constants::GradientInfo gradient_info_;

  // Handlers
  void updateRampPropertiesHandler();
  void getRampInfoHandler();
  void startGradientHandler(modular_server::Interrupt * interrupt_ptr);
  void stopGradientHandler(modular_server::Interrupt * interrupt_ptr);
  void setConcentrationHandler(int index);

};

#endif
