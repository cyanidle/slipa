#ifndef SLIPA_HPP
#define SLIPA_HPP
#pragma once

#include <type_traits>
#include <string_view>
#include <stdlib.h>
#include <assert.h>

namespace slipa
{

enum Special : char {
    ESC = char(0xDB),
    END = char(0xC0),
    ESC_ESC = char(0xDD),
    ESC_END = char(0xDC),
};

template<typename Fn>
concept handler = requires (Fn f, std::string_view part)
{
    {f(part)} -> std::convertible_to<bool>;
};

using CannotFail = std::false_type;

[[gnu::flatten]]
constexpr auto Write(std::string_view msg, handler auto&& out) noexcept {
    constexpr char toescape[] = {ESC, END, 0};
    constexpr char esc_end[] = {ESC, ESC_END, 0};
    constexpr char esc_esc[] = {ESC, ESC_ESC, 0};
    size_t pos = 0;
    while (pos < msg.size()) {
        auto next = msg.find_first_of(toescape, pos);
        if (next == std::string_view::npos) {
            if (auto err = out(msg.substr(pos))) [[unlikely]] {
                return err;
            }
            break;
        } else {
            if (pos != next) {
                if (auto err = out(msg.substr(pos, next))) [[unlikely]] {
                    return err;
                }
            }
            if (msg[next] == ESC) {
                if (auto err = out(std::string_view{esc_esc})) [[unlikely]] {
                    return err;
                }
            } else { //END
                if (auto err = out(std::string_view{esc_end})) [[unlikely]] {
                    return err;
                }
            }
            pos = next + 1;
        }
    }
    return out(std::string_view{toescape+1});
}

enum ReadErrors {
    NoError = 0,
    UnterminatedEscape = 1,
    InvalidEscape = 2,
    HandlerError = 3,
};

[[gnu::flatten]]
constexpr ReadErrors Read(std::string_view msg, handler auto&& out)
    constexpr char esc[] = {ESC, 0};
    constexpr char end[] = {END, 0};
    if (msg.size() && msg.back() == END) {
        msg = msg.substr(0, msg.size() - 1);
    }
    size_t pos = 0;
    while (pos < msg.size()) {
        auto next = msg.find_first_of(esc, pos);
        if (next == std::string_view::npos) {
            if (out(msg.substr(pos))) [[unlikely]] {
                return HandlerError;
            }
            break;
        } else if (next != msg.size() - 1) {
            if (pos != next) {
                if (out(msg.substr(pos, next))) [[unlikely]] {
                    return HandlerError;
                }
            }
            auto escaped = msg[next + 1];
            if (escaped == ESC_END) {
                if (out(std::string_view{end})) [[unlikely]] {
                    return HandlerError;
                }
            } else if (escaped == ESC_ESC) {
                if (out(std::string_view{esc})) [[unlikely]] {
                    return HandlerError;
                }
            } else {
                return InvalidEscape;
            }
            pos = next + 2;
        } else {
            return UnterminatedEscape;
        }
    }
    return NoError;
}


} //slipa

#endif //SLIPA_HPP
