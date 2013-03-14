#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QDeclarativeItem>
#include <QGraphicsPixmapItem>

class QGraphicsProxyWidget;

class KoPart;
class KoView;
class KoDocument;
class KWPage;

class DocumentView;

class DocumentPage : public QGraphicsPixmapItem
{
public:
    DocumentPage(const KWPage &page, DocumentView *view);
    virtual ~DocumentPage();
private:
    class Private;
    Private *const d;
};

class DocumentView : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit DocumentView(QDeclarativeItem *parent = 0);
    virtual ~DocumentView();

signals:
    
public slots:
    bool openFileWithDialog();
    bool openFile(const QString &file);

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    class Private;
    Private *const d;
};

#endif // DOCUMENTVIEW_H
