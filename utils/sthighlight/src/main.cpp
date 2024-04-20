#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Scintilla.h>
#include <SciLexer.h>

// Define a default C code to display in the Scintilla editor
#define DEFAULT_TEXT \
    "#include <stdio.h>\n\nint main() {\n    printf(\"Hello, world!\\n\");\n    return 0;\n}"

int main() {
    // Initialize Scintilla
    Scintilla_LinkLexers();

    // Create a Scintilla editor
    sptr_t hwnd = ScintillaNew(0);

    // Set default text
    Scintilla_SendMessage(hwnd, SCI_SETTEXT, 0, (sptr_t)DEFAULT_TEXT);

    // Configure lexer for C syntax highlighting
    Scintilla_SendMessage(hwnd, SCI_SETLEXER, SCLEX_CPP, 0);

    // Configure other properties (optional)
    Scintilla_SendMessage(hwnd, SCI_SETTABWIDTH, 4, 0);
    Scintilla_SendMessage(hwnd, SCI_SETINDENTATIONGUIDES, SC_IV_REAL, 0);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, STYLE_DEFAULT, 0x000000);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_COMMENT, 0x007F00);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_COMMENTLINE, 0x007F00);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_COMMENTDOC, 0x007F00);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_NUMBER, 0x007F7F);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_WORD, 0x00007F);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_STRING, 0x7F007F);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_CHARACTER, 0x7F007F);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_OPERATOR, 0x00007F);
    Scintilla_SendMessage(hwnd, SCI_STYLESETFORE, SCE_C_PREPROCESSOR, 0x7F7F7F);

    // Start Scintilla message loop
    while (1) {
        Scintilla_MessageLoop();
    }

    return 0;
}

