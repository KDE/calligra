/***************************************************************************
                          propertyeditor.h  -  description
                             -------------------
    begin                : 08.12.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qdockwindow.h>
#include <qwidget.h>
#include <qstring.h>
#include <map>

#include "property.h"

class QWidget;
class QTable;
class QString;
class CanvasBox;
/**
  *@author Alexander Dymo
  Docked window that contains property editor - QTable with customized editors
  Unfinished
  */

class PropertyEditor : public QDockWindow  {
   Q_OBJECT
public: 
    PropertyEditor( Place p = InDock, QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    ~PropertyEditor();

public slots:
    void populateProperties(std::map<QString, PropPtr> *v_props, const CanvasBox *item); //if this should be more generic perhaps void*
    void clearProperties();

    void emitPropertyChange(QString name, QString newValue);
            
private:
    QTable *table;
    std::map<QString, PropPtr> *props;

signals:
    /** property name, new property value */
    void propertyChanged(QString name, QString newValue);


#undef signals
#define signals public
signals:
#undef signals
#define signals protected
    void createPluggedInEditor(QWidget *&retVal, PropertyEditor *editor,
	int type,QString name,QString value,const CanvasBox *cb);

};

#endif
