
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
    QMap<QString,QString> m_documentProperties;
    QPtrList<KWordNormalTextFrameset> m_normalTextFramesetList; ///< List of \<FRAMESET\> having normal text
};
