
#ifndef _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
#define _FILTER_KWORD_1_3__KWORDPARAGRAPH_H

#include <qstring.h>
#include <qvaluelist.h>

#include "kword13layout.h"

/**
 * A paragraph
 */
class KWord13Paragraph
{
public:
    void xmldump( QTextStream& iostream );
    void setText( const QString& str);
    void appendText( const QString& str);
    QString text( void ) const;
public:
    KWord13Layout m_layout;
private:
    QString m_text; ///< Text
};

/**
 * Group of paragraphs
 * (Mostly all paragraphs of a text frameset)
 */
class KWord13ParagraphGroup : public QValueList <KWord13Paragraph>
{
public:
    void xmldump( QTextStream& iostream );
};

#endif //  _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
