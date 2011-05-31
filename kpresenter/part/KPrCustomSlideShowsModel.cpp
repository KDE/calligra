

#include "KPrCustomSlideShowsModel.h"

#include "KPrCustomSlideShows.h"

//Calligra headers
#include "KoPAPageBase.h"

//KDE headers
#include "KLocalizedString"

//Qt headers
#include <QIcon>

KPrCustomSlideShowsModel::KPrCustomSlideShowsModel(QObject *parent, KPrCustomSlideShows *customShows)
    : QAbstractListModel(parent)
    , m_customShows(customShows)
    , m_iconSize(QSize(200,200))
{

}

QVariant KPrCustomSlideShowsModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid() || !m_customShows || m_currentSlideShowName.isEmpty()) {
        return QVariant();
    }

    Q_ASSERT(index.model() == this);

    KoPAPageBase *page = m_customShows->pageByIndex(m_currentSlideShowName, index.row());

    switch (role) {
        case Qt::DisplayRole:
        {
            QString name = i18n("Unknown");
            if (page)
            {
                name = page->name ();
                if (name.isEmpty())
                {
                    //Default case
                    name = i18n("Slide %1",  index.row());
                }
            }
            return name;
        }
        case Qt::DecorationRole:
        {
            return QIcon(page->thumbnail(m_iconSize));
        }
        default:
            return QVariant();
    }
}

int KPrCustomSlideShowsModel::rowCount(const QModelIndex &parent) const
{
    if (!m_currentSlideShowName.isEmpty()) {
        if (!parent.isValid())
            return m_customShows->getByName(m_currentSlideShowName).count();
    }

    return 0;
}

void KPrCustomSlideShowsModel::setCustomSlideShows(KPrCustomSlideShows *customShows)
{
    m_customShows = customShows;
    m_currentSlideShowName = "";
    reset();
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(QString name)
{
    if (m_customShows->names().contains(name)) {
        m_currentSlideShowName = name;
    }
    reset();
}

void KPrCustomSlideShowsModel::setIconSize(QSize size)
{
    if (size != m_iconSize)
        m_iconSize = size;
}

QStringList KPrCustomSlideShowsModel::customShowsNamesList() const
{
    if (m_customShows) {
        return m_customShows->names();
    }

    return QStringList();

}
