#pragma once

#include "../LD2410S.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace ld2410s
    {
        class LD2410SSensor : public LD2410SListener, public Component, sensor::Sensor
        {
        public:
            void set_distance_sensor(sensor::Sensor *sensor) { this->distance_sensor = sensor; }
            void set_threshold_update_sensor(sensor::Sensor *sensor) { this->threshold_update_sensor = sensor; }
            void on_distance(int distance) override
            {
                if (this->distance_sensor != nullptr)
                {
                    if (this->distance_sensor->get_state() != distance)
                    {
                        this->distance_sensor->publish_state(distance);
                    }
                }
            }
            void on_threshold_progress(int progress) override
            {
                if (this->threshold_update_sensor != nullptr)
                {
                    if (this->threshold_update_sensor->get_state() != progress)
                    {
                        this->threshold_update_sensor->publish_state(progress);
                    }
                }
            };

        private:
            sensor::Sensor *distance_sensor{nullptr};
            sensor::Sensor *threshold_update_sensor{nullptr};
        };
    }
}