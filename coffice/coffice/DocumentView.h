#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QDeclarativeItem>

class QGraphicsProxyWidget;

class KoPart;
class KoView;

class DocumentView : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit DocumentView(QDeclarativeItem *parent = 0);
    virtual ~DocumentView();

signals:
    
public slots:
    bool openFile(const QString &file);

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    KoPart *m_part;
    KoView* m_view;
    QGraphicsProxyWidget *m_proxyWidget;

    bool init();
};

#endif // DOCUMENTVIEW_H
