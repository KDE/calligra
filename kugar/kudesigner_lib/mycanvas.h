/***************************************************************************
                          mycanvas.h  -  description
                             -------------------
    begin                : 07.06.2002
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
#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <qcanvas.h>

class CanvasBox;
class CanvasKugarTemplate;
class KuDesignerPlugin;

class MyCanvas: public QCanvas{
public:
    MyCanvas(int w, int h);
    ~MyCanvas();

//    KudesignerDoc *document(){return m_doc;}
    CanvasKugarTemplate *templ;
    QPtrList<CanvasBox> selected;

    KuDesignerPlugin *plugin();
    void setPlugin(KuDesignerPlugin *plugin);

protected:
   virtual void drawForeground ( QPainter & painter, const QRect & clip );

private:
//    KudesignerDoc *m_doc;
    void scaleCanvas(int scale);
    KuDesignerPlugin *m_plugin;
};

#endif
