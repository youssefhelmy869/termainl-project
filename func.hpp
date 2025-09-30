#include <string>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "output_screen.hpp"
#include <bitset>
#pragma once

using namespace sf;
using namespace std;
class termainl
{
private:
    template <size_t size>
    string decemial_to_binary(int num)
    {
        bitset<size> binary(num);
        string return_value = binary.to_string();
        return return_value;
    }

    int binary_to_decimal(string number)
    {
        int retrun_value = stoul(number, nullptr, 2);
        return retrun_value;
    }

    Color calculate_colour(int red, int green, int blue, int alpha)
    {
        if (red < 0 || red > 255 ||
            green < 0 || green > 255 ||
            blue < 0 || blue > 255 ||
            alpha < 0 || alpha > 255)
        {
            cerr << "Couler value is bigger or equal to 256" << endl;
        }
        Color color(red, green, blue, alpha);
        return color;
    }
    screen scrn;
    void draw_pixsel(string command)
    {
        string x_pos = command.substr(4, 16);
        string y_pos = command.substr(20, 16);
        string s_red = command.substr(36, 8);
        string s_green = command.substr(44, 8);
        string s_blue = command.substr(52, 8);
        string s_alpha = command.substr(60, 8);

        int red, green, blue, alpha;
        red = binary_to_decimal(s_red);
        green = binary_to_decimal(s_green);
        blue = binary_to_decimal(s_blue);
        alpha = binary_to_decimal(s_alpha);

        Color c = calculate_colour(red, green, blue, alpha);
        int x = binary_to_decimal(x_pos);
        int y = binary_to_decimal(y_pos);
        scrn.add_pixel(x, y, c);
    }

public:
    void excute_binary(string command)
    {
        string main_command = command.substr(0, 4);
        if (main_command == "0000")
        {
            draw_pixsel(command);
        }
    }
};