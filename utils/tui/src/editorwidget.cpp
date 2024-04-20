#include "editorwidget.h"
#include "logwindow.h"
#include "main.h"
#include "editorwindow.h"

EditorWidget::EditorWidget(EditorWindow* parent, const TRect& bounds): 
	TEditor(bounds,NULL, NULL, NULL,8000 ),
	m_parent(parent)
{
	//auto sb = new TScrollBar(TRect(bounds.b.x - 1, bounds.a.y, bounds.b.x, bounds.b.y));
	TColorAttr attr;
	getStyle(attr);
	setStyle(attr,  slNoShadow);
}


void EditorWidget::draw() 
{
        TDrawBuffer b;
        ushort color = getColor(0x30); // Adjust color as needed
        ushort lineNumberColor = getColor(0x17); // Adjust color as needed
        ushort fillChar = ' ';
        int width = size.x;
        int lineCount = 10; 
        for (int i = 0; i < size.y; ++i) {
            b.moveChar(0, fillChar, color, width);
            writeLine(0, i, width, 1, b);
        }
        // Draw the rest of the editor
        TEditor::draw();
}


void EditorWidget::updateMarginWidth() noexcept
{
        TRect mr = getBounds();
        mr.b.x = mr.a.x -10 ;
}

void EditorWidget::handleEvent(TEvent& event) 
{
	if (event.what == evKeyDown) {
		// Do something with the key event
		// For example, print the pressed key code
		int key = event.keyDown.keyCode;
		getLogWin()->message("Key: "+ std::to_string(key ) + "\n");
		if( key == ':' ) {
			getLogWin()->message("Command Key: "+ std::to_string(key ) + "\n");
			char buffer[81];
			memset(buffer,0, sizeof(buffer));
			if( inputBox( "cmd", ":", buffer, sizeof(buffer)-1) != cmCancel ) {
				getLogWin()->message("execute:"+ std::string(buffer) + "\n");
			} else {
				getLogWin()->message("escape:");
			}
			return;
		} else if ( key == 18 ) {
			getLogWin()->message("Switch view\n");
			m_parent->focus(0);
			return;
		} else if ( key == 283) {
			getLogWin()->message("Switch view2\n");
			m_parent->focus(1);
			return;
		} else if ( key == 49 ) {
			getLogWin()->message("Switch view3\n");
			m_parent->focus(2);
			return;
		}


	}
	// Call base class handleEvent() to handle built-in behavior
	TEditor::handleEvent(event);

}
