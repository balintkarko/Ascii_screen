Ascii_screen

USAGE EXAMPLE:

#include "ascii_screen.hpp"

int main()
{
    constexpr int SCREEN_WIDTH = 100;
    constexpr int SCREEN_HEIGHT = 40;
    constexpr int WINDOW_X_COORDINATE = 5;
    constexpr int WINDOW_Y_COORDINATE = 10;
    constexpr int WINDOW_WIDTH = 20;
    constexpr int WINDOW_HEIGHT = 10;
    std::vector<std::string> v { "overwritten", "by string", "anyway... " };
    std::string s { 
        "+------------------+\n"
        "01234567890123456789\n"
        "    THIS IS THE\n" 
        "   WINDOW EXAMPLE!!\n"
        "see how long string\n"
        "is sliced: \n"
        "he1-he2-he3-he4-he5-he6-he7-he8-he9-he10\n"
        "it was 10 he-s. \n"
        "+------------------+\n"
        };


    // Create screen
    Ascii_screen::Screen screen(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create window
    // X: int, character's index from left
    // Y: int, character's index from top
    // top left is (0:0)
    Ascii_screen::Window window(
        Ascii_screen::Coord(WINDOW_X_COORDINATE, WINDOW_Y_COORDINATE), 
        WINDOW_WIDTH, WINDOW_HEIGHT
        );

    // Attach window to screen
    screen.attach(window);
    // Do NOT attach Screen to Window

    // Modify window contents. Be ware of multiline and wide characters
    // Line contents and lines not fitting to window are sliced
    window.set_lines(v /*VECTOR<STRING>*/); // Consecutive elements are consecutive lines 
    window.set_lines(s /*STRING*/); // Use '\n' for new line

    // Print cleared and refreshed screen 
    screen.update();

    screen.clear(); // When needed. 
    screen.detach(window); // When needed. Can be attached again. 

    return 0;
}

// Background changed from ' ' to '.' for the sake of example: 
![ascii_screen_example](https://github.com/balintkarko/Ascii_screen/assets/124880530/f05a8b4f-a85f-43d6-860d-29c93b953343)

// End of example



// Header file must be compiled because of inlined functions 
g++ -o main.exe main.cpp ascii_screen.cpp ascii_screen.hpp
main.exe
