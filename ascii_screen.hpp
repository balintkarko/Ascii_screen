// ascii_screen.hpp 
// v1.01 - incomplete features removed
// 
// Simple GUI. Output through standard output. Windows can be created. Windows 
// contain lines. Superfluous characters within lines and superfluous lines 
// are omitted. 
// Balint Karko 2024.06.15.

#ifndef ASCII_SCREEN_HPP
#define ASCII_SCREEN_HPP

#include <string>
#include <vector>
#include <iterator>
#include <functional>

namespace Ascii_screen {
    class Window;

    struct Coord {
        Coord();
        Coord(int xx, int yy);
        Coord(const Coord& c);
        Coord(Coord&& c);
        ~Coord();
        Coord& operator=(const Coord& c);
        Coord& operator=(Coord&& c);

        int x;
        int y;
    };

    class Screen {
    public:
        class Invalid_screen { };
        Screen();
        Screen(int width, int height);
        Screen(const Screen& screen);
        Screen(Screen&& screen);
        ~Screen();
        Screen& operator=(const Screen& screen);
        Screen& operator=(Screen&& screen);

        std::string clear() const;
        void update() const;
        void attach(Window& window);
        void detach(Window& window);

        std::vector<Window*>::iterator find_win(Window& win);
        std::vector<Window*>::const_iterator find_win(const Window& win) const;
        bool is_win(const Window& win) const;
        int width() const { return width_; }
        int height() const { return height_; }
    private:
        int width_;
        int height_;
        // Higher indexed window is drawn on top
        std::vector<Window*> windows_;

        bool is_valid_screen(const std::vector<std::string>& lines) const;
        std::vector<std::string> default_screen() const;
        std::vector<std::string> draw_window(
            std::vector<std::string> lines, const Window& win
            ) const;
        std::string refresh() const;
        void print(const std::string& s) const;
    };

    class Window {
    public:
        class Invalid_window { };
        Window();
        Window(Coord coord, int width, int height); 
        Window(const Window& window);
        Window(Window&& window);
        ~Window();
        Window& operator=(const Window& window);
        Window& operator=(Window&& window);

        void set_lines(std::vector<std::string> lines) { lines_ = lines; }
        void set_lines(std::string text);
        void attach(Screen* screen);
        void detach();

        Coord coord() const { return coord_; }
        int width() const { return width_; }
        int height() const { return height_; }
        std::vector<std::string> lines() const { return lines_; }
        std::string line(int y) const;
    private:
        Coord coord_;
        int width_;
        int height_;
        std::vector<std::string> lines_;
        Screen* screen_{nullptr};

        std::vector<std::string> default_window() const;
    };
}

#endif
