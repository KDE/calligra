

#include "KPrCustomSlideShowsModel.h"


//Calligra headers
#include "KPrCustomSlideShows.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"
#include "commands/KPrEditCustomSlideShowsCommand.h"

//KDE headers
#include "KLocalizedString"
#include "KIcon"

//Qt headers
#include <QIcon>
#include <QMimeData>
#include <QApplication>
#include <QMenu>

KPrCustomSlideShowsModel::KPrCustomSlideShowsModel(QObject *parent, KPrCustomSlideShows *customShows, KPrDocument *document)
    : QAbstractListModel(parent)
    , m_customShows(customShows)
    , m_iconSize(QSize(200,200))
    , m_document(document)
{

}

KPrCustomSlideShowsModel::~KPrCustomSlideShowsModel(){
}

QVariant KPrCustomSlideShowsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_customShows || m_currentSlideShowName.isEmpty()) {
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

Qt::ItemFlags KPrCustomSlideShowsModel::flags(const QModelIndex &index) const
{
    if (m_currentSlideShowName.isEmpty()) {
        return 0;
    }

    Qt::ItemFlags defaultFlags = QAbstractListModel::flags (index);

    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    } else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

QModelIndex KPrCustomSlideShowsModel::index(int row, int column, const QModelIndex &parent) const
{
    if(m_currentSlideShowName.isEmpty())
        return QModelIndex();

    // check if parent is root node
    if(!parent.isValid())
    {
        if(row >= 0 && row < rowCount(QModelIndex()))
            return createIndex(row, column, m_customShows->pageByIndex(m_currentSlideShowName, row));
    }
    return QModelIndex();
}

QStringList KPrCustomSlideShowsModel::mimeTypes() const
{
    return QStringList() << "application/x-koffice-customslideshows";
}

QMimeData * KPrCustomSlideShowsModel::mimeData(const QModelIndexList &indexes) const
{
    // check if there is data to encode
    if( ! indexes.count() )
        return 0;

    // check if we support a format
    QStringList types = mimeTypes();
    if( types.isEmpty() )
        return 0;

    QMimeData *data = new QMimeData();
    QString format = types[0];
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    // encode the data
    QModelIndexList::ConstIterator it = indexes.begin();
    for( ; it != indexes.end(); ++it)
        stream << QVariant::fromValue( qulonglong( it->internalPointer() ) );

    data->setData(format, encoded);
    return data;
}

Qt::DropActions KPrCustomSlideShowsModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

bool KPrCustomSlideShowsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction) {
        return true;
    }

    if (data->hasFormat("application/x-koffice-sliderssorter") | data->hasFormat("application/x-koffice-customslideshows")) {

        if (column > 0) {
            return false;
        }

        QList<KoPAPageBase *> slides;
        QList<KoPAPageBase*> selectedSlideShow;

        int beginRow;

        if (row != -1) {
            beginRow = row;
        } else if (parent.isValid()) {
            beginRow = parent.row();
        } else {
            beginRow = rowCount(QModelIndex());
        }

        if (data->hasFormat("application/x-koffice-sliderssorter")) {

            QByteArray encoded = data->data("application/x-koffice-sliderssorter");
            QDataStream stream(&encoded, QIODevice::ReadOnly);

            // decode the data
            while( ! stream.atEnd() )
            {
                QVariant v;
                stream >> v;
                slides.append( static_cast<KoPAPageBase*>((void*)v.value<qulonglong>()));
            }

            if (slides.empty ()) {
                return false;
            }

            qSort(slides.begin(), slides.end());

            //get the slideshow
            selectedSlideShow = m_customShows->getByName(m_currentSlideShowName);

            //insert the slides and update the Widget
            int i = beginRow;
            foreach(KoPAPageBase* page, slides)
            {
                selectedSlideShow.insert(i, page);
                i++;
            }

        }

        if (data->hasFormat("application/x-koffice-customslideshows")) {

            QByteArray encoded = data->data("application/x-koffice-customslideshows");
            QDataStream stream(&encoded, QIODevice::ReadOnly);

            // decode the data
            while( ! stream.atEnd() )
            {
                QVariant v;
                stream >> v;
                slides.append( static_cast<KoPAPageBase*>((void*)v.value<qulonglong>()));
            }

            if (slides.empty ()) {
                return false;
            }

            //order slides
            QMap<int, KoPAPageBase*> map;
            foreach (KoPAPageBase* slide, slides)
                map.insert(m_customShows->indexByPage(m_currentSlideShowName, slide), slide);

            slides = map.values();

            //get the slideshow
           selectedSlideShow = m_customShows->getByName(m_currentSlideShowName);

            //insert the slides and update the Widget
           if (beginRow >= selectedSlideShow.count())
               beginRow = selectedSlideShow.count() - 1;

            foreach(KoPAPageBase* page, slides)
            {
                int from = selectedSlideShow.indexOf(page);
                selectedSlideShow.move(from, beginRow);
            }

        }

        //update the SlideShow with the resulting list
        KPrEditCustomSlideShowsCommand *command = new KPrEditCustomSlideShowsCommand(
                    m_document, this, m_currentSlideShowName, selectedSlideShow);
        m_document->addCommand(command);

        return true;
    }

    return false;
}

void KPrCustomSlideShowsModel::doDrop(QList<KoPAPageBase *> slides, KoPAPageBase *pageAfter, Qt::DropAction action)
{
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

    if (((modifiers & Qt::ControlModifier) == 0) &&
        ((modifiers & Qt::ShiftModifier) == 0)) {
           QMenu popup;
           QString seq = QKeySequence(Qt::ShiftModifier).toString();
           seq.chop(1);
           QAction* popupMoveAction = new QAction(i18n("&Move Here") + '\t' + seq, this);
           popupMoveAction->setIcon(KIcon("go-jump"));
           seq = QKeySequence(Qt::ControlModifier).toString();
           seq.chop(1);
           QAction* popupCopyAction = new QAction(i18n("&Copy Here") + '\t' + seq, this);
           popupCopyAction->setIcon(KIcon("edit-copy"));
           seq = QKeySequence( Qt::ControlModifier + Qt::ShiftModifier ).toString();
           seq.chop(1);
           QAction* popupCancelAction = new QAction(i18n("C&ancel") + '\t' + QKeySequence(Qt::Key_Escape).toString(), this);
           popupCancelAction->setIcon(KIcon("process-stop"));

           popup.addAction(popupMoveAction);
           popup.addAction(popupCopyAction);

           popup.addSeparator();
           popup.addAction(popupCancelAction);

           QAction* result = popup.exec(QCursor::pos());

           if(result == popupCopyAction)
               action = Qt::CopyAction;
           else if(result == popupMoveAction)
               action = Qt::MoveAction;
           else {
               return;
           }
    } else if ((modifiers & Qt::ControlModifier) != 0) {
        action = Qt::CopyAction;
    } else if ((modifiers & Qt::ShiftModifier) != 0) {
        action = Qt::MoveAction;
    } else {
        return;
    }


   switch ( action ) {
   case Qt::MoveAction : {
       return;
   }
   case Qt::CopyAction : {
       return;
   }
   default :
       qDebug("Unknown action: %d ", (int)action);
       return;
   }
}

void KPrCustomSlideShowsModel::setCustomSlideShows(KPrCustomSlideShows *customShows)
{
    m_customShows = customShows;
    m_currentSlideShowName = "";
    reset();
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(QString name)
{
    if (!m_customShows | (m_currentSlideShowName == name))
        return;
    if (m_customShows->names().contains(name)) {
        m_currentSlideShowName = name;
    }
    reset();
}

void KPrCustomSlideShowsModel::setCurrentSlideShow(int index)
{
    if (!m_customShows)
        return;
    QString name = m_customShows->names().value(index);
    setCurrentSlideShow(name);
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

void KPrCustomSlideShowsModel::setDocument(KPrDocument *document)
{
    m_document = document;
    setCustomSlideShows(document->customSlideShows());
}

void KPrCustomSlideShowsModel::updateCustomShow(QString name, QList<KoPAPageBase *> newCustomShow)
{
    m_customShows->update(name, newCustomShow);
    reset();
}
