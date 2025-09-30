//termainl.hpp

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "output_screen.hpp"
#include <bitset>
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define NOGDI
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0
#include <windows.h>

using namespace sf;
using namespace std;

class termainl
{
private:
    output_screen scrn;

    template <size_t size>
    string decimal_to_binary(int num)
    {
        bitset<size> binary(num);
        return binary.to_string();
    }

    int binary_to_decimal(const string &number)
    {
        return static_cast<int>(stoul(number, nullptr, 2));
    }

    Color calculate_colour(int red, int green, int blue, int alpha)
    {
        if (red < 0 || red > 255 ||
            green < 0 || green > 255 ||
            blue < 0 || blue > 255 ||
            alpha < 0 || alpha > 255)
        {
            cerr << "Colour value out of range [0-255]" << endl;
        }
        return Color(red, green, blue, alpha);
    }

    void draw_pixel(const string &command)
    {
        string x_pos = command.substr(4, 16);
        string y_pos = command.substr(20, 16);
        string s_red = command.substr(36, 8);
        string s_green = command.substr(44, 8);
        string s_blue = command.substr(52, 8);
        string s_alpha = command.substr(60, 8);

        int x = binary_to_decimal(x_pos);
        int y = binary_to_decimal(y_pos);
        int red = binary_to_decimal(s_red);
        int green = binary_to_decimal(s_green);
        int blue = binary_to_decimal(s_blue);
        int alpha = binary_to_decimal(s_alpha);

        Color c = calculate_colour(red, green, blue, alpha);
        scrn.add_pixel(x, y, c);
    }

public:
    void execute_binary(const string &command)
    {
        string main_command = command.substr(0, 4);
        if (main_command == "0000")
        {
            draw_pixel(command);
        }
    }

private:
    void spawn_process()
    {
        char cmd[] = "C:\\Users\\ahmed\\OneDrive\\Desktop\\termainl-project\\output\\screen_process.exe";

        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcessA(
                nullptr,
                cmd,
                nullptr,
                nullptr,
                FALSE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi))
        {
            cout << "Started subprocess: " << cmd
                 << " (PID " << pi.dwProcessId << ")" << endl;

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else
        {
            cerr << "Failed to start process: " << cmd << endl;
        }
    }

public:
    template <typename Func>
    termainl(Func f)
    {
        // Start two screen processes
        spawn_process();
        scrn.run([&]() {});
    }
};
