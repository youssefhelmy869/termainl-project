/// the run() takes the main thread while the rest of the mian that run() was called in will be excuted by a logic_thread
//output_screen.hpp

#include <SFML/Graphics.hpp>
#include <thread>
#include <string>
#include <iostream>
#include <mutex>
#include <vector>

using namespace std;
using namespace sf;

class output_screen
{
private:
    RenderWindow *window = nullptr;
    Image image;
    Texture texture;
    Sprite sprite;
    mutex mtx;
    thread logic_thread;
    bool update_image = false;
    bool runing;
    int x;
    int y;
    Font font;
    Text user_input;

    vector<Text> texts;

    string buffer;
    bool ready = false;
    vector<Text> user_input_history;
    int line_height = 28;
    string last_input;

    void handle_text_input(Event &event)
    {
        if (event.type == Event::TextEntered)
        {
            if (event.text.unicode == '\b')
            {
                if (!buffer.empty())
                    buffer.pop_back();
            }
            else if (event.text.unicode == '\r' || event.text.unicode == '\n')
            {
                if (!buffer.empty())
                {
                    add_line(buffer);

                    last_input = buffer;
                    ready = true;
                    buffer.clear();
                }
                ready = true;
            }
            else if (event.text.unicode < 128) // ASCII only
            {
                buffer += static_cast<char>(event.text.unicode);
            }
        }
        user_input.setString(buffer);
    }
    void add_line(const string &str)
    {
        Text line(str, font, 20);
        line.setFillColor(Color::White);
        line.setPosition(10, 10 + user_input_history.size() * line_height);
        user_input_history.push_back(line);
    }

public:
    bool is_ready()
    {
        return ready;
    }
    string getInput()
    {
        if (ready == true)
        {
            ready = false;
            return last_input;
        }
        else
        {
            return "";
        }
    }
    string get_buffer()
    {
        return buffer;
    }

    bool window_is_open;

private:
    void create_window()
    {

        // start loop

        window_is_open = true;

        while (window->isOpen() && runing == true)
        {
            window->clear();
            Event user_event;

            while (window->pollEvent(user_event))
            {
                if (user_event.type == Event::Closed)
                {
                    window->close();
                    lock_guard<mutex> lock(mtx);
                    window_is_open = false;
                }
                handle_text_input(user_event);
            }

            if (update_image == true)
            {
                texture.update(image);
                update_image = false;
            }

            window->draw(sprite);
            lock_guard<mutex> lock(mtx);
            if (!user_input_history.empty())
            {
                window->draw(user_input_history.back());
            }

            for (const auto &t : texts)
            {
                window->draw(t);
            }
            window->draw(user_input);

            window->display();
        }
        window_is_open = false;
    }

public:
    void write_to_window(string data, int x_pos, int y_pos, int size = 20)
    {
        Text text(data, font, size);
        text.setFont(font);
        text.setFillColor(Color::White);
        text.setPosition(x_pos, y_pos);
        lock_guard<mutex> lock(mtx);
        texts.push_back(text);
    }

    output_screen(string window_name = "output window", int para_x = 800, int para_y = 800)
    {
        x = para_x;
        y = para_y;
        window = new RenderWindow(VideoMode(x, y), window_name);
        image.create(x, y);
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
        {
            cerr << "File not found" << endl;
        }
        user_input.setFont(font);
        user_input.setCharacterSize(24);
        user_input.setFillColor(Color::Green);
        user_input.setPosition(10, y - 40); // bottom of window
    }

    void add_pixel(int x_position, int y_postion, Color colour = Color::White)
    {

        cout << "adding pixsel" << endl;
        image.setPixel(x_position, y_postion, colour);
        update_image = true;
    }

    template <typename Func>
    void run(Func rest_of_main)
    {
        cout << "run function called" << endl;
        runing = true;
        cout << "logic thread created" << endl;
        logic_thread = thread([&, rest_of_main]()
                              { rest_of_main(); });

        create_window();

        if (logic_thread.joinable())
        {
            logic_thread.join();
        }
    }

    ~output_screen()
    {
        runing = false;
        if (window && window->isOpen())
            window->close();
        if (logic_thread.joinable())
            logic_thread.join();
    }
};