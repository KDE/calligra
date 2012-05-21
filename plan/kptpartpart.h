/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPLATO_PARTPART_H
#define KPLATO_PARTPART_H

#include <KoPart.h>

#include "kplato_export.h"

class KoView;
class QWidget;

/// The main namespace.
namespace KPlato
{
class Part;

class KPLATO_EXPORT PartPart : public KoPart
{
    Q_OBJECT

public:
    PartPart(QObject *parent);

    virtual ~PartPart();

    void setDocument(KPlato::Part *document);

    /// reimplemented
    virtual KoView *createViewInstance(QWidget *parent);

protected:
    virtual void openTemplate( const KUrl& url );

    KPlato::Part *m_document;
};

}  //KPlato namespace
#endif
