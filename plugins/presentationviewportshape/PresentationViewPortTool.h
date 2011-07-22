#ifndef PRESENTATIONVIEWPORT_TOOL
#define PRESENTATIONVIEWPORT_TOOL

#include <KoToolBase.h>
#include <KJob>
#include <KoInteractionTool.h>

class PresentationViewPortShape;
  
class PresentationViewPortTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit PresentationViewPortTool(KoCanvasBase* canvas);

    /// reimplemented from KoToolBase
    virtual void paint(QPainter&, const KoViewConverter&) {}
    /// reimplemented from KoToolBase
    virtual void mousePressEvent(KoPointerEvent*);
    /// reimplemented from superclass
    virtual void mouseDoubleClickEvent(KoPointerEvent *event);
    /// reimplemented from KoToolBase
    virtual void mouseMoveEvent(KoPointerEvent*) {}
    /// reimplemented from KoToolBase
    virtual void mouseReleaseEvent(KoPointerEvent*) {}

    /// reimplemented from KoToolBase
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    /// reimplemented from KoToolBase
    virtual void deactivate();

protected:
    /// reimplemented from KoToolBase
    virtual QWidget *createOptionWidget();

private slots:
    void changeUrlPressed();
    void setImageData(KJob *job);

private:
    PresentationViewPortShape *m_shape;
};

#endif
