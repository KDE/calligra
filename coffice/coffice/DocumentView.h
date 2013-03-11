#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QDeclarativeItem>

class KoDocument;

class DocumentView : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit DocumentView(QDeclarativeItem *parent = 0);
    virtual ~DocumentView();

signals:
    
public slots:
    bool loadFile(const QString &file);

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    KoDocument *m_document;
};

#endif // DOCUMENTVIEW_H
