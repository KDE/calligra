/***************************************************************************
                          kexi_factory.h  -  description
                             -------------------
    begin                : Sun Nov  17 23:30:00 CET 2002
    copyright            : (C) 2002 Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXI_FACTORY_H
#define KEXI_FACTORY_H

#include <koFactory.h>

class KInstance;
class KAboutData;

class KexiFactory : public KoFactory
{
    Q_OBJECT
public:
    KexiFactory( QObject* parent = 0, const char* name = 0 );
    ~KexiFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, const char *widgetName = 0,
		QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument",
		const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
    static KAboutData* s_aboutData;
};

#endif
