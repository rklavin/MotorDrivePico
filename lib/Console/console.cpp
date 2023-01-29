#include "console.h"
#include "pico/stdlib.h"

#include "Interface/interface.h"

namespace console {
    std::vector<std::string> split_command_and_args(int timeout_ms) {
        // read serial until a space or line break for command
        // if we got a space keep reading in arguments to array, separated by spaces
        // read until line break or timeout

        // timeout after an eternity
        if (timeout_ms <= 0) timeout_ms = INT32_MAX;

        std::vector<std::string> res;
        std::string arg = "";

	    int time = to_ms_since_boot(get_absolute_time());
	    int chr = interface_getchar_timeout_ms(10);
        while (chr != '\n' && chr != '\r') {
            if (chr != ' ' && chr > 0) {
                arg += chr;
                // reset timeout if we got a character
                time = to_ms_since_boot(get_absolute_time());
            } else {
                // ignore spaces at the start
                if (arg.length() > 0) break;
            }

            // check timeout
            if (to_ms_since_boot(get_absolute_time()) - time > timeout_ms) {
                stdio_flush();
                return res;
            } else {
                chr = interface_getchar_timeout_ms(10);
            }
        }

        res.push_back(arg);
        if (chr == '\n' || chr == '\r') {
            // discard last character
            chr = interface_getchar_timeout_ms(10);
            stdio_flush();
            return res;
        }

        arg = "";
        chr = interface_getchar_timeout_ms(10);
        while (chr != '\n' && chr != '\r') {
            // arguments are separated by a space
            if (chr != ' ' && chr > 0) {
                arg += chr;
                // reset timeout if we got a character
                time = to_ms_since_boot(get_absolute_time());
            } else {
                if (arg.length() > 0) res.push_back(arg);
                arg = "";
            }
            
            // check timeout
            if (to_ms_since_boot(get_absolute_time()) - time > timeout_ms) {
                stdio_flush();
                return res;
            } else {
                chr = interface_getchar_timeout_ms(10);
            }
        }

        // add last argument
        res.push_back(arg);

        // discard last character
        chr = interface_getchar_timeout_ms(10);
        stdio_flush();
        return res;
    }
}