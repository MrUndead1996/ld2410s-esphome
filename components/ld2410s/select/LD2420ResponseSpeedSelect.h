#pragma once

#include "../LD2410S.h"
#include "esphome/components/select/select.h"

namespace esphome
{
    namespace ld2410s
    {
        class LD2420ResponseSpeedSelect : public Component, public select::Select, public Parented<LD2410S>
        {
        public:
            LD2420ResponseSpeedSelect() = default;

        protected:
            void control(const std::string &value) override;
        };
    }
}