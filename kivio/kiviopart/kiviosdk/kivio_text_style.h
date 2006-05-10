#ifndef KIVIO_TEXT_STYLE_H
#define KIVIO_TEXT_STYLE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QFont>

class QDomDocument;
class QPainter;


class KivioTextStyle
{
protected:
    QString m_text;                                 // The text inside this shape
    QColor m_color;                                 // The text color
    QFont m_font;                                   // The text font
    bool m_isHtml;                                  // Is the text HTML formatted?
    int m_hTextAlign, m_vTextAlign;                 // Horizontal and vertical text alignment flags

public:
    KivioTextStyle();
    virtual ~KivioTextStyle();

    QDomElement saveXML( QDomDocument & );
    bool loadXML( const QDomElement & );

    void copyInto( KivioTextStyle * );




    inline QString text() { return m_text; }
    inline void setText( QString s ) { m_text=s; }

    inline QColor color() { return m_color; }
    inline void setColor( QColor c ) { m_color=c; }

    inline QFont font() { return m_font; }
    inline void setFont( QFont f ) { m_font=f; }

    inline bool isHtml() { return m_isHtml; }
    inline void setIsHtml( bool b ) { m_isHtml=b; }

    inline int hTextAlign() { return m_hTextAlign; }
    inline void setHTextAlign(int i) { m_hTextAlign=i; }

    inline int vTextAlign() { return m_vTextAlign; }
    inline void setVTextAlign(int i) { m_vTextAlign=i; }

};

#endif

