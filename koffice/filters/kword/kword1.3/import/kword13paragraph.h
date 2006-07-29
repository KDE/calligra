
#ifndef _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
#define _FILTER_KWORD_1_3__KWORDPARAGRAPH_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qptrlist.h>

#include "kword13layout.h"

class KWord13Format;

/**
 * A paragraph
 */
class KWord13Paragraph
{
public:
    KWord13Paragraph( void );
    ~KWord13Paragraph( void );
public:
    void xmldump( QTextStream& iostream );
    void setText( const QString& str);
    void appendText( const QString& str);
    QString text( void ) const;
public:
    /// \<LAYOUT\>
    KWord13Layout m_layout;
    /// \<FORMATS\>
    QPtrList<KWord13Format> m_formats;
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
