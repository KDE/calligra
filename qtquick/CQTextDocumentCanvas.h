#ifndef CQTEXTDOCUMENTCANVAS_H
#define CQTEXTDOCUMENTCANVAS_H

#include <QtDeclarative/qdeclarativeitem.h>

#include <KoZoomMode.h>

class CQTextDocumentModel;
class KoFindText;
class KoDocument;
class KoZoomController;
class KoCanvasController;
class KoCanvasBase;
class KUrl;
class KoFindMatch;

class CQTextDocumentCanvas : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(ZoomMode zoomMode READ zoomMode WRITE setZoomMode NOTIFY zoomModeChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(QObject* documentModel READ documentModel NOTIFY documentModelChanged)
    Q_ENUMS(ZoomMode)

public:
    CQTextDocumentCanvas();
    ~CQTextDocumentCanvas();

    enum ZoomMode
    {
        ZOOM_CONSTANT = 0,  ///< zoom x %
        ZOOM_WIDTH    = 1,  ///< zoom pagewidth
        ZOOM_PAGE     = 2,  ///< zoom to pagesize
    };

    QString source() const;
    void setSource(const QString &source);

    ZoomMode zoomMode() const;
    void setZoomMode(ZoomMode zoomMode);

    QString searchTerm() const;
    void setSearchTerm(const QString &term);

    QObject *documentModel() const;

signals:
    void sourceChanged();
    void zoomModeChanged();
    void searchTermChanged();
    void documentModelChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private slots:
    void updateControllerWithZoomMode();
    void findNoMatchFound();
    void findMatchFound(const KoFindMatch& match);
    void updateCanvas();
    void findPrevious();
    void findNext();

private:
    bool openFile(const QString& uri);
    void createAndSetCanvasControllerOn(KoCanvasBase *canvas);
    void createAndSetZoomController(KoCanvasBase *canvas);
    void updateZoomControllerAccordingToDocument(const KoDocument *document);

    QString m_source;
    KoCanvasBase *m_canvasBase;
    KoCanvasController *m_canvasController;
    KoZoomController *m_zoomController;
    ZoomMode m_zoomMode;
    QString m_searchTerm;
    KoFindText *m_findText;
    CQTextDocumentModel *m_documentModel;
};
#endif // CQTEXTDOCUMENTCANVAS_H
