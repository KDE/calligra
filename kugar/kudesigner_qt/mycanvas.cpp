#include <qstringlist.h>
#include "mycanvas.h"
#include "canvbox.h"

void MyCanvas::scaleCanvas(int scale)
{
    resize(width()*scale, height()*scale);
    
    //scale all child items if they are textbox's
    QCanvasItemList l=this->allItems();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
	if ((*it)->rtti() == RttiTextBox)
	{
/*	    CanvasTextBox* b = (CanvasTextBox*)(*it);
	    b->scale(scale);*/
	}
    }
}
