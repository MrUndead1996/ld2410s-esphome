#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

namespace esphome
{
    namespace ld2410s
    {
        // Short reporting format
        static const uint16_t DATA_FRAME_HEADER = 0x6E;
        static const uint16_t DATA_FRAME_FOOTER = 0x62;

        static const uint32_t CMD_FRAME_HEADER = 0xFAFBFCFD;
        static const uint32_t CMD_FRAME_FOOTER = 0x01020304;
        static const uint32_t THRESHOLD_HEADER = 0xF1F2F3F4;
        static const uint32_t THRESHOLD_FOOTER = 0xF5F6F7F8;

        static const uint16_t READ_FW_CMD = 0x0000;
        static const uint16_t START_CONFIG_MODE_CMD = 0x00FF;
        static const uint16_t END_CONFIG_MODE_CMD = 0x00FE;
        static const uint16_t WRITE_PARAMS_CMD = 0x0070;
        static const uint16_t READ_PARAMS_CMD = 0x0071;
        static const uint16_t AUTO_UPDATE_THRESHOLD_CMD = 0x0009;

        static const uint16_t START_CONFIG_MODE_VALUE = 0x0001;
        static const uint16_t CFG_MAX_DETECTION_VALUE = 0x0005;
        static const uint16_t CFG_MIN_DETECTION_VALUE = 0x000A;
        static const uint16_t CFG_NO_DELAY_VALUE = 0x0006;
        static const uint16_t CFG_STATUS_FREQ_VALUE = 0x0002;
        static const uint16_t CFG_DISTANCE_FREQ_VALUE = 0x000C;
        static const uint16_t CFG_RESPONSE_SPEED_VALUE = 0x000B;
        static const uint16_t THRESHOLD_TRIGGER_VALUE = 0x0002;
        static const uint16_t THRESHOLD_RETENTION_VALUE = 0x0001;
        static const uint16_t THRESHOLD_TIME_VALUE = 0x0078;

        static const uint16_t START_CONFIG_MODE_REPLAY = 0x01FF;
        static const uint16_t END_CONFIG_MODE_REPLAY = 0x01FE;
        static const uint16_t WRITE_PARAMS_REPLAY = 0x0170;
        static const uint16_t READ_PARAMS_REPLAY = 0x0171;
        static const uint16_t READ_FW_REPLAY = 0x0100;

        static const std::string RESPONSE_SPEED_NORMAL = "Normal";
        static const std::string RESPONSE_SPEED_FAST = "Fast";

        struct Config
        {
            uint32_t max_dist{ 0 };
            uint32_t min_dist{ 0 };
            uint32_t delay{ 0 };
            uint32_t status_freq{ 0 };
            uint32_t dist_freq{ 0 };
            uint32_t resp_speed{ 0 };
        };

        struct DataFrame
        {
            uint16_t header{ DATA_FRAME_HEADER };
            bool presence{ false };
            uint16_t distance{ 0 };
            uint16_t footer{ DATA_FRAME_FOOTER };
        };

        struct ThresholdFrame
        {
            uint32_t header{ THRESHOLD_HEADER };
            uint16_t number{ 0 };
            uint16_t type{ 0 };
            uint16_t progress{ 0 };
            uint32_t footer{ THRESHOLD_FOOTER };
        };

        struct CmdFrameT
        {
            uint32_t header{ 0 };
            uint16_t length{ 0 };
            uint16_t command{ 0 };
            uint8_t data[36];
            uint16_t data_length{ 0 };
            uint32_t footer{ 0 };
        };

        enum class PackageType
        {
            ACK,
            SHORT_DATA,
            TRESHOLD,
            UNKNOWN
        };

        class LD2410SListener
        {
        public:
            virtual void on_presence(bool presence) {};
            virtual void on_distance(int distance) {};
            virtual void on_threshold_update(bool running) {};
            virtual void on_threshold_progress(int progress) {};
            virtual void on_fw_version(std::string& fw) {};
        };

        class LD2410S : public uart::UARTDevice,
            public Component
        {
        public:
            Config new_config;

            void setup() override;
            void loop() override;
            float get_setup_priority() const override;

            void register_listener(LD2410SListener* listener) { this->listeners.push_back(listener); };
            void set_config_mode(bool enabled);
            void apply_config();
            void start_auto_threshold_update();
#ifdef USE_NUMBER
            void set_max_distance_number(number::Number* max_distance_number) { this->max_distance_number = max_distance_number; };
            void set_min_distance_number(number::Number* min_distance_number) { this->min_distance_number = min_distance_number; };
            void set_no_delay_number(number::Number* delay_number) { this->no_delay_number = delay_number; };
            void set_status_reporting_freq_number(number::Number* status_reporting_freq_number) { this->status_reporting_freq_number = status_reporting_freq_number; };
            void set_distance_reporting_freq_number(number::Number* distance_reporting_freq_number) { this->distance_reporting_freq_number = distance_reporting_freq_number; };
#endif
#ifdef USE_BUTTON
            void set_enable_config_button(button::Button* button) { this->enable_config_button = button; };
            void set_disable_config_button(button::Button* button) { this->disable_config_button = button; };
            void set_apply_config_button(button::Button* button) { this->apply_config_button = button; };
            void set_auto_threshold_button(button::Button* button) { this->auto_threshold_button = button; };
#endif
#ifdef USE_SELECT
            void set_response_speed_select(select::Select* selector) { this->response_speed_select = selector; };
#endif
        private:
            std::vector<uint8_t> buffer;
            std::vector<LD2410SListener*> listeners{};
            Config current_config;
            bool cmd_active{ false };
            bool config_mode_active{ false };
#ifdef USE_NUMBER
            number::Number* max_distance_number{ nullptr };
            number::Number* min_distance_number{ nullptr };
            number::Number* no_delay_number{ nullptr };
            number::Number* status_reporting_freq_number{ nullptr };
            number::Number* distance_reporting_freq_number{ nullptr };
#endif
#ifdef USE_BUTTON
            button::Button* enable_config_button{ nullptr };
            button::Button* disable_config_button{ nullptr };
            button::Button* apply_config_button{ nullptr };
            button::Button* auto_threshold_button{ nullptr };
#endif
#ifdef USE_SELECT
            select::Select* response_speed_select{ nullptr };
#endif
            void send_apply_config_cmd();
            void send_start_auto_threshold_update_cmd();
            void send_read_fw_cmd();
            void send_command(CmdFrameT cmd_frame);
            PackageType read_line(uint32_t data);
            bool process_cmd_ack_package(uint8_t* buffer, int len);
            void process_data_package(PackageType type);
            int bytes_to_int(unsigned char* buffer, size_t size)
            {
                unsigned int ret = 0;
                int shift = 0;
                for (int i = 0; i < size; i++)
                {
                    ret |= static_cast<unsigned int>(buffer[i]) << shift;
                    shift += 8;
                }
                return ret;
            }
            void read_current_config();
        };
    }
}