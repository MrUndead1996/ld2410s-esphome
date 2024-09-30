#include "LD2410SNumber.h"

namespace esphome
{
    namespace ld2410s
    {
        void LD2410SMaxDistanceNumber::control(float max_distance)
        {
            this->publish_state(max_distance);
            this->parent_->new_config.max_dist = max_distance;
        }

        void LD2410SMinDistanceNumber::control(float min_distance)
        {
            this->publish_state(min_distance);
            this->parent_->new_config.min_dist = min_distance;
        }

        void LD2410SDelayNumber::control(float no_delay)
        {
            this->publish_state(no_delay);
            this->parent_->new_config.delay = no_delay;
        }

        void LD2410SStatusReportingFreqNumber::control(float status_reporting_freq)
        {
            this->publish_state(status_reporting_freq);
            this->parent_->new_config.status_freq = status_reporting_freq * 10;
        }

        void LD2410SDistReportingFreqNumber::control(float distance_reporting_freq)
        {
            this->publish_state(distance_reporting_freq);
            this->parent_->new_config.dist_freq = distance_reporting_freq * 10;
        }
    }
}
