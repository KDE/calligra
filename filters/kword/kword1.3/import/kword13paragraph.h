
#ifndef _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
#define _FILTER_KWORD_1_3__KWORDPARAGRAPH_H

#include <qstring.h>
#include <qvaluelist.h>

/**
 * A paragraph
 */
class KWordParagraph
{
public:
    void xmldump( QTextStream& iostream );
    void setText( const QString& str);
    void appendText( const QString& str);
    QString text( void ) const;
private:
    QString m_text; ///< Text
};

/**
 * Group of paragraphs
 * (Mostly all paragraphs of a text frameset)
 */
class KWordParagraphGroup : public QValueList <KWordParagraph>
{
public:
    void xmldump( QTextStream& iostream );
};

#endif //  _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
