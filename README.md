# GradientController

Authors:

Peter Polidoro <polidorop@janelia.hhmi.org>

License:

BSD

## Ancestors

[ModularServer](https://github.com/janelia-arduino/ModularServer)

[ModularDeviceBase](https://github.com/janelia-arduino/ModularDeviceBase)

[MixingValveController](https://github.com/janelia-arduino/MixingValveController)

## Clients

## Devices

[modular_device_base](https://github.com/janelia-modular-devices/modular_device_base.git)

[power_switch_controller](https://github.com/janelia-modular-devices/power_switch_controller.git)

## API NAMES

```json
{
  "id": "getApi",
  "result": {
    "firmware": [
      "GradientController"
    ],
    "verbosity": "NAMES",
    "functions": [
      "getRampInfo"
    ],
    "properties": [
      "preRampConcentration",
      "preRampDuration",
      "rampDuration",
      "rampConcentrationIncrement",
      "postRampConcentration",
      "postRampDuration",
      "finalConcentration",
      "finalDuration",
      "testGradient"
    ],
    "callbacks": [
      "startGradient",
      "stopGradient"
    ]
  }
}
```

## API GENERAL

[API GENERAL](./api/)

## More Detailed Modular Device Information

[modular-devices](https://github.com/janelia-modular-devices/modular-devices)

## Install Library Dependencies

[arduino-libraries](https://github.com/janelia-arduino/arduino-libraries)
