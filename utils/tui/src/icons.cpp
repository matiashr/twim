#include "icons.h"

// https://emojiguide.org/search?q=browse
static const std::string symbols[LAST_ENTRY] =
{
	"",
	"📂",
	"🗂️",
	"🗒️",
	"🗄️",
	"🗃️",
	"😀",
	"💾",
	"🔌",
	"🖥️",
	"📡",
	"🧬",
	"🔍",
	"🛑",
	"♏"
};

static const std::string texts[LAST_ENTRY] =
{
	"",
	"",
	"",
	"[]",
	"{}",
	"_",
	"(*)",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

static bool useGraphics=false;
void enablegraphics(bool s) { 
	printf("Graphics:%d\n", s );
	useGraphics=s; 
}

std::string getIcon( symbol_t t ){
	if( useGraphics) {
		return symbols[t];
	} else {
		return texts[t];
	}
}
