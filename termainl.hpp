// termainl.hpp

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
#include <thread>

using namespace sf;
using namespace std;

class termainl
{
private:
    output_screen scrn;
    char output_buffer[256];
    thread excuting_thread;
    vector<string> commands;

public:
    int command_index = 0;

private:
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

        int x = binary_to_decimal(x_pos);
        int y = binary_to_decimal(y_pos);
        int red = binary_to_decimal(s_red);
        int green = binary_to_decimal(s_green);
        int blue = binary_to_decimal(s_blue);

        Color c = calculate_colour(red, green, blue, 255);

        cout << "x = " << x << endl;
        cout << "y = " << y << endl;
        cout << " red " << " | " << " blue" << " | " << " green" << endl;
        cout << " " << red << " " << blue << " " << green << endl;
        scrn.add_pixel(x, y, c);
    }

public:
    void execute_binary(const string &command)
    {

        bool is_binary = true;
        for (char ch : command)
        {
            if (ch != '1' && ch != '0')
            {
                is_binary = false;
            }
        }

        if (is_binary == true)
        {
            cout << "command is binary" << endl;
            string main_command = command.substr(0, 4);
            cout << "command number" << command_index << endl;
            cout << "main command = " << main_command << endl;
            cout << "command size = " << command.size() << endl;

            if (main_command == "0000") // main function to excut incoming binarys
                                        // display a pixsel

            {
                if (command.size() != 60)
                {
                    cerr << "command size is not right" << endl;
                }

                cout << "drawing pixsel" << endl;
                draw_pixel(command);
            }
            else if (main_command == "0001") // write the last data added
            {
                string data = commands[command_index -1];
                cout << "data to write to screen = " << data << endl;

                // get x and y
                string s_x = command.substr(4, 16);
                string s_y = command.substr(20, 16);

                // convert to ints
                int x = binary_to_decimal(s_x);
                int y = binary_to_decimal(s_y);
                cout << "x = " << x << endl;
                cout << "y =" << y << endl;

                scrn.write_to_window(data, x, y);
            }
        }
        else
        {
            cout << "command is data" << endl;
        }
    }

private:
    void spawn_process()
    {
        cout << "creating subprocess" << endl;
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
            DWORD error = GetLastError();
            cerr << "Failed to start process: " << cmd << endl;
            cerr << "error( " << error << " )" << endl;
        }
    }

    void create_pipline()
    {
        cout << "creating pipline" << endl;
        HANDLE hPipe = CreateNamedPipe(R"(\\.\pipe\terminalPipe)", // Pipe name
                                       PIPE_ACCESS_INBOUND,        // Only read from this side
                                       PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                       1, 1024, 1024, 0, NULL);

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            cerr << "Failed to create pipe" << endl;
        }
        else
        {
            cout << "termainl.hpp has made the pipeline" << endl;
        }
        cout << "Waiting for input process to connect..." << endl;
        ConnectNamedPipe(hPipe, NULL); // Step 2: wait
        cout << "Connected!" << endl;

        excuting_thread = thread([this, hPipe]()
                                 {
    DWORD bytesRead;

    while (true) {
        if (ReadFile(hPipe, output_buffer, sizeof(output_buffer), &bytesRead, NULL)) {
            string msg(output_buffer, bytesRead);
            cout << "got :" << msg << endl;
            commands.push_back(msg);
            execute_binary(msg);
            ++command_index; 
        }
    }
    CloseHandle(hPipe); });

        excuting_thread.detach();
    }

public:
    template <typename Func>
    termainl(Func f)
    {
        // Start two screen processes

        spawn_process();
        create_pipline();

        // problem with pipline and process timing
        scrn.run([&]()
                 { cout << "the output screen is on" << endl; });
    }
};
