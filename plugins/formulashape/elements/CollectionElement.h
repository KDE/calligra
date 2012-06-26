#ifndef COLLECTIONELEMENT_H
#define COLLECTIONELEMENT_H

#include "TokenElement.h"
#include "kformula_export.h"
#include <QStringList>



class KOFORMULA_EXPORT CollectionElement : public TokenElement {
public:
    /// The standard constructor
    CollectionElement( BasicElement* parent = 0 );

    bool addCharacter(const QString);

    QString group();

    QRectF renderToPath( const QString& rawString, QPainterPath& path ) const;


 private :

 QString m_group;

};



#endif // COLLECTIONELEMENT_H
