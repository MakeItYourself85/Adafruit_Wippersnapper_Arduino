/*!
 * @file WipperSnapper_I2C_Driver_scd40.h
 *
 * Device driver for the SCD40 CO2, Temperature, and Humidity sensor.
 * TEMPORARY HACK
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Copyright (c) Marni Brewster 2022 for Adafruit Industries.
 *
 * MIT license, all text here must be included in any redistribution.
 *
 */

#ifndef WipperSnapper_I2C_Driver_scd40_H
#define WipperSnapper_I2C_Driver_scd40_H

#include "WipperSnapper_I2C_Driver.h"
#include <SensirionI2CScd4x.h>
#include <Wire.h>

// TODO: Change title to SCD4X.h

/**************************************************************************/
/*!
    @brief  Class that provides a driver interface for the SCD40 sensor.
*/
/**************************************************************************/
class WipperSnapper_I2C_Driver_SCD40 : public WipperSnapper_I2C_Driver {

public:
  /*******************************************************************************/
  /*!
      @brief    Constructor for a SCD40 sensor.
      @param    i2c
                The I2C interface.
      @param    sensorAddress
                7-bit device address.
  */
  /*******************************************************************************/
  WipperSnapper_I2C_Driver_SCD40(TwoWire *i2c, uint16_t sensorAddress)
      : WipperSnapper_I2C_Driver(i2c, sensorAddress) {
    _i2c = i2c;
    _sensorAddress = sensorAddress;
  }

  /*******************************************************************************/
  /*!
      @brief    Initializes the SCD40 sensor and begins I2C.
      @returns  True if initialized successfully, False otherwise.
  */
  /*******************************************************************************/
  bool begin() {
    _scd = new SensirionI2CScd4x();
    _scd->begin(*_i2c);

    // stop previously started measurement
    if (!_scd->stopPeriodicMeasurement())
      return false;

    // attempt to grab SCD4x's serial number
    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    if (!_scd->getSerialNumber(serial0, serial1, serial2))
      return false;

    // start measurements
    if (!_scd->startPeriodicMeasurement())
      return false;

    return true;
  }

  bool readSensorMeasurements() {
    // TODO: We should also have a func. to check if data is ready
    // prior to calling this func but sensiron does not yet
    // have this released:
    // https://github.com/Sensirion/arduino-i2c-scd4x/commit/4ec07965f30f32f4320960a04aadb7a19e6499c7

    int16_t error;
    error = _scd->readMeasurement(_co2, _temperature, _humidity);
    if (error || _co2 == 0)
      return false;
    return true;
  }

  /*******************************************************************************/
  /*!
      @brief    Gets the SCD40's current temperature.
      @param    tempEvent
                Pointer to an Adafruit_Sensor event.
      @returns  True if the temperature was obtained successfully, False
                otherwise.
  */
  /*******************************************************************************/
  bool getEventAmbientTemp(sensors_event_t *tempEvent) {
    // read all sensor measurements
    if (!readSensorMeasurements())
      return false;

    tempEvent->temperature = _temperature;
    return true;
  }

  /*******************************************************************************/
  /*!
      @brief    Gets the SCD40's current relative humidity reading.
      @param    humidEvent
                Pointer to an Adafruit_Sensor event.
      @returns  True if the humidity was obtained successfully, False
                otherwise.
  */
  /*******************************************************************************/
  bool getEventRelativeHumidity(sensors_event_t *humidEvent) {
    // read all sensor measurements
    if (!readSensorMeasurements())
      return false;

    humidEvent->relative_humidity = _humidity;
    return true;
  }

  /*******************************************************************************/
  /*!
      @brief    Gets the SCD40's current CO2 reading.
      @param    co2Event
                  Adafruit Sensor event for CO2
      @returns  True if the sensor value was obtained successfully, False
                otherwise.
  */
  /*******************************************************************************/
  bool getEventCO2(sensors_event_t *co2Event) {
    // read all sensor measurements
    if (!readSensorMeasurements())
      return false;

    // TODO: This is a TEMPORARY HACK, we need to add CO2 type to
    // adafruit_sensor
    co2Event->data[0] = _co2;
    return true;
  }

protected:
  SensirionI2CScd4x *_scd; ///< SCD4x driver object
  uint16_t _co2;           ///< SCD4x co2 reading
  float _temperature;      ///< SCD4x temperature reading
  float _humidity;         ///< SCD4x humidity reading
};

#endif // WipperSnapper_I2C_Driver_SCD40