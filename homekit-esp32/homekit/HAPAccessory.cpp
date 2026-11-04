#include "HAPAccessory.h"
namespace esphome
{
  namespace homekit
  {
    HAPAccessory::HAPAccessory() {
    }
    void HAPAccessory::setup() {
      // hap_set_setup_code() 必须在 hap_init() 之前；hap_start() 必须在所有配件都加进数据库之后
      if (!this->setup_code_.empty()) {
        hap_set_setup_code(this->setup_code_.c_str());
      }
      int ret = hap_init(HAP_TRANSPORT_WIFI);
      if (ret != HAP_SUCCESS) {
        ESP_LOGE(TAG, "hap_init failed: %d", ret);
      }
      #ifdef USE_LIGHT
      for (const auto v : lights) {
        v->setup();
      }
      #endif
      #ifdef USE_LOCK
      for (const auto &v : locks) {
        #ifdef USE_HOMEKEY
        if (nfc) {
          v->set_hk_hw_finish(hkHwFinish);
          v->set_nfc_ctx(nfc);
        }
        #endif
        v->setup();
      }
      #endif
      #ifdef USE_FAN
      for (const auto v : fans) {
        v->setup();
      }
      #endif
      #ifdef USE_SWITCH
      for (const auto v : switches) {
        v->setup();
      }
      #endif
      #ifdef USE_SENSOR
      for (const auto v : sensors) {
        v->setup();
      }
      #endif
      #ifdef USE_BINARY_SENSOR
      for (const auto v : binary_sensors) {
        v->setup();
      }
      #endif
      #ifdef USE_CLIMATE
      for (const auto v : climates) {
        v->setup();
      }
      #endif
      // 配件都进数据库之后再启动 HAP
      ret = hap_start();
      if (ret != HAP_SUCCESS) {
        ESP_LOGE(TAG, "hap_start failed: %d", ret);
      } else {
        ESP_LOGI(TAG, "HomeKit started (setup code %s)", this->setup_code_.empty() ? "default" : this->setup_code_.c_str());
      }
    }
    void HAPAccessory::dump_config() {
      #ifdef USE_LOCK
      ESP_LOGCONFIG(TAG, "Lock HK Entities: %d", locks.size());
      #endif
      #ifdef USE_LIGHT
      ESP_LOGCONFIG(TAG, "Light HK Entities: %d", lights.size());
      #endif
      #ifdef USE_SENSOR
      ESP_LOGCONFIG(TAG, "Sensor HK Entities: %d", sensors.size());
      #endif
      #ifdef USE_BINARY_SENSOR
      ESP_LOGCONFIG(TAG, "BinarySensor HK Entities: %d", binary_sensors.size());
      #endif
      #ifdef USE_FAN
      ESP_LOGCONFIG(TAG, "Fan HK Entities: %d", fans.size());
      #endif
      #ifdef USE_SWITCH
      ESP_LOGCONFIG(TAG, "Switch HK Entities: %d", switches.size());
      #endif
    }
    #ifdef USE_LIGHT
    LightEntity* HAPAccessory::add_light(light::LightState* lightPtr) {
      lights.push_back(new LightEntity(lightPtr));
      return lights.back();
    }
    #endif
    #ifdef USE_LOCK
    LockEntity* HAPAccessory::add_lock(lock::Lock* lockPtr) {
      locks.push_back(new LockEntity(lockPtr));
      return locks.back();
    }
    #endif
    #ifdef USE_HOMEKEY
    void HAPAccessory::set_nfc_ctx(pn532::PN532* nfcCtx) {
      nfc = nfcCtx;
    }
    void HAPAccessory::set_hk_hw_finish(HKFinish color) {
      hkHwFinish = color;
    }
    #endif
    #ifdef USE_FAN
    FanEntity* HAPAccessory::add_fan(fan::Fan* fanPtr) {
      fans.push_back(new FanEntity(fanPtr));
      return fans.back();
    }
    #endif
    #ifdef USE_SWITCH
    SwitchEntity* HAPAccessory::add_switch(switch_::Switch* switchPtr) {
      switches.push_back(new SwitchEntity(switchPtr));
      return switches.back();
    }
    #endif
    #ifdef USE_SENSOR
    SensorEntity* HAPAccessory::add_sensor(sensor::Sensor* sensorPtr, TemperatureUnits units) {
      sensors.push_back(new SensorEntity(sensorPtr));
      return sensors.back();
    }
    #endif
    #ifdef USE_BINARY_SENSOR
    BinarySensorEntity* HAPAccessory::add_binary_sensor(binary_sensor::BinarySensor* sensorPtr, const char* type) {
      binary_sensors.push_back(new BinarySensorEntity(sensorPtr, type));
      return binary_sensors.back();
    }
    #endif
    #ifdef USE_CLIMATE
    ClimateEntity* HAPAccessory::add_climate(climate::Climate* climatePtr) {
      climates.push_back(new ClimateEntity(climatePtr));
      return climates.back();
  }
    #endif
  }
}