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

class PropertyEditor;
class QString;
class CanvasBox;
class QWidget;
class Property;

class KuDesignerPlugin: public QObject {
	Q_OBJECT
public:
	KuDesignerPlugin(QObject *parent, const char* name, const QStringList& args);
	virtual ~KuDesignerPlugin();
        virtual bool acceptsDrops(){return false;}
	virtual bool dragMove(QDragMoveEvent *,CanvasBox *cb) {return false;}
	virtual void newCanvasBox(int type, CanvasBox *cb){;}
public slots:
    virtual void createPluggedInEditor(QWidget *& retVal, PropertyEditor *editor,
        Property *property,CanvasBox *);
};

#endif
