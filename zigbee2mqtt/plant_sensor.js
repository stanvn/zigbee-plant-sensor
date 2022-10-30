const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const extend = require('zigbee-herdsman-converters/lib/extend');
const e = exposes.presets;
const ea = exposes.access;

const definition = {
  zigbeeModel: ['plant_sensor'],
  model: 'plant_sensor',
  vendor: 'Custom devices (DiY)',
  description: 'My Plant Sensor',
  fromZigbee: [fz.battery, fz.temperature,  fz.humidity, fz.soil_moisture, fz.illuminance],
  toZigbee: [],
  configure: async (device, coordinatorEndpoint, logger) => {
    const endpoint = device.getEndpoint(1);
    await reporting.bind(endpoint, coordinatorEndpoint, [
      'msTemperatureMeasurement', 'msRelativeHumidity', 'msSoilMoisture', 'msIlluminanceMeasurement', 'genPowerCfg']);
    const overides = {min: 10, max: 64800, change: 0};
    await reporting.batteryVoltage(endpoint, overides);
    // await reporting.batteryPercentageRemaining(endpoint, overides);
    await reporting.temperature(endpoint, overides);
    await reporting.humidity(endpoint, overides);
    await reporting.illuminance(firstEndpoint, overides);
    await reporting.soil_moisture(endpoint, overides);

  },
  exposes: [
    e.temperature(),
    e.humidity(),
    e.soil_moisture(),
    e.battery_voltage(),
    e.illuminance_lux(),
    e.illuminance(),
    e.battery_low()],
};

module.exports = definition;
