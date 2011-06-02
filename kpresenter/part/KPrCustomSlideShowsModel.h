#ifndef KPRCUSTOMSLIDESHOWSMODEL_H
#define KPRCUSTOMSLIDESHOWSMODEL_H

#include <QAbstractListModel>
#include <QSize>

class KPrCustomSlideShows;
class KoPAPageBase;
class KPrDocument;

class KPrCustomSlideShowsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KPrCustomSlideShowsModel(QObject *parent = 0, KPrCustomSlideShows *customShows = 0, KPrDocument *document = 0);

    virtual ~KPrCustomSlideShowsModel();

    QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    virtual QStringList mimeTypes() const;

    virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

    virtual Qt::DropActions supportedDropActions() const;

    //virtual bool removeRows(int row, int count, const QModelIndex &parent);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    /**
     * Drop selected slides (copy/move) if a modifier key is pressed
     * or display a context menu with alternatives.
     * @param slides list of slides to be dropped
     * @param pageAfter destination of the drop
     * @param action the drop action
     */
    void doDrop(QList<KoPAPageBase *> slides, KoPAPageBase * pageAfter, Qt::DropAction action);

    void setCustomSlideShows(KPrCustomSlideShows *customShows);

    void setCurrentSlideShow(QString name);

    void setCurrentSlideShow(int index);

    void setIconSize(QSize size);

    QStringList customShowsNamesList() const;

    void setDocument(KPrDocument* document);

    void updateCustomShow(QString name, QList<KoPAPageBase *> newCustomShow);

signals:

public slots:

private:
    KPrCustomSlideShows *m_customShows;
    QString m_currentSlideShowName;
    QSize m_iconSize;
    KPrDocument *m_document;

};

#endif // KPRCUSTOMSLIDESHOWSMODEL_H
