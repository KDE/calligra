
class QIODevice;

#include <qstring.h>
#include <qmap.h>
#include <qptrlist.h>

#include "kwordframeset.h"

class KWordDocument
{
public:
    KWordDocument( void );
    ~KWordDocument( void );
    
public:
    void xmldump( QIODevice* io );

public:
    QMap<QString, QString> m_documentProperties;
    QValueList<KWord13Layout> m_styles;
    QPtrList<KWordTextFrameset> m_normalTextFramesetList; ///< List of \<FRAMESET\> having normal text
    QPtrList<KWordTextFrameset> m_headerFooterFramesetList; ///< List of \<FRAMESET\> having footer/header
    QPtrList<KWordTextFrameset> m_footEndNoteFramesetList; ///< List of \<FRAMESET\> having footnotes or endnotes
    QPtrList<KWordFrameset> m_otherFramesetList; ///< List of \<FRAMESET\> of other types
};
