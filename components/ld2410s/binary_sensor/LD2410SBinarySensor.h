#pragma once

#include "../LD2410S.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
    namespace ld2410s {
        class LD2410SBinarySensor : public LD2410SListener, public Component, binary_sensor::BinarySensor
        {
        public:
            void set_presence_sensor(binary_sensor::BinarySensor* bsensor) { this->presence_bsensor = bsensor; };
            void set_threshold_update_sensor(binary_sensor::BinarySensor* bsensor) { this->threshold_update_bsensor = bsensor; };
            void on_presence(bool presence) override {
                if (this->presence_bsensor != nullptr) {
                    if (this->presence_bsensor->state != presence) {
                        this->presence_bsensor->publish_state(presence);
                    }
                }
            };
            void on_threshold_update(bool running) override {
                if (this->threshold_update_bsensor != nullptr) {
                    if (this->threshold_update_bsensor->state != running)
                    {
                        this->threshold_update_bsensor->publish_state(running);
                    }
                }
            };

        private:
            binary_sensor::BinarySensor* presence_bsensor{ nullptr };
            binary_sensor::BinarySensor* threshold_update_bsensor{ nullptr };
        };
    }
}