#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <qcanvas.h>
#include "canvbox.h"

class MyCanvas: public QCanvas{
public:
    MyCanvas(int w, int h): QCanvas(w, h) 
    {
	templ = 0;
    }
    ~MyCanvas()
    {
	delete templ;
/*	QCanvasItemList l = allItems();
	for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
	{
	    delete (*it);
	}*/
    }
    
    CanvasKugarTemplate *templ;
private:
    void scaleCanvas(int scale);
};

#endif
