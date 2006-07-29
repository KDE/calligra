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

#ifndef _KUGAR_DESIGNER_PLUGIN_H_
#define _KUGAR_DESIGNER_PLUGIN_H_

#include <qobject.h>
#include <koproperty/property.h>
#include <koproperty/editor.h>
#include <box.h>

class QString;
class QWidget;
class CanvasReportItem;
class KoStore;

using namespace KoProperty;
using namespace Kudesigner;

class KuDesignerPlugin: public QObject
{
    Q_OBJECT
public:
    KuDesignerPlugin( QObject *parent, const char* name, const QStringList& args );
    virtual ~KuDesignerPlugin();
    virtual bool acceptsDrops()
    {
        return false;
    }
    virtual bool dragMove( QDragMoveEvent *, Box */*cb*/ )
    {
        return false;
    }
    virtual void newCanvasBox( int /*type*/, Box */*cb*/ )
    {
        ;
    }
    virtual void modifyItemPropertyOnSave( CanvasReportItem */*item*/, const Property & /*p*/ , QString &/*propertyName*/, QString &/*propertyValue*/ )
    {
        ;
    }
    virtual void modifyItemPropertyOnLoad( CanvasReportItem */*item*/, const Property & /*p*/, QString &/*propertyName*/, QString &/*propertyValue*/ )
    {
        ;
    }
    virtual bool store ( KoStore* )
    {
        return true;
    }
    virtual bool load ( KoStore* )
    {
        return true;
    }

public slots:
    virtual void createPluggedInEditor( QWidget *& retVal, Editor *editor,
                                        Property *property, Box * );
};

#endif
