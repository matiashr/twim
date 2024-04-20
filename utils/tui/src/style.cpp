class TFrame
{
public:

    static char frameChars[33];
    static const char * closeIcon;
    static const char * zoomIcon;
    static const char * unZoomIcon;
};

void useCustomSkin()
{
    TFrame::frameChars[19]  = '\xC0';
    TFrame::frameChars[21]  = '\xB3';
    TFrame::frameChars[22]  = '\xDA';
    TFrame::frameChars[23]  = '\xC3';
    TFrame::frameChars[25]  = '\xD9';
    TFrame::frameChars[26]  = '\xC4';
    TFrame::frameChars[27]  = '\xC1';
    TFrame::frameChars[28]  = '\xBF';
    TFrame::frameChars[29]  = '\xB4';
    TFrame::frameChars[30]  = '\xC2';
    TFrame::closeIcon       = "\xB4~X~\xC3";
    TFrame::zoomIcon        = "\xB4~\x18~\xC3";
    TFrame::unZoomIcon      = "\xB4~\x12~\xC3";
}
