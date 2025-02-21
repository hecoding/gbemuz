#pragma once

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using s8 = std::int8_t;

const size_t CLOCK_FREQUENCY = 4194304;
const float FRAMERATE = 59.63;
const float CYCLES_PER_FRAME = CLOCK_FREQUENCY / FRAMERATE;
