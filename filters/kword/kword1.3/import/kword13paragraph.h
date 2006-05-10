
#ifndef _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
#define _FILTER_KWORD_1_3__KWORDPARAGRAPH_H

#include <QString>
#include <q3valuelist.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QTextStream>

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
    Q3PtrList<KWord13Format> m_formats;
private:
    QString m_text; ///< Text
};

/**
 * Group of paragraphs
 * (Mostly all paragraphs of a text frameset)
 */
class KWord13ParagraphGroup : public Q3ValueList <KWord13Paragraph>
{
public:
    void xmldump( QTextStream& iostream );
};

#endif //  _FILTER_KWORD_1_3__KWORDPARAGRAPH_H
