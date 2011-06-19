
#include <qpalette.h>
 #include "highlighter.h"
Highlighter::Highlighter(QTextDocument *document) //Constructor for HTML Syntax Highlighter
    : QSyntaxHighlighter(document)
{
    QPalette palette;
    QColor foreground = palette.text().color();
    QColor background = palette.base().color();

    QTextCharFormat entityFormat;
    QColor entityColor;
    entityColor.setRed((foreground.red() + background.red()) / 2);
    entityColor.setGreen(
            (foreground.green() + background.green()) / 2);
    entityColor.setBlue(
            (foreground.blue() + background.blue()) / 2);
    if (abs(entityColor.red() - background.red()) > 80)
        entityColor.setRed(foreground.red());
    else if (abs(entityColor.green()-background.green()) > 80)
        entityColor.setGreen(foreground.green());
    else entityColor.setBlue(foreground.blue());
    entityFormat.setForeground(entityColor);
    entityFormat.setFontWeight(QFont::Normal);
    setFormatFor(Entity, entityFormat);

    QTextCharFormat tagFormat;
    QColor tagColor;
    tagColor.setRed(foreground.red() + (background.red() - foreground.red()) / 4);
    tagColor.setGreen(
            foreground.green() + (background.green() - foreground.green()) / 4);
    tagColor.setBlue(
            foreground.blue() + (background.blue() - foreground.blue()) / 4);
    tagFormat.setForeground(tagColor);
    tagFormat.setFontWeight(QFont::Bold);
    setFormatFor(Tag, tagFormat);

    QTextCharFormat commentFormat;
    QColor commentColor;
    commentColor.setRed(background.red() + (foreground.red() - background.red()) / 3);
    commentColor.setGreen(
            background.green() + (foreground.green() - background.green()) / 3);
    commentColor.setBlue(
            background.blue() + (foreground.blue() - background.blue()) / 3);
    commentFormat.setForeground(commentColor);
    commentFormat.setFontWeight(QFont::Normal);
    setFormatFor(Comment, commentFormat);

    QTextCharFormat attributeFormat;
    QColor attributeColor;
    attributeColor.setRed((foreground.red() + background.red()) / 2);
    attributeColor.setGreen(
            (foreground.green() + background.green()) / 2);
    attributeColor.setBlue(
            (foreground.blue() + background.blue()) / 2);
    if (abs(attributeColor.red() - background.red()) > 80)
        attributeColor.setRed(background.red());
    else if (abs(attributeColor.green() - background.green()) > 80)
        attributeColor.setGreen(background.green());
    else attributeColor.setBlue(background.blue());
    attributeFormat.setForeground(attributeColor);
    attributeFormat.setFontWeight(QFont::Normal);
    setFormatFor(Attribute, attributeFormat);
}

void Highlighter::setFormatFor(Construct construct, //Sets format for construct to &format
                         QTextCharFormat &format)
{
    formats[construct] = format;
}

QTextCharFormat Highlighter::getFormatFor(Construct construct) //Gets format for construct - returns QTextCharFormat
{
    return formats[construct];
}

void Highlighter::highlightBlock(const QString &text) //Highlights block of text
{
    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;
    QRegExp regex;

    while (pos >= 0 && pos < len && len > 0) {
        switch (state) {
        default:
        case Normal:
            regex.setPattern(QLatin1String("[<&]"));
            pos = regex.indexIn(text, pos);
            if (pos >= 0) {
                if (text.at(pos) == QLatin1Char('<')) {
                    start = pos;
                    if (text.mid(pos, 4) == QLatin1String("<!--"))
                        state = InComment;
                    else
                        state = InTag;
                    ++pos;
                } else if (text.at(pos) == QLatin1Char('&')) {
                    regex.setPattern(QLatin1String("&[a-zA-Z0-9]+;"));
                    if (regex.indexIn(text, pos) == pos)
                        setFormat(pos, regex.matchedLength(), formats[Entity]);
                    ++pos;
                }
            }
            break;
        case InComment:
            regex.setPattern(QLatin1String("-->"));
            pos = regex.indexIn(text, pos);
            if (pos >= 0) {
                state = Normal;
                pos += 3;
                setFormat(start, pos - start, formats[Comment]);
                ++pos;
            } else
                setFormat(start, len - start, formats[Comment]);
            break;
         case InTag:
            regex.setPattern(QLatin1String("[>\"]"));
            pos = regex.indexIn(text, pos);
            if (pos >= 0) {
                if (text.at(pos) == QLatin1Char('>')) {
                    state = Normal;
                    ++pos;
                    setFormat(start, pos - start, formats[Tag]);
                } else if (text.at(pos) == QLatin1Char('"')) {
                    setFormat(start, pos - start, formats[Tag]);
                    start = pos;
                    state = InAttribute;
                    ++pos;
                }
            } else
                setFormat(start, len-start, formats[Tag]);
            break;
        case InAttribute:
            regex.setPattern(QLatin1String("\""));
            pos = regex.indexIn(text, pos);
            if (pos >= 0) {
                setFormat(start, pos - start, formats[Attribute]);
                state = InTag;
                start = ++pos;
            } else
                setFormat(start, len - start, formats[Attribute]);
            break;
        }
    }
    setCurrentBlockState(state);
}
