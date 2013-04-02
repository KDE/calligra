#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QRectF>
#include <QImage>
#include <QSharedPointer>

class KoPart;

class Document;

class Page : public QObject
{
    Q_OBJECT
public:
    Page(Document *doc, int pageNumber, const QRectF &rect);
    virtual ~Page();
    Document* doc() const;
    int pageNumber() const;
    QRectF rect() const;
    void markDirty();
public Q_SLOTS:
    void maybeUpdateThumbnail();
    void forceUpdateThumbnail();
Q_SIGNALS:
    void thumbnailFinished(const QImage &image);
private:
    class Private;
    Private *const d;
};

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = 0);
    virtual ~Document();
    QList< QSharedPointer<Page> > pages() const;
    bool openFile(const QString &file);
    void emitProgressUpdated(int percent);
Q_SIGNALS:
    //void openFileSucceeded();
    void openFileFailed();
    void progressUpdated(int percent);
    void layoutFinished();
private:
    friend class OpenFileCommand;
    friend class UpdatePageCommand;
    friend class AppManager;

    class Private;
    Private *const d;
};

#endif // DOCUMENT_H
