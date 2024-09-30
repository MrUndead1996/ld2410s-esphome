#include "esphome/core/log.h"
#include "LD2410S.h"

namespace esphome
{
    namespace ld2410s
    {

        static const char* TAG = "ld2410s";

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

                bool reply = false;
                while (!reply)
                {
                    while (available())
                    {
                        PackageType type = this->read_line(read());
                        if (type == PackageType::ACK)
                        {
                            reply = this->process_cmd_ack_package(buffer.data(), buffer.size());
                            buffer.clear();
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
            this->buffer.clear();
        }

        PackageType LD2410S::read_line(uint32_t data)
        {
            buffer.push_back(data);
            size_t pos = buffer.size() - 1;

            if (pos > 4)
            {
                if (memcmp(&buffer[pos - 4], &CMD_FRAME_FOOTER, sizeof(CMD_FRAME_FOOTER)) == 0)
                {
                    return PackageType::ACK;
                }
                else if (buffer[pos] == DATA_FRAME_FOOTER && buffer[pos - 4] == DATA_FRAME_HEADER)
                {
                    return PackageType::SHORT_DATA;
                }
                else if (memcmp(&buffer[pos - 4], &THRESHOLD_FOOTER, sizeof(THRESHOLD_FOOTER)) == 0)
                {
                    return PackageType::TRESHOLD;
                }
            }
            return PackageType::UNKNOWN;
        }
        bool LD2410S::process_cmd_ack_package(uint8_t* buffer, int len)
        {
            uint8_t command_word_bytes[] = { buffer[6], buffer[7] };
            int command_word = this->bytes_to_int(command_word_bytes, sizeof(command_word_bytes));
            uint8_t result_bytes[] = { buffer[8], buffer[9] };
            bool result = static_cast<bool>(this->bytes_to_int(result_bytes, sizeof(result_bytes)));

            if (result)
            {
                ESP_LOGW(TAG, "Command %d failed", command_word);
                return false;
            }

            switch (command_word)
            {
            case START_CONFIG_MODE_REPLAY:
                this->config_mode_active = true;
                ESP_LOGD(TAG, "Config mode enabled");
                break;
            case END_CONFIG_MODE_REPLAY:
                this->config_mode_active = false;
                ESP_LOGD(TAG, "Config mode disabled");
                break;
            case READ_PARAMS_REPLAY:
            {
                uint8_t max_dist_bytes[] = { buffer[10], buffer[11], buffer[12], buffer[13] };
                int max_dist = this->bytes_to_int(max_dist_bytes, sizeof(max_dist_bytes));

                uint8_t min_dist_bytes[] = { buffer[14], buffer[15], buffer[16], buffer[17] };
                int min_dist = this->bytes_to_int(min_dist_bytes, sizeof(min_dist_bytes));

                uint8_t delay_bytes[] = { buffer[18], buffer[19], buffer[20], buffer[21] };
                int delay = this->bytes_to_int(delay_bytes, sizeof(delay_bytes));

                uint8_t status_resp_freq_bytes[] = { buffer[22], buffer[23], buffer[24], buffer[25] };
                int status_resp_freq = this->bytes_to_int(status_resp_freq_bytes, sizeof(status_resp_freq_bytes));

                uint8_t dist_resp_freq_bytes[] = { buffer[26], buffer[27], buffer[28], buffer[29] };
                int dist_resp_freq = this->bytes_to_int(dist_resp_freq_bytes, sizeof(dist_resp_freq_bytes));

#ifdef USE_NUMBER
                this->max_distance_number->publish_state(max_dist);
                this->min_distance_number->publish_state(min_dist);
                this->no_delay_number->publish_state(delay);
                this->status_reporting_freq_number->publish_state(status_resp_freq / 10);
                this->distance_reporting_freq_number->publish_state(dist_resp_freq / 10);
#endif

                uint8_t resp_speed_bytes[] = { buffer[30], buffer[31], buffer[32], buffer[33] };
                int resp_speed = this->bytes_to_int(resp_speed_bytes, sizeof(resp_speed_bytes));
#ifdef USE_SELECT
                this->response_speed_select->publish_state(resp_speed == 5 ? RESPONSE_SPEED_NORMAL : RESPONSE_SPEED_FAST);
#endif
                Config current_cfg, new_cfg;
                current_cfg.max_dist = max_dist;
                current_cfg.min_dist = min_dist;
                current_cfg.delay = delay;
                current_cfg.status_freq = status_resp_freq;
                current_cfg.dist_freq = dist_resp_freq;
                current_cfg.resp_speed = resp_speed;
                memcpy(&new_cfg, &current_cfg, sizeof(new_cfg));
                this->current_config = current_cfg;
                this->new_config = new_cfg;
                ESP_LOGD(TAG, "Read config replay processed");
            }
            break;
            case WRITE_PARAMS_REPLAY:
                ESP_LOGD(TAG, "Write config replay processed");
                break;
            case READ_FW_REPLAY:
            {
                size_t pos = len - 1;
                uint8_t major_bytes[] = { buffer[pos - 3] };
                int major = this->bytes_to_int(major_bytes, sizeof(major_bytes));
                uint8_t minor_bytes[] = { buffer[pos - 2] };
                int minor = this->bytes_to_int(minor_bytes, sizeof(minor_bytes));
                uint8_t patch_bytes[] = { buffer[pos - 1] };
                int patch = this->bytes_to_int(patch_bytes, sizeof(patch_bytes));
                std::string version = "v" + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
                for (auto& listener : this->listeners) {
                    listener->on_fw_version(version);
                }
            }
            break;
            default:
                ESP_LOGD(TAG, "Unknown replay: %d", command_word);
                break;
            }
            this->buffer.clear();
            return true;
        }

        void LD2410S::process_data_package(PackageType type)
        {
            size_t pos = buffer.size() - 1;
            switch (type)
            {
            case PackageType::SHORT_DATA:
            {
                const bool presenceState = buffer[pos - 3] > 1;
                uint8_t dist_bytes[] = { buffer[pos - 2], buffer[pos - 1] };
                int distance = bytes_to_int(dist_bytes, sizeof(dist_bytes));
                for (auto& listener : this->listeners)
                {
                    listener->on_presence(presenceState);
                    listener->on_distance(distance);
                }
            }
            break;
            case PackageType::TRESHOLD:
            {
                uint8_t bytes[] = { buffer[pos - 6], buffer[pos - 5] };
                int progress = this->bytes_to_int(bytes, sizeof(bytes));
                for (auto& listener : this->listeners)
                {
                    if (progress == 100)
                    {
                        listener->on_threshold_progress(0);
                        listener->on_threshold_update(false);
                    }
                    else
                    {
                        listener->on_threshold_progress(progress);
                        listener->on_threshold_update(true);
                    }
                }
            }
            break;
            }
            this->buffer.clear();
        }

        void LD2410S::setup()
        {
            this->set_config_mode(true);
            this->send_read_fw_cmd();
            this->read_current_config();
            this->set_config_mode(false);
        }

        void LD2410S::loop()
        {
            if (!this->cmd_active)
            {
                while (available())
                {
                    PackageType type = this->read_line(read());
                    if (type == PackageType::SHORT_DATA || type == PackageType::TRESHOLD)
                    {
                        this->process_data_package(type);
                        this->buffer.clear();
                    }
                }
            }
        }

        float LD2410S::get_setup_priority() const
        {
            return setup_priority::HARDWARE;
        }

        void LD2410S::set_config_mode(bool enabled)
        {
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

        void LD2410S::read_current_config()
        {
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
            this->send_command(cmd_frame);
        }

        void LD2410S::apply_config()
        {
            this->status_set_warning("Sending command to sensor");
            this->set_config_mode(true);
            this->send_apply_config_cmd();
            this->set_config_mode(false);
            this->status_clear_warning();
        }

        void LD2410S::send_apply_config_cmd()
        {
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
            this->send_command(cmd_frame);
        }

        void LD2410S::start_auto_threshold_update()
        {
            this->status_set_warning("Sending command to sensor");
            this->set_config_mode(true);
            this->send_start_auto_threshold_update_cmd();
            this->set_config_mode(false);
            this->status_clear_warning();
        }

        void LD2410S::send_start_auto_threshold_update_cmd() {
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
            this->send_command(cmd_frame);
        }

        void LD2410S::send_read_fw_cmd() {
            CmdFrameT cmd_frame;
            cmd_frame.header = CMD_FRAME_HEADER;
            cmd_frame.command = READ_FW_CMD;
            cmd_frame.data_length = 0;
            cmd_frame.footer = CMD_FRAME_FOOTER;
            this->send_command(cmd_frame);
        }
    }
}