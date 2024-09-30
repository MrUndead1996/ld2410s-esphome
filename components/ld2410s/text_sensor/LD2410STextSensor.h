#pragma once

#include "../LD2410S.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
    namespace ld2410s {
        class LD2410STextSensor : public LD2410SListener, public Component, text_sensor::TextSensor {
        public:
            void set_fw_version_text_sensor(text_sensor::TextSensor* tsensor) { this->fw_version_text_sensor = tsensor; };
            void on_fw_version(std::string& fw) override {
                if (this->fw_version_text_sensor != nullptr) {
                    this->fw_version_text_sensor->publish_state(fw);
                }
            }
        private:
            text_sensor::TextSensor* fw_version_text_sensor{ nullptr };
        };
    }
}