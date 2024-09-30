#include "LD2420ResponseSpeedSelect.h"

void esphome::ld2410s::LD2420ResponseSpeedSelect::control(const std::string &value)
{
    this->publish_state(value);
    this->parent_->new_config.resp_speed = value == RESPONSE_SPEED_NORMAL ? 5 : 10;
}