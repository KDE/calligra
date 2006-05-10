//Added by qt3to4:
#include <QTextStream>

#ifndef _KWORD13LAYOUT
#define _KWORD13LAYOUT

class QTextStream;

#include <QString>
#include <QMap>

#include "kword13formatone.h"

/**
 * Layout or style
 */
class KWord13Layout
{
public:
    KWord13Layout( void );
    ~KWord13Layout( void );
    
public:
    void xmldump( QTextStream& iostream );
    
    /**
     * @brief Get a key representating the properties
     *
     * This key helps to categorize the automatic styles
     */
    QString key( void ) const;
    
    
    QString getProperty( const QString& name ) const;

public:
    KWord13FormatOneData m_format; ///< Character format properties
    QMap<QString,QString> m_layoutProperties;
    bool m_outline;
    QString m_name; ///< Name of the style (either the used one or the one currently being defined)
public: // OASIS-specific
    QString m_autoStyleName; ///< Name of the OASIS automatic style
};

#endif // _KWORD13LAYOUT
