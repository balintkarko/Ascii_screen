// ascii_screen.cpp
// v1.01
// 
// Screens and Windows are copied without pointers: copies will have nullptr-s.
// Balint Karko 2024.06.15.

#include "ascii_screen.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>

using namespace Ascii_screen;

constexpr char DEFAULT_SCREEN_CHARACTER = ' ';
constexpr char DEFAULT_WINDOW_CHARACTER = ' ';

namespace Ascii_screen {
    Coord::Coord() :x{0}, y{0} { }
    Coord::Coord(int xx, int yy) :x{xx}, y{yy} { }
    Coord::Coord(const Coord& c) :x{c.x}, y{c.y} { }
    Coord::Coord(Coord&& c) { x = std::exchange(c.x, 0); y = std::exchange(c.y, 0); }
    Coord::~Coord() { }
    Coord& Coord::operator=(const Coord& c) { x = c.x; y = c.y; }
    Coord& Coord::operator=(Coord&& c) { x = std::exchange(c.x, 0); y = std::exchange(c.y, 0); }

    Screen::Screen() 
        :width_{30}, height_{30}
    {
        
    }

    Screen::Screen(int width, int height)
    {
        if (width < 0 || height < 0) {
            throw Invalid_screen{};
        }
        width_ = width;
        height_ = height;
    }

    Screen::Screen(const Screen& screen)
        :width_{screen.width()}, height_{screen.height()}
    {
        
    }

    Screen::Screen(Screen&& screen)
    {
        
        width_ = std::exchange(screen.width_, 0);
        height_ = std::exchange(screen.height_, 0);
    }

    Screen& Screen::operator=(const Screen& screen)
    {
        
        if (this == &screen) {
            return *this;
        }

        width_ = screen.width();
        height_ = screen.height();
        return *this;
    }

    Screen& Screen::operator=(Screen&& screen)
    {
        
        if (this == &screen) {
            return *this;
        }

        width_ = std::exchange(screen.width_, 0);
        height_ = std::exchange(screen.height_, 0);
        return *this;
    }

    Screen::~Screen()
    {
        for (Window* wp : windows_) {
            detach(*wp);
        }
    }

    constexpr int SCREEN_MAX_HEIGHT = 100;
    // Returns SCREEN_MAX_HEIGHT newlines as a string
    std::string Screen::clear() const
    {
        return std::string(SCREEN_MAX_HEIGHT, '\n');
    }

    // Draws windows on default screen. Returns resulting screen as a string
    std::string Screen::refresh() const
    {
        std::vector<std::string> lines = default_screen();

        // Reads windows_ in order: lowest to hightest priority
        for (Window* wp : windows_) {
            lines = draw_window(lines, *wp);
        }

        std::string screen;
        for (std::string line : lines) {
            screen += line;
            screen.push_back('\n');
        }
        
        return screen;
    }

    // Outputs s to standard output
    void Screen::print(const std::string& s) const
    {
        std::cout << s;
    }

    // Prints updated screen  
    void Screen::update() const
    {
        std::string screen = clear();
        screen += refresh();
        print(screen);
    }

    // Attaches window to screen and screen to window
    // Window must not be attached yet 
    void Screen::attach(Window& window)
    {
        if (is_win(window)) {
            throw std::runtime_error("Screen::attach: window already "
                                        "attached");
        }
        windows_.push_back(&window);
        window.attach(this);
    }

    // Deatches window from screen and screen from window
    void Screen::detach(Window& window)
    {
        auto window_it = find_win(window);
        if (window_it == windows_.end()) {
            throw std::runtime_error("Screen::detach: window not attached"); 
        }
        windows_.erase(window_it);
        window.detach();
    }
    
    // True: correct number of lines, each of correct length
    // False: invalid number of lines, or line of invalid length
    bool Screen::is_valid_screen(const std::vector<std::string>& lines) const 
    {
        if (lines.size() != height_) {
            return false;    
        }
        for (std::string l : lines) {
            if (l.length() != width_) {
                return false;
            }
        }
        return true;
    }

    // Returns default screen in vector<string> format
    std::vector<std::string> Screen::default_screen() const
    // Default screen is DEFAULT_CHARACTER only 
    {
        std::vector<std::string> lines;

        for (int y = 0; y < height_; ++y) {
            lines.push_back(std::string(width_, DEFAULT_SCREEN_CHARACTER));
        }

        return lines;
    }

    // Returns screen in vector<string> format with window added on top. 
    // Screen lines must make valid screen. Each window must have non-negative 
    // sizes. Unspecified window region is filled with DEFAULT_CHARACTER
    std::vector<std::string> Screen::draw_window(
        std::vector<std::string> lines, const Window& win
        ) const
    {
        if (!is_valid_screen(lines)) {
            throw std::runtime_error("Screen::draw_window: invalid screen");
        }

        Coord win_coord = win.coord();
        if (win_coord.x < 0 || win_coord.y < 0) {
            throw std::runtime_error("Screen::draw_window: negative window "
                                        "coordinate is not supported");
        }
        int win_shown_width = std::min(width_ - win_coord.x, win.width());
        int win_shown_height = std::min(height_ - win_coord.y, win.height());
        
        
        for(int i = 0; i < win_shown_height; ++i) {
            std::string win_line;
            if (i < win.lines().size()) {
                win_line = win.line(i);    
            }
                        
            // Make win_line correct length
            if (win_shown_width < win_line.length()) {
                win_line = win_line.substr(0, win_shown_width);
            }
            else if (win_shown_width > win_line.length()) {
                win_line += std::string(
                    win_shown_width - win_line.length(), DEFAULT_WINDOW_CHARACTER
                    );
            }

            lines[win_coord.y + i].replace(
                win_coord.x, win_shown_width, win_line
                ); 
        }
        

        if (!is_valid_screen(lines)) {
            throw std::runtime_error("Screen::draw_window: something went " 
                                        "wrong while drawing window");
        }
        return lines;
    }

    // Returns iterator to windows_ element corresponding to win, 
    // or windows_.end(), if such was not found. 
    std::vector<Window*>::iterator Screen::find_win(Window& win)
    {
        return std::find(windows_.begin(), windows_.end(), &win);
    }

    // Returns const_iterator to windows_ element corresponding to win, 
    // or windows_.end(), if such was not found. 
    std::vector<Window*>::const_iterator Screen::find_win(const Window& win) const
    {
        return std::find(windows_.begin(), windows_.end(), &win);
    }

    // True: win is attached to screen
    // False: win is not attached to screen
    bool Screen::is_win(const Window& win) const
    {
        return find_win(win) != windows_.end();
    }

    constexpr int DEFAULT_WINDOW_X = 0;
    constexpr int DEFAULT_WINDOW_Y = 0;
    constexpr int DEFAULT_WINDOW_WIDTH = 20;
    constexpr int DEFAULT_WINDOW_HEGIHT = 20;
    Window::Window()
        :coord_{Coord(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y)}, 
        width_{DEFAULT_WINDOW_WIDTH}, height_{DEFAULT_WINDOW_HEGIHT}, 
        lines_{default_window()}
    {
        
    }

    Window::Window(Coord coord, int width, int height)
    {
        if (width < 0 || height < 0) {
            throw Window::Invalid_window{};
        }
        coord_ = coord;
        width_ = width;
        height_ = height;
        lines_ = default_window();
    }

    Window::Window(const Window& window)
        :coord_{window.coord_}, 
        width_{window.width_}, height_{window.height()}, 
        lines_{window.lines()}
    {
        
    }

    Window::Window(Window&& window)
    {
        coord_ = std::exchange(window.coord_, Coord(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y));
        width_ = std::exchange(window.width_, DEFAULT_WINDOW_WIDTH);
        height_ = std::exchange(window.height_, DEFAULT_WINDOW_HEGIHT);
        lines_ = std::exchange(window.lines_, std::vector<std::string>());
    }

    Window::~Window()
    {
        if (screen_) {
            screen_->detach(*this);
        }
    }

    Window& Window::operator=(const Window& window)
    {
        if (this == &window) {
            return *this;
        }

        coord_ = window.coord();
        width_ = window.width();
        height_ = window.height();
        lines_ = window.lines();

        return *this;
    }

    Window& Window::operator=(Window&& window)
    {
        if (this == &window) {
            return *this;
        }

        coord_ = std::exchange(window.coord_, Coord(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y));
        width_ = std::exchange(window.width_, DEFAULT_WINDOW_WIDTH);
        height_ = std::exchange(window.height_, DEFAULT_WINDOW_HEGIHT);
        lines_ = std::exchange(window.lines_, std::vector<std::string>());

        return *this;
    }

    // Sets window's lines based on text. '\n' means new line. 
    void Window::set_lines(std::string text)
    {
        lines_.clear();
        std::istringstream iss {text};
        for (std::string line; getline(iss, line); ) {
            lines_.push_back(line);
        }
        if (iss.eof() || iss.good()) {
            return;
        }
        else {
            throw std::runtime_error("Window::set_lines: could not set liens "
                                        "to text");
        }
    }

    // Do NOT use. Use Screen::attach(). 
    // Attaches screen to window. 
    // Window must be attached to screen. 
    void Window::attach(Screen* screen) 
    {
        if (screen == nullptr || !(screen->is_win(*this))) {
            throw std::runtime_error("Window::attach: window not attached to "
                                        "screen");
        }
        screen_ = screen; 
    }

    // Do NOT use. Use Screen::detach(). 
    // Detaches screen from window. 
    // Window must be detached from screen. Screen must be attached to window. 
    void Window::detach()
    {
        if (screen_ == nullptr || screen_->is_win(*this)) {
            throw std::runtime_error("Window::detach: screen not attached, "
                                        "or window still attached to screen");
        }
        screen_ = nullptr; 
    }

    std::string Window::line(int y) const
    {
        if (y < 0 || lines_.size() <= y) {
            throw std::runtime_error("Window::line: invalid line");
        }
        
        return lines_[y];
    }

    // Returns default window in vector<string> format
    std::vector<std::string> Window::default_window() const
    // Default window is DEFAULT_CHARACTER only
    { 
        std::vector<std::string> lines;
        
        for (int i = 0; i < height_; ++i) {
            lines.push_back(std::string(width_, DEFAULT_WINDOW_CHARACTER));
        }
        
        return lines;
    }
}
