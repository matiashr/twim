#include "methodswidget.h"
#include "logwindow.h"
#include "main.h"
#include "editorwindow.h"
#include "datanode.h"

MethodWidget::MethodWidget(EditorWindow* parent, const TRect& bounds): 
	TWindow( bounds, "Methods", 0),
	TWindowInit( TWindow::initFrame ),
	m_parent(parent)
{
	TColorAttr attr;
	getStyle(attr);
	setStyle(attr,  slNoShadow);
	auto item1  = new DataNode("","child1" ) ;
	auto item2  = new DataNode("","child2" ) ;
	auto item3 = new DataNode("","child3",item1,  item2, false );
	auto  root = new DataNode("","Root", item3,  NULL, true);

	auto size = bounds;
	size.grow(-2,-2);
	m_tree = new TOutline( size, NULL, NULL, root );
	insert(m_tree);
}



TPalette& MethodWidget::getPalette() const
{
    static TPalette paletteGray( cpGrayWindow, sizeof( cpGrayWindow)-1 );
    return paletteGray;
}

void MethodWidget::draw() 
{
	// Draw the window without shadow
	TDrawBuffer b;
	char fillChar = ' ';
	ushort color = getColor(0x30); // Adjust color as needed
	int width = size.x;

	for (int i = 0; i < size.y; ++i) {
		b.moveChar(0, fillChar, color, width);
		writeLine(0, i, width, 1, b);
	}

	// Draw the rest of the window as usual
	TWindow::draw();
}




void MethodWidget::handleEvent(TEvent& event) 
{
	if (event.what == evKeyDown) {
		// Do something with the key event
		// For example, print the pressed key code
		int key = event.keyDown.keyCode;
		if( key == ':' ) {
			getLogWin()->message("Command Key: "+ std::to_string(key ) + "\n");
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
		} else {
			//getLogWin()->message("Key: "+ std::to_string(key ) + "\n");
		}
	}
	// Call base class handleEvent() to handle built-in behavior
	TWindow::handleEvent(event);
}
