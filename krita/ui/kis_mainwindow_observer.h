#ifndef KIS_MAINWINDOW_OBSERVER_H
#define KIS_MAINWINDOW_OBSERVER_H

#include <KoCanvasObserverBase.h>
#include <krita_export.h>

class KisCanvasResourceProvider;

class KRITAUI_EXPORT KisMainwindowObserver : public KoCanvasObserverBase
{   
public:
    KisMainwindowObserver();
    virtual ~KisMainwindowObserver();

    virtual void setCanvasResourceProvider(KisCanvasResourceProvider* canvasResourceProvider) = 0;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
};

#endif // KIS_MAINWINDOW_OBSERVER_H
