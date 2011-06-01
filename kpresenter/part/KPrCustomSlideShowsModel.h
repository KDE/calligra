#ifndef KPRCUSTOMSLIDESHOWSMODEL_H
#define KPRCUSTOMSLIDESHOWSMODEL_H

#include <QAbstractListModel>
#include <QSize>

class KPrCustomSlideShows;

class KPrCustomSlideShowsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KPrCustomSlideShowsModel(QObject *parent = 0, KPrCustomSlideShows *customShows = 0);

    QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;

    //virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    //virtual QStringList mimeTypes() const;

    //virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

    //virtual Qt::DropActions supportedDropActions() const;

    //virtual bool removeRows(int row, int count, const QModelIndex &parent);

    //virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    //virtual bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    void setCustomSlideShows(KPrCustomSlideShows *customShows);

    void setCurrentSlideShow(QString name);

    void setCurrentSlideShow(int index);

    void setIconSize(QSize size);

    QStringList customShowsNamesList() const;

signals:

public slots:

private:
    KPrCustomSlideShows *m_customShows;
    QString m_currentSlideShowName;
    QSize m_iconSize;

};

#endif // KPRCUSTOMSLIDESHOWSMODEL_H
