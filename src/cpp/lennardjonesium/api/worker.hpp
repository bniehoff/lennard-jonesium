/**
 * worker.hpp
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

#ifndef LJ_WORKER_HPP
#define LJ_WORKER_HPP

#include <string>
#include <utility>
#include <memory>
#include <thread>
#include <chrono>

#include <boost/iostreams/filtering_stream.hpp>

#include <lennardjonesium/tools/text_buffer.hpp>

namespace api
{
    class Worker
    {
        public:
            virtual void launch(int count = 10);
    };

    class AsyncWorker : public Worker
    {
        public:
            virtual void launch(int count = 10) override;

            void wait();

            // std::pair<bool, std::string> get();

            std::string read();
            bool eof();

            ~AsyncWorker() noexcept;
        
        private:
            std::unique_ptr<tools::TextBuffer> buffer_;
            std::jthread producer_;
            boost::iostreams::filtering_ostream out_stream_;
    };
} // namespace api

#endif
