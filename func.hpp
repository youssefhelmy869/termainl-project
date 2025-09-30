#include <string>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

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

void excute_binary(string command)
{
}