#include "esphome/core/log.h"
#include "LD2410S.h"

namespace esphome
{
    namespace ld2410s
    {

        static const char* TAG = "ld2410s";

        void LD2410S::setup() {
            this->set_config_mode(true);
            CmdFrameT read_fw_cmd = this->prepare_read_fw_cmd();
            this->send_command(read_fw_cmd);
            CmdFrameT read_config_cmd = this->prepare_read_config_cmd();
            this->send_command(read_config_cmd);
            this->set_config_mode(false);
        }

        void LD2410S::loop() {
            if (!this->cmd_active) {
                static uint8_t buffer[64];
                static size_t pos = 0;
                while (available()) {
                    PackageType type = this->read_line(read(), buffer, pos++);
                    if (type == PackageType::SHORT_DATA || type == PackageType::TRESHOLD) {
                        this->process_data_package(type, buffer, pos);
                        pos = 0;
                    }
                }
            }
        }

        void LD2410S::set_config_mode(bool enabled) {
            CmdFrameT start_cfg;
            start_cfg.header = CMD_FRAME_HEADER;
            start_cfg.command = enabled ? START_CONFIG_MODE_CMD : END_CONFIG_MODE_CMD;
            start_cfg.data_length = 0;
            if (enabled)
            {
                memcpy(&start_cfg.data[0], &START_CONFIG_MODE_VALUE, sizeof(START_CONFIG_MODE_VALUE));
                start_cfg.data_length += sizeof(START_CONFIG_MODE_VALUE);
            }

            start_cfg.footer = CMD_FRAME_FOOTER;
            this->send_command(start_cfg);
        }

        void LD2410S::apply_config() {
            this->status_set_warning("Sending command to sensor");
            this->set_config_mode(true);
            CmdFrameT apply_config_cmd = this->prepare_apply_config_cmd();
            this->send_command(apply_config_cmd);
            this->set_config_mode(false);
            this->status_clear_warning();
        }

        void LD2410S::start_auto_threshold_update() {
            this->status_set_warning("Sending command to sensor");
            this->set_config_mode(true);
            CmdFrameT threshold_update_cmd = this->prepare_threshold_cmd();
            this->send_command(threshold_update_cmd);
            this->set_config_mode(false);
            this->status_clear_warning();
        }

        CmdFrameT LD2410S::prepare_read_config_cmd() {
            CmdFrameT cmd_frame;
            cmd_frame.header = CMD_FRAME_HEADER;
            cmd_frame.command = READ_PARAMS_CMD;
            cmd_frame.data_length = 0;

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_MAX_DETECTION_VALUE, sizeof(CFG_MAX_DETECTION_VALUE));
            cmd_frame.data_length += sizeof(CFG_MAX_DETECTION_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_MIN_DETECTION_VALUE, sizeof(CFG_MIN_DETECTION_VALUE));
            cmd_frame.data_length += sizeof(CFG_MIN_DETECTION_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_NO_DELAY_VALUE, sizeof(CFG_NO_DELAY_VALUE));
            cmd_frame.data_length += sizeof(CFG_NO_DELAY_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_STATUS_FREQ_VALUE, sizeof(CFG_STATUS_FREQ_VALUE));
            cmd_frame.data_length += sizeof(CFG_STATUS_FREQ_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_DISTANCE_FREQ_VALUE, sizeof(CFG_DISTANCE_FREQ_VALUE));
            cmd_frame.data_length += sizeof(CFG_DISTANCE_FREQ_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_RESPONSE_SPEED_VALUE, sizeof(CFG_RESPONSE_SPEED_VALUE));
            cmd_frame.data_length += sizeof(CFG_RESPONSE_SPEED_VALUE);

            cmd_frame.footer = CMD_FRAME_FOOTER;
            return cmd_frame;
        }

        CmdFrameT LD2410S::prepare_apply_config_cmd() {
            CmdFrameT cmd_frame;
            cmd_frame.header = CMD_FRAME_HEADER;
            cmd_frame.command = WRITE_PARAMS_CMD;
            cmd_frame.data_length = 0;

            Config to_save = this->new_config;

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_MAX_DETECTION_VALUE, sizeof(CFG_MAX_DETECTION_VALUE));
            cmd_frame.data_length += sizeof(CFG_MAX_DETECTION_VALUE);
            memcpy(&cmd_frame.data[cmd_frame.data_length], &to_save.max_dist, sizeof(to_save.max_dist));
            cmd_frame.data_length += sizeof(to_save.max_dist);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_MIN_DETECTION_VALUE, sizeof(CFG_MIN_DETECTION_VALUE));
            cmd_frame.data_length += sizeof(CFG_MIN_DETECTION_VALUE);
            memcpy(&cmd_frame.data[cmd_frame.data_length], &to_save.min_dist, sizeof(to_save.min_dist));
            cmd_frame.data_length += sizeof(to_save.min_dist);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_NO_DELAY_VALUE, sizeof(CFG_NO_DELAY_VALUE));
            cmd_frame.data_length += sizeof(CFG_NO_DELAY_VALUE);
            memcpy(&cmd_frame.data[cmd_frame.data_length], &to_save.delay, sizeof(to_save.delay));
            cmd_frame.data_length += sizeof(to_save.delay);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_STATUS_FREQ_VALUE, sizeof(CFG_STATUS_FREQ_VALUE));
            cmd_frame.data_length += sizeof(CFG_STATUS_FREQ_VALUE);
            memcpy(&cmd_frame.data[cmd_frame.data_length], &to_save.status_freq, sizeof(to_save.status_freq));
            cmd_frame.data_length += sizeof(to_save.status_freq);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_DISTANCE_FREQ_VALUE, sizeof(CFG_DISTANCE_FREQ_VALUE));
            cmd_frame.data_length += sizeof(CFG_DISTANCE_FREQ_VALUE);
            memcpy(&cmd_frame.data[cmd_frame.data_length], &to_save.dist_freq, sizeof(to_save.dist_freq));
            cmd_frame.data_length += sizeof(to_save.dist_freq);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &CFG_RESPONSE_SPEED_VALUE, sizeof(CFG_RESPONSE_SPEED_VALUE));
            cmd_frame.data_length += sizeof(CFG_RESPONSE_SPEED_VALUE);
            memcpy(&cmd_frame.data[cmd_frame.data_length], &to_save.resp_speed, sizeof(to_save.resp_speed));
            cmd_frame.data_length += sizeof(to_save.resp_speed);

            cmd_frame.footer = CMD_FRAME_FOOTER;
            return cmd_frame;
        }

        CmdFrameT LD2410S::prepare_threshold_cmd() {
            CmdFrameT cmd_frame;
            cmd_frame.header = CMD_FRAME_HEADER;
            cmd_frame.command = AUTO_UPDATE_THRESHOLD_CMD;
            cmd_frame.data_length = 0;

            memcpy(&cmd_frame.data[cmd_frame.data_length], &THRESHOLD_TRIGGER_VALUE, sizeof(THRESHOLD_TRIGGER_VALUE));
            cmd_frame.data_length += sizeof(THRESHOLD_TRIGGER_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &THRESHOLD_RETENTION_VALUE, sizeof(THRESHOLD_RETENTION_VALUE));
            cmd_frame.data_length += sizeof(THRESHOLD_RETENTION_VALUE);

            memcpy(&cmd_frame.data[cmd_frame.data_length], &THRESHOLD_TIME_VALUE, sizeof(THRESHOLD_TIME_VALUE));
            cmd_frame.data_length += sizeof(THRESHOLD_TIME_VALUE);

            cmd_frame.footer = CMD_FRAME_FOOTER;
            return cmd_frame;
        }

        CmdFrameT LD2410S::prepare_read_fw_cmd() {
            CmdFrameT cmd_frame;
            cmd_frame.header = CMD_FRAME_HEADER;
            cmd_frame.command = READ_FW_CMD;
            cmd_frame.data_length = 0;
            cmd_frame.footer = CMD_FRAME_FOOTER;
            return cmd_frame;
        }

        void LD2410S::send_command(CmdFrameT frame)
        {
            this->cmd_active = true;
            uint32_t start_millis = millis();
            uint8_t retry = 3;
            uint8_t cmd_buffer[64];

            while (retry)
            {
                frame.length = 0;
                uint16_t frame_data_bytes = frame.data_length + 2;
                // HEADER
                memcpy(&cmd_buffer[frame.length], &frame.header, sizeof(frame.header));
                frame.length += sizeof(frame.header);
                // SIZE
                memcpy(&cmd_buffer[frame.length], &frame_data_bytes, sizeof(frame.data_length));
                frame.length += sizeof(frame.data_length);
                // COMMAND
                memcpy(&cmd_buffer[frame.length], &frame.command, sizeof(frame.command));
                frame.length += sizeof(frame.command);
                // DATA
                for (uint16_t index = 0; index < frame.data_length; index++)
                {
                    memcpy(&cmd_buffer[frame.length], &frame.data[index], sizeof(frame.data[index]));
                    frame.length += sizeof(frame.data[index]);
                }
                // FOOTER
                memcpy(cmd_buffer + frame.length, &frame.footer, sizeof(frame.footer));
                frame.length += sizeof(frame.footer);
                // WRITE
                for (uint16_t index = 0; index < frame.length; index++)
                {
                    this->write_byte(cmd_buffer[index]);
                }

                this->flush();

                bool reply = false;

                while (!reply) {
                    uint8_t ack_buffer[64];
                    size_t last_pos = 0;
                    while (available()) {
                        PackageType type = this->read_line(read(), ack_buffer, last_pos++);
                        if (type == PackageType::ACK) {
                            reply = this->process_cmd_ack_package(ack_buffer, last_pos + 1);
                            last_pos = 0;
                        }
                    }
                    delay_microseconds_safe(1450);
                    if ((millis() - start_millis) > 1000)
                    {
                        start_millis = millis();
                        retry--;
                        break;
                    }
                }
                if (reply)
                {
                    retry = 0;
                }
            }
            this->cmd_active = false;
        }

        PackageType LD2410S::read_line(uint8_t data, uint8_t* buffer, size_t pos) {
            buffer[pos] = data;

            if (pos > 4) {
                if (memcmp(&buffer[pos - 3], &CMD_FRAME_FOOTER, sizeof(CMD_FRAME_FOOTER)) == 0) {
                    return PackageType::ACK;
                }
                else if (buffer[pos] == DATA_FRAME_FOOTER && buffer[pos - 4] == DATA_FRAME_HEADER) {
                    return PackageType::SHORT_DATA;
                }
                else if (memcmp(&buffer[pos - 3], &THRESHOLD_FOOTER, sizeof(THRESHOLD_FOOTER)) == 0) {
                    return PackageType::TRESHOLD;
                }
            }
            return PackageType::UNKNOWN;
        }

        void LD2410S::process_config_read_ack(uint8_t* data) {
            int max_dist = this->read_int(data, 0, 4);
            int min_dist = this->read_int(data, 4, 4);
            int delay = this->read_int(data, 8, 4);
            int status_resp_freq = this->read_int(data, 12, 4);
            int dist_resp_freq = this->read_int(data, 16, 4);
            int resp_speed = this->read_int(data, 20, 4);
#ifdef USE_NUMBER
            this->max_distance_number->publish_state(max_dist);
            this->min_distance_number->publish_state(min_dist);
            this->no_delay_number->publish_state(delay);
            this->status_reporting_freq_number->publish_state(status_resp_freq / 10);
            this->distance_reporting_freq_number->publish_state(dist_resp_freq / 10);
#endif
#ifdef USE_SELECT
            this->response_speed_select->publish_state(resp_speed == 5 ? RESPONSE_SPEED_NORMAL : RESPONSE_SPEED_FAST);
#endif
            memcpy(&this->new_config, &this->current_config, sizeof(this->current_config));
            ESP_LOGD(TAG, "Read config replay: max_dist=%d, min_dist=%d, delay=%d, status_resp_freq=%d, dist_resp_freq=%d, resp_speed=%d", max_dist, min_dist, delay, status_resp_freq, dist_resp_freq, resp_speed);
        }

        void LD2410S::process_read_fw_ack(uint8_t* data) {
            int major_v = static_cast<int>(data[0]);
            int minor_v = static_cast<int>(data[1]);
            int patch_v = static_cast<int>(data[2]);
            std::string version = "v" + std::to_string(major_v) + "." + std::to_string(minor_v) + "." + std::to_string(patch_v);
            for (auto& listener : this->listeners) {
                listener->on_fw_version(version);
            }
            ESP_LOGD(TAG, "Read firmware replay: %s", version.c_str());
        }

        bool LD2410S::process_cmd_ack_package(uint8_t* buffer, int len) {
            CmdAckT ack = this->parse_ack(buffer, len);
            int command_word = ack.command;
            bool result = ack.result;
            if (!result) {
                ESP_LOGW(TAG, "Command %x failed", command_word);
                return false;
            }
            else {
                ESP_LOGI(TAG, "Command %x success", command_word);
            }

            uint8_t* data = ack.data;

            switch (command_word) {
            case START_CONFIG_MODE_REPLAY:
                ESP_LOGD(TAG, "Config mode enabled");
                break;
            case END_CONFIG_MODE_REPLAY:
                ESP_LOGD(TAG, "Config mode disabled");
                break;
            case READ_PARAMS_REPLAY:
                this->process_config_read_ack(data);
                break;
            case WRITE_PARAMS_REPLAY:
                ESP_LOGD(TAG, "Write config replay processed");
                break;
            case READ_FW_REPLAY:
                this->process_read_fw_ack(data);
                break;
            default:
                ESP_LOGD(TAG, "Unknown replay: %x", command_word);
                break;
            }

            return true;
        }

        void LD2410S::process_short_data_package(uint8_t* data) {
            const bool presenceState = data[0] > 1;
            int distance = this->two_byte_to_int(data[1], data[2]);
            for (auto& listener : this->listeners) {
                listener->on_presence(presenceState);
                listener->on_distance(distance);
            }
        }

        void LD2410S::process_threshold_package(uint8_t* data) {
            int progress = this->two_byte_to_int(data[3], data[4]);
            for (auto& listener : this->listeners) {
                if (progress == 100) {
                    listener->on_threshold_progress(0);
                    listener->on_threshold_update(false);
                }
                else {
                    listener->on_threshold_progress(progress);
                    listener->on_threshold_update(true);
                }
            }
        }

        void LD2410S::process_data_package(PackageType type, uint8_t* buffer, size_t pos) {
            switch (type) {
            case PackageType::SHORT_DATA:
                this->process_short_data_package(&buffer[1]);
                break;
            case PackageType::TRESHOLD:
                this->process_threshold_package(&buffer[4]);
                break;
            default:
                ESP_LOGD(TAG, "Unexpected package type");
                break;
            }
        }

        float LD2410S::get_setup_priority() const
        {
            return setup_priority::HARDWARE;
        }

        CmdAckT LD2410S::parse_ack(uint8_t* buffer, size_t length) {
            CmdAckT result;
            size_t start = -1;
            for (size_t i = 0; i < length; i++) {
                if (memcmp(&buffer[i], &CMD_FRAME_HEADER, sizeof(CMD_FRAME_HEADER)) == 0) {
                    start = i;
                    break;
                }
            }
            if (start == -1) {
                ESP_LOGE(TAG, "Can't find cmd header");
                result.result = false;
                return result;
            }
            int data_length = this->two_byte_to_int(buffer[start + 4], buffer[start + 5]);
            result.length = data_length;
            int command_word = this->two_byte_to_int(buffer[start + 6], buffer[start + 7]);
            result.command = command_word;
            bool ack = buffer[start + 8] == 0x00 && buffer[start + 9] == 0x00;
            result.result = ack;
            // memcpy(&result.data, &buffer[start + 10], sizeof(uint8_t) * result.length);
            for (size_t idx = 0; idx < result.length; idx++) {
                memcpy(&result.data[idx], &buffer[idx + 10], sizeof(buffer[idx + 10]));
            }
            return result;
        }
    }
}