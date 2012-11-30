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
class MainDocument;

class KPLATO_EXPORT Part : public KoPart
{
    Q_OBJECT

public:
    Part(QObject *parent);

    virtual ~Part();

    void setDocument(KPlato::MainDocument *document);

    /// reimplemented
    virtual KoView *createViewInstance(QWidget *parent);

protected:
    virtual void openTemplate( const KUrl& url );

    KPlato::MainDocument *m_document;
};

}  //KPlato namespace
#endif
