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
#include <qcstring.h>
#include <qstringlist.h>

class KugarNOPImport : public KoFilter
{
    Q_OBJECT

public:
    KugarNOPImport( KoFilter *parent, const char *name, const QStringList& );

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

};

#endif
