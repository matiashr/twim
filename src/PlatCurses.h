// Copyright 2012-2024 Mitchell. See LICENSE.
// Scintilla platform for a curses (terminal) environment.

#ifndef PLAT_CURSES_H
#define PLAT_CURSES_H
#include <curses.h>

#define IMAGE_MAX 31
namespace Scintilla::Internal 
{
	class FontImpl : public Font {
		public:
			/**
			 * Sets terminal character attributes for a particular font.
			 * These attributes are a union of curses attributes and stored in the font's `attrs` member.
			 * The curses attributes are not constructed from various fields in *fp* since there is no
			 * `underline` parameter. Instead, you need to manually set the `weight` parameter to be the
			 * union of your desired attributes. Scintilla's lexers/LexLPeg.cxx has an example of this.
			 */
			FontImpl(const FontParameters &fp);
			virtual ~FontImpl() noexcept  = default;

			attr_t attrs = 0;
	};

	class SurfaceImpl : public Surface {
		WINDOW *win = nullptr; // curses window to draw on
		PRectangle clip;
		ColourRGBA pixmapColor;

		public:
		SurfaceImpl() = default;
		~SurfaceImpl() noexcept ;

		void Init(WindowID wid) ;
		void Init(SurfaceID sid, WindowID wid) ;
		std::unique_ptr<Surface> AllocatePixMap(int width, int height) ;

		void SetMode(SurfaceMode mode) ;

		void Release() noexcept ;
		int SupportsFeature(Supports feature) noexcept ;
		bool Initialised() ;
		int LogPixelsY() ;
		int PixelDivisions() ;
		int DeviceHeightFont(int points) ;
		void LineDraw(Point start, Point end, Stroke stroke) ;
		void PolyLine(const Point *pts, size_t npts, Stroke stroke) ;
		void Polygon(const Point *pts, size_t npts, FillStroke fillStroke) ;
		void RectangleDraw(PRectangle rc, FillStroke fillStroke) ;
		void RectangleFrame(PRectangle rc, Stroke stroke) ;
		void FillRectangle(PRectangle rc, Fill fill) ;
		void FillRectangleAligned(PRectangle rc, Fill fill) ;
		void FillRectangle(PRectangle rc, Surface &surfacePattern) ;
		void RoundedRectangle(PRectangle rc, FillStroke fillStroke) ;
		void AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) ;
		void GradientRectangle(
				PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) ;
		void DrawRGBAImage(
				PRectangle rc, int width, int height, const unsigned char *pixelsImage) ;
		void Ellipse(PRectangle rc, FillStroke fillStroke) ;
		void Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) ;
		void Copy(PRectangle rc, Point from, Surface &surfaceSource) ;

		std::unique_ptr<IScreenLineLayout> Layout(const IScreenLine *screenLine) ;

		void DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
				ColourRGBA fore, ColourRGBA back) ;
		void DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
				ColourRGBA fore, ColourRGBA back) ;
		void DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase,
				std::string_view text, ColourRGBA fore) ;
		void MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) ;
		XYPOSITION WidthText(const Font *font_, std::string_view text) ;

		void DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
				ColourRGBA fore, ColourRGBA back) ;
		void DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase,
				std::string_view text, ColourRGBA fore, ColourRGBA back) ;
		void DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase,
				std::string_view text, ColourRGBA fore) ;
		void MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) ;
		XYPOSITION WidthTextUTF8(const Font *font_, std::string_view text) ;

		XYPOSITION Ascent(const Font *font_) ;
		XYPOSITION Descent(const Font *font_) ;
		XYPOSITION InternalLeading(const Font *font_) ;
		XYPOSITION Height(const Font *font_) ;
		XYPOSITION AverageCharWidth(const Font *font_) ;

		void SetClip(PRectangle rc) ;
		void PopClip() ;
		void FlushCachedState() ;
		void FlushDrawing() ;

		// Custom drawing functions for Curses.
		void DrawLineMarker(
				const PRectangle &rcWhole, const Font *fontForCharacter, int tFold, const void *data);
		void DrawWrapMarker(PRectangle rcPlace, bool isEndMarker, ColourRGBA wrapColour);
		void DrawTabArrow(PRectangle rcTab, const ViewStyle &vsDraw);

		bool isCallTip = false;
	};

	class ListBoxImpl : public ListBox {
		int height = 5, width = 10;
		std::vector<std::string> list;
		char types[IMAGE_MAX + 1][5]; // UTF-8 character plus terminating '\0' instead of an image
		int selection = 0;

		public:
		IListBoxDelegate *delegate = nullptr;

		ListBoxImpl();
		~ListBoxImpl()  = default;

		void SetFont(const Font *font) ;
		void Create(Window &parent, int ctrlID, Point location_, int lineHeight_, bool unicodeMode_,
				Technology technology_) ;
		void SetAverageCharWidth(int width) ;
		void SetVisibleRows(int rows) ;
		int GetVisibleRows() const ;
		PRectangle GetDesiredRect() ;
		int CaretFromEdge() ;
		void Clear() noexcept ;
		void Append(char *s, int type) ;
		int Length() ;
		void Select(int n) ;
		int GetSelection() ;
		int Find(const char *prefix) ;
		std::string GetValue(int n) ;
		void RegisterImage(int type, const char *xpm_data) ;
		void RegisterRGBAImage(
				int type, int width, int height, const unsigned char *pixelsImage) ;
		void ClearRegisteredImages() ;
		void SetDelegate(IListBoxDelegate *lbDelegate) ;
		void SetList(const char *listText, char separator, char typesep) ;
		void SetOptions(ListOptions options_) ;
	};
	void init_colors();
	short term_color(ColourRGBA color);
	short term_color(short color);
};

/**
 * Returns the given Scintilla `WindowID` as a curses `WINDOW`.
 * @param w A Scintilla `WindowID`.
 * @return curses `WINDOW`.
 */
#define _WINDOW(w) reinterpret_cast<WINDOW *>(w)

/**
 * Returns the curses `COLOR_PAIR` for the given curses foreground and background `COLOR`s.
 * This is used simply to enumerate every possible color combination.
 * Note: only 256 combinations are possible due to curses portability.
 * Note: This references the global curses variable `COLORS` and is not a constant expression.
 * @param f The curses foreground `COLOR`.
 * @param b The curses background `COLOR`.
 * @return int number for defining a curses `COLOR_PAIR`.
 */
#define SCI_COLOR_PAIR(f, b) ((b) * ((COLORS < 16) ? 8 : 16) + (f) + 1)

/**
 * Returns a curses color pair from the given fore and back colors.
 * @param f Foreground color, either a Scintilla color or curses color.
 * @param b Background color, either a Scintilla color or curses color.
 * @return curses color pair suitable for calling `COLOR_PAIR()` with.
 */
#define term_color_pair(f, b) SCI_COLOR_PAIR(term_color(f), term_color(b))

#endif
