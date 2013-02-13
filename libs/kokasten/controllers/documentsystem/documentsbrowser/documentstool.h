/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DOCUMENTSTOOL_H
#define DOCUMENTSTOOL_H

// lib
#include "kastencontrollers_export.h"
// Kasten core
#include <abstracttool.h>

namespace Kasten2
{

class DocumentManager;
class AbstractDocument;


class KASTENCONTROLLERS_EXPORT DocumentsTool : public AbstractTool
{
  Q_OBJECT

  public:
    explicit DocumentsTool( DocumentManager* documentManager );
    virtual ~DocumentsTool();

  public:
    AbstractDocument* focussedDocument() const;
    QList<AbstractDocument*> documents() const;

  public:
    void setFocussedDocument( AbstractDocument* document );

  public: // AbstractTool API
    virtual QString title() const;
    virtual void setTargetModel( AbstractModel* model );

  Q_SIGNALS:
    void documentsAdded( const QList<Kasten2::AbstractDocument*>& documents );
    void documentsClosing( const QList<Kasten2::AbstractDocument*>& documents );
    void focussedDocumentChanged( Kasten2::AbstractDocument* document );

  private: // sources
    DocumentManager* mDocumentManager;

    AbstractDocument* mFocussedDocument;
};


inline AbstractDocument* DocumentsTool::focussedDocument() const { return mFocussedDocument; }

}

#endif
