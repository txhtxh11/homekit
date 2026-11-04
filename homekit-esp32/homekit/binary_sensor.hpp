#pragma once
#include <esphome/core/defines.h>
#ifdef USE_BINARY_SENSOR
#include <esphome/core/application.h>
#include <esphome/components/binary_sensor/binary_sensor.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include "hap_entity.h"

namespace esphome
{
  namespace homekit
  {
    /**
     * 把 ESPHome 的 binary_sensor 映射成 HomeKit 的状态类传感器。
     * type 决定用哪种 HAP 服务：
     *   motion     → Motion Sensor（人体感应）
     *   contact    → Contact Sensor（门磁/窗磁，也用于按键）
     *   occupancy  → Occupancy Sensor
     *   smoke      → Smoke Sensor
     * 只读：家庭 App 不写值，也没必要注册 write 回调。
     */
    class BinarySensorEntity : public HAPEntity
    {
    private:
      static constexpr const char* TAG = "BinarySensorEntity";
      binary_sensor::BinarySensor* sensorPtr_{nullptr};
      std::string type_{"motion"};
      hap_serv_t* service_{nullptr};
      hap_char_t* state_char_{nullptr};

      bool is_contact_() const { return type_ == "contact" || type_ == "door" || type_ == "window"; }

      // HAP 的布尔型特征用 val.b；0/1 型的（contact / occupancy / smoke）用 val.u
      void push_state_(bool v)
      {
        if (this->state_char_ == nullptr)
          return;
        hap_val_t val;
        if (this->is_contact_())
        {
          // HAP: Contact Sensor State 0 = 接触（关），1 = 未接触（开）
          val.u = v ? 1 : 0;
        }
        else if (type_ == "occupancy" || type_ == "smoke")
        {
          val.u = v ? 1 : 0;
        }
        else
        {
          val.b = v;
        }
        hap_char_update_val(this->state_char_, &val);
      }

      static int acc_identify(hap_acc_t *ha)
      {
        ESP_LOGI(TAG, "Accessory identified");
        return HAP_SUCCESS;
      }

    public:
      BinarySensorEntity(binary_sensor::BinarySensor* sensorPtr, const char* type)
          : HAPEntity({{MODEL, "HAP-BINARY-SENSOR"}}), sensorPtr_(sensorPtr)
      {
        if (type != nullptr && type[0] != '\0')
          this->type_ = type;
      }

      void setup() override
      {
        hap_acc_cfg_t acc_cfg = {
            .model = strdup(accessory_info[MODEL]),
            .manufacturer = strdup(accessory_info[MANUFACTURER]),
            .fw_rev = strdup(accessory_info[FW_REV]),
            .hw_rev = NULL,
            .pv = strdup("1.1.0"),
            .cid = HAP_CID_BRIDGE,
            .identify_routine = acc_identify,
        };

        std::string accessory_name = sensorPtr_->get_name();
        acc_cfg.name = strdup(accessory_info[NAME] == NULL ? accessory_name.c_str() : accessory_info[NAME]);
        acc_cfg.serial_num = strdup(accessory_info[SN] == NULL
                                        ? std::to_string(sensorPtr_->get_object_id_hash()).c_str()
                                        : accessory_info[SN]);

        hap_acc_t* accessory = hap_acc_create(&acc_cfg);
        if (accessory == nullptr)
        {
          ESP_LOGE(TAG, "Failed to create accessory for '%s'", accessory_name.c_str());
          return;
        }

        const bool st = sensorPtr_->state;
        if (this->is_contact_())
        {
          this->service_ = hap_serv_contact_sensor_create(st ? 1 : 0);
          this->state_char_ = hap_serv_get_char_by_uuid(this->service_, HAP_CHAR_UUID_CONTACT_SENSOR_STATE);
        }
        else if (type_ == "occupancy")
        {
          this->service_ = hap_serv_occupancy_sensor_create(st ? 1 : 0);
          this->state_char_ = hap_serv_get_char_by_uuid(this->service_, HAP_CHAR_UUID_OCCUPANCY_DETECTED);
        }
        else if (type_ == "smoke")
        {
          this->service_ = hap_serv_smoke_sensor_create(st ? 1 : 0);
          this->state_char_ = hap_serv_get_char_by_uuid(this->service_, HAP_CHAR_UUID_SMOKE_DETECTED);
        }
        else
        {
          this->service_ = hap_serv_motion_sensor_create(st);
          this->state_char_ = hap_serv_get_char_by_uuid(this->service_, HAP_CHAR_UUID_MOTION_DETECTED);
        }

        if (this->service_ == nullptr)
        {
          ESP_LOGE(TAG, "Failed to create service for '%s'", accessory_name.c_str());
          return;
        }

        hap_acc_add_serv(accessory, this->service_);
        hap_add_bridged_accessory(accessory, hap_get_unique_aid(std::to_string(sensorPtr_->get_object_id_hash()).c_str()));

        if (!sensorPtr_->is_internal())
          sensorPtr_->add_on_state_callback([this](bool v) { this->push_state_(v); });

        ESP_LOGI(TAG, "Binary sensor '%s' linked to HomeKit (%s)", accessory_name.c_str(), this->type_.c_str());
      }
    };
  }
}
#endif
