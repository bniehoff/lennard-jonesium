/**
 * sinks.hpp
 * 
 * Copyright (c) 2021-2022 Benjamin E. Niehoff
 * 
 * This file is part of Lennard-Jonesium.
 * 
 * Lennard-Jonesium is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Lennard-Jonesium is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Lennard-Jonesium.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef LJ_SINKS_HPP
#define LJ_SINKS_HPP

#include <concepts>
#include <iostream>

#include <lennardjonesium/physics/system_state.hpp>
#include <lennardjonesium/output/log_message.hpp>

namespace detail
{
    class SinkCommon
    {
        public:
            virtual void write_header() = 0;

            void flush() {destination_.flush();}

            SinkCommon() = default;
            explicit SinkCommon(std::ostream& destination) : destination_{destination} {}

            virtual ~SinkCommon() = default;

        protected:
            /**
             * NOTE: A Sink does not own its destination stream, the caller must ensure it remains
             * open for the duration that the Sink is writing.
             */
            std::ostream& destination_ = std::cout;
    };

    template<class MessageType>
    class MessageSink
    {
        public:
            virtual void write(int time_step, MessageType message) = 0;
    };
} // namespace detail


namespace output
{
    /**
     * A Sink represents some logging destination.  It could be a file, or the console, or in
     * principle anything.  Sink is a thin wrapper around some actual IO object; its main
     * responsibility is to actually format log messages into strings before they are sent
     * to their final destination.
     * 
     * There are three types of Sinks which record different types of LogMessages to different
     * destinations:
     * 
     *  EventSink
     *  ThermodynamicSink
     *  ObservationSink
     */

    /**
     * A single, polymorphic Sink base class does not make sense, because all of the write() methods
     * accept different parameter types.  Nevertheless, we can create a concept Sink which can
     * check at compile time whether a given sink class implements the correct interface.
     * 
     * Not sure if this will be useful, though.
     */
    template<class SinkType, class... MessageTypes>
    concept Sink = (
        std::derived_from<SinkType, detail::SinkCommon>
        && ... && std::derived_from<SinkType, detail::MessageSink<MessageTypes>>
    );

    /**
     * EventSink will handle writing event messages to the events file, which gives a rough
     * summary of what happened during the simulation.
     */
    class EventSink
        : public detail::SinkCommon,
          public detail::MessageSink<PhaseStartEvent>,
          public detail::MessageSink<AdjustTemperatureEvent>,
          public detail::MessageSink<RecordObservationEvent>,
          public detail::MessageSink<PhaseCompleteEvent>,
          public detail::MessageSink<AbortSimulationEvent>
    {
        public:
            // For the moment, the Events file has no header information
            virtual void write_header() override {}

            virtual void write(int time_step, PhaseStartEvent message) override;
            virtual void write(int time_step, AdjustTemperatureEvent message) override;
            virtual void write(int time_step, RecordObservationEvent message) override;
            virtual void write(int time_step, PhaseCompleteEvent message) override;
            virtual void write(int time_step, AbortSimulationEvent message) override;

            EventSink() = default;
            explicit EventSink(std::ostream& destination) : detail::SinkCommon{destination} {}
    };

    /**
     * ThermodynamicSink will record the raw (instantaneous) thermodynamic measurements to a file,
     * which will allow us to re-analyze them later, if desired.
     */
    class ThermodynamicSink
        : public detail::SinkCommon, public detail::MessageSink<ThermodynamicData>
    {
        public:
            virtual void write_header() override;

            virtual void write(int time_step, ThermodynamicData message) override;

            ThermodynamicSink() = default;

            explicit ThermodynamicSink(std::ostream& destination)
                : detail::SinkCommon{destination}
            {}
    };

    /**
     * ObservationSink will record the statistical-mechanical Observation results to a file.
     */
    class ObservationSink
        : public detail::SinkCommon, public detail::MessageSink<ObservationData>
    {
        public:
            virtual void write_header() override;

            virtual void write(int time_step, ObservationData message) override;

            ObservationSink() = default;
            
            explicit ObservationSink(std::ostream& destination)
                : detail::SinkCommon{destination}
            {}
    };

    /**
     * SystemSnapshotSink will write the positions, velocities, and forces (accelerations) of all
     * particles to a file.  One use is for writing the final state at the end of the simulation.
     * However, it can easily write any number of states, so is also suitable to use for the
     * trajectory output.
     */
    class SystemSnapshotSink
        : public detail::SinkCommon, public detail::MessageSink<SystemSnapshot>
    {
        public:
            virtual void write_header() override;

            virtual void write(int time_step, SystemSnapshot message) override;

            SystemSnapshotSink() = default;
            
            explicit SystemSnapshotSink(std::ostream& destination)
                : detail::SinkCommon{destination}
            {}
    };
} // namespace output


#endif
