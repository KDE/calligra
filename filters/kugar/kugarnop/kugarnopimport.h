/***************************************************************************
                          kugarnopimport.h  -  description
                             -------------------
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License verstion 2 as    *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#ifndef __KUGARNOPIMPORT_H__
#define __KUGARNOPIMPORT_H__

#include <KoFilter.h>
#include <q3cstring.h>
#include <qstringlist.h>

class KugarNOPImport : public KoFilter
{
    Q_OBJECT

public:
    KugarNOPImport( QObject* parent, const QStringList& );

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

};

#endif
