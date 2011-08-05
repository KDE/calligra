#ifndef _KIS_TOOL_SIOXSELECTION_H_
#define _KIS_TOOL_SIOXSELECTION_H_

#include <QVector>

#include <kis_tool.h>
#include <KoToolFactoryBase.h>

class KisCanvas2;

class KisToolSioxSelection : public KisTool {
    Q_OBJECT
public:
    KisToolSioxSelection(KoCanvasBase * canvas);
    virtual ~KisToolSioxSelection();

    //
    // KisToolPaint interface
    //

    virtual quint32 priority() { return 3; }
    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);

// Uncomment if you have a configuration widget
//     QWidget* createOptionWidget();
//     virtual QWidget* optionWidget();

public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    void deactivate();

protected:
    
    virtual void paint(QPainter& gc, const KoViewConverter &converter);

protected:
    KisCanvas2* m_canvas;
};


class KisToolSioxSelectionFactory : public KoToolFactoryBase {

public:
    KisToolSioxSelectionFactory(const QStringList&)
        : KoToolFactoryBase("KisToolSioxSelection")
        {
            setToolTip( i18n( "SioxSelection" ) );
            setToolType( TOOL_TYPE_VIEW );
            setIcon( "tool_sioxselection" );
            setPriority( 0 );
        };


    virtual ~KisToolSioxSelectionFactory() {}

    virtual KoToolBase * createTool(KoCanvasBase * canvas) {
        return new KisToolSioxSelection(canvas);
    }

};

#endif

