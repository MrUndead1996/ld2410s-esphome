#include "LD2410SButtons.h"

namespace esphome
{
    namespace ld2410s
    {
        void LD2410SEnableConfigButton::press_action()
        {
            this->parent_->set_config_mode(true);
        }

        void LD2410SDisableConfigButton::press_action()
        {
            this->parent_->set_config_mode(false);
        }

        void LD2410SApplyConfigButton::press_action()
        {
            this->parent_->apply_config();
        }

        void LD2410SAutoConfigThreshold::press_action()
        {
            this->parent_->start_auto_threshold_update();
        }
    }
}