// g++ ex.cpp -I include/ -I../scintilla/include/ -lncurses
#include <ncurses.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "Scintilla.h"
#if defined(__cplusplus)
#include "ILexer.h"
#endif

#include "Lexilla.h"

#if defined(__cplusplus)
using namespace Lexilla;
#endif

// Custom window class for Lexilla control
class LexillaWindow {
public:
    LexillaWindow(int height, int width) {
        // Initialize ncurses
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);

        // Create Lexilla window
        lexillaWin = newwin(height, width, 0, 0);
	int n=GetLexerCount();
/*
        // Initialize Lexilla control
        lexilla_send_message(LEXILLA(lexillaWin), SCI_STYLESETFORE, STYLE_DEFAULT, 0xFFFFFF); // Set default text color to white
        lexilla_send_message(LEXILLA(lexillaWin), SCI_STYLESETBACK, STYLE_DEFAULT, 0x000000); // Set default background color to black
        lexilla_send_message(LEXILLA(lexillaWin), SCI_SETLEXER, SCLEX_CPP, 0); // Set lexer to C++
        lexilla_send_message(LEXILLA(lexillaWin), SCI_SETKEYWORDS, 0, (sptr_t)"int char float double"); // Set some example keywords
        lexilla_send_message(LEXILLA(lexillaWin), SCI_SETSTYLEBITS, 7, 0); // Set number of style bits to 7

        // Set Lexilla font
        lexilla_send_message(LEXILLA(lexillaWin), SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)"Monospace");

        // Load C file content into Lexilla control (replace this with your own file loading logic)
        std::ifstream file("example.c");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            lexilla_send_message(LEXILLA(lexillaWin), SCI_SETTEXT, 0, (sptr_t)content.c_str());
            file.close();
        }
*/
    }

    ~LexillaWindow() {
        // Cleanup ncurses
        endwin();
    }

    WINDOW* lexillaWin;
};

int main() {
    // Get terminal dimensions
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Create Lexilla window
    LexillaWindow lexillaWindow(w.ws_row, w.ws_col);

    // Main loop
    while (true) {
        // Handle user input
        int ch = getch();
        if (ch == 'q')
            break; // Quit on 'q'

        // Redraw
        wrefresh(lexillaWindow.lexillaWin);
    }

    return 0;
}

