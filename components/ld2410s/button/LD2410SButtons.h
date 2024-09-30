#pragma once

#include "esphome/components/button/button.h"
#include "../LD2410S.h"

namespace esphome
{
    namespace ld2410s
    {
        class LD2410SEnableConfigButton : public button::Button, public Parented<LD2410S>
        {
        public:
            LD2410SEnableConfigButton() = default;

        protected:
            void press_action() override;
        };

        class LD2410SDisableConfigButton : public button::Button, public Parented<LD2410S>
        {
        public:
            LD2410SDisableConfigButton() = default;

        protected:
            void press_action() override;
        };

        class LD2410SApplyConfigButton : public button::Button, public Parented<LD2410S>
        {
        public:
            LD2410SApplyConfigButton() = default;

        protected:
            void press_action() override;
        };

        class LD2410SAutoConfigThreshold : public button::Button, public Parented<LD2410S>
        {
        public:
            LD2410SAutoConfigThreshold() = default;

        protected:
            void press_action() override;
        };
    }
}