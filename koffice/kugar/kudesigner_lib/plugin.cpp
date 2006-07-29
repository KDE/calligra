/***************************************************************************
                       plugin.h  -  description
                          -------------------
 begin                : 19.01.2003
 copyright            : (C) 2003 Joseph Wenninger
 email                : jowenn@kde.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "plugin.h"

KuDesignerPlugin::KuDesignerPlugin( QObject *parent, const char *name, const QStringList& ) : QObject( parent, name )
{}
KuDesignerPlugin::~KuDesignerPlugin()
{}
void KuDesignerPlugin::createPluggedInEditor( QWidget *&/*retVal*/, Editor */*editor*/,
        Property */*property*/, Box */*cb*/ )
{}

#include "plugin.moc"
