/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOWORK_PACKAGESETTINGS_H
#define KPLATOWORK_PACKAGESETTINGS_H

#include "kplatowork_export.h"
#include "ui_packagesettings.h"

#include "workpackage.h"

#include <KDialog>
#include <klocale.h>
#include <kdebug.h>

class QUndoCommand;

namespace KPlatoWork
{

class PackageSettingsPanel;

class KPLATOWORK_EXPORT PackageSettingsDialog : public KDialog
{
    Q_OBJECT
public:
    explicit PackageSettingsDialog( WorkPackage &p, QWidget *parent=0 );

    QUndoCommand *buildCommand();

private:
    PackageSettingsPanel *dia;
};

class PackageSettingsPanel : public QWidget, public Ui_PackageSettings
{
    Q_OBJECT
public:
    explicit PackageSettingsPanel( WorkPackage &p, QWidget *parent=0 );

    QUndoCommand *buildCommand();
    WorkPackageSettings settings() const;
    void setSettings( const WorkPackageSettings &settings );

signals:
    void changed( bool );

public slots:
    void slotChanged();

protected:
    WorkPackage &m_package;
};

}  //KPlatoWork namespace


#endif
