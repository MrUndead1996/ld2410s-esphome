#pragma once

#include "../LD2410S.h"
#include "esphome/components/number/number.h"

namespace esphome
{
    namespace ld2410s
    {
        class LD2410SMaxDistanceNumber : public number::Number, public Parented<LD2410S>
        {
        public:
            LD2410SMaxDistanceNumber() = default;

        protected:
            void control(float max_distance) override;
        };

        class LD2410SMinDistanceNumber : public number::Number, public Parented<LD2410S>
        {
        public:
            LD2410SMinDistanceNumber() = default;

        protected:
            void control(float min_distance) override;
        };

        class LD2410SDelayNumber : public number::Number, public Parented<LD2410S>
        {
        public:
            LD2410SDelayNumber() = default;

        protected:
            void control(float no_delay) override;
        };

        class LD2410SStatusReportingFreqNumber : public number::Number, public Parented<LD2410S>
        {
        public:
            LD2410SStatusReportingFreqNumber() = default;

        protected:
            void control(float status_reporting_freq) override;
        };

        class LD2410SDistReportingFreqNumber : public number::Number, public Parented<LD2410S>
        {
        public:
            LD2410SDistReportingFreqNumber() = default;

        protected:
            void control(float distance_reporting_freq) override;
        };
    }
}