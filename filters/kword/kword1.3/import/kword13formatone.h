
#ifndef KWORD_1_3_FORMAT_ONE
#define KWORD_1_3_FORMAT_ONE

class QTextStream;

#include <qstring.h>
#include <qmap.h>

/**
 * Contains the data of the \<FORMAT id="1"\> children
 * also the grand-children of \<STYLE\> and \<LAYOUT\>
 */
class KWord13FormatOne
{
public:
    KWord13FormatOne( void );
    ~KWord13FormatOne( void );
    
public:
    void xmldump( QTextStream& iostream );
    
    /**
     * @brief Get a key representating the properties
     *
     * This key helps to categorize the automatic styles
     */
    QString key( void ) const;

public:
    QMap<QString,QString> m_properties;
};

#endif // KWORD_1_3_FORMAT_ONE
