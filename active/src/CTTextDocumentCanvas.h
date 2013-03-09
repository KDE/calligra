#ifndef CTTEXTDOCUMENTCANVAS_H
#define CTTEXTDOCUMENTCANVAS_H

#include <QtDeclarative/qdeclarativeitem.h>

class KoCanvasController;
class KoCanvasBase;
class KUrl;

class CTTextDocumentCanvas : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
public:
    CTTextDocumentCanvas();
    ~CTTextDocumentCanvas();

    QString source() const;
    void setSource(const QString &source);

signals:
    void sourceChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private:
    bool openFile(const QString& uri);
    void createAndSetCanvasControllerOn(KoCanvasBase *canvas);

    QString m_source;
    KoCanvasBase *m_canvasBase;
    KoCanvasController *m_canvasController;
};

#endif // CTTEXTDOCUMENTCANVAS_H
