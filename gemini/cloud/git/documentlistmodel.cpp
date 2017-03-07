/* This file is part of the KDE project
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "documentlistmodel.h"

#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QRunnable>
#include <QThreadPool>
#include <QTimer>
#include <QLocale>

#include <KSharedConfig>
#include <KConfigGroup>

#include <kfileitem.h>

QDebug operator<<(QDebug dbg, const DocumentListModel::DocumentInfo& d) { 
    dbg.nospace() << d.filePath << "," << d.fileName << "," << d.docType << "," << d.fileSize << "," << d.authorName << "," << d.accessedTime << "," << d.modifiedTime << "," << d.uuid;
    return dbg.space();
};

SearchThread::SearchThread(const QHash<QString, DocumentListModel::DocumentType> &docTypes, QString docDir, QObject *parent) 
    : QObject(parent), m_abort(false), m_docDir(docDir), m_docTypes(docTypes)
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
    QStringList nameFilters;
    for (QHash<QString, DocumentListModel::DocumentType>::const_iterator it = m_docTypes.constBegin(); it != m_docTypes.constEnd(); ++it)
        nameFilters.append("*." + it.key());

    QDirIterator it(m_docDir, nameFilters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext() && !m_abort) {
        it.next();
        DocumentListModel::DocumentInfo info;
        info.fileName = it.fileName();
        info.authorName = "-";
        info.filePath = it.filePath();
        info.modifiedTime = it.fileInfo().lastModified();
        info.accessedTime = it.fileInfo().lastRead();
        info.fileSize = QString("%1").arg(it.fileInfo().size());
        info.docType = m_docTypes.value(info.filePath.split('.').last());
        info.uuid = "not known...";
        emit documentFound(info);
    }

    emit finished();
}

DocumentListModel::DocumentListModel(QObject *parent)
    : QAbstractListModel(parent), m_searchThread(0), m_groupBy(GroupByName), m_filter(UnknownType)
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[FileNameRole] = "fileName";
    roleNames[FilePathRole] = "filePath";
    roleNames[DocTypeRole] = "docType";
    roleNames[SectionCategoryRole] = "sectionCategory";
    roleNames[FileSizeRole] = "fileSize";
    roleNames[AuthorNameRole] = "authorName";
    roleNames[AccessedTimeRole] = "accessedTime";
    roleNames[ModifiedTimeRole] = "modifiedTime";
    roleNames[UUIDRole] = "uuid";
    setRoleNames(roleNames);

    m_docTypes["odt"] = TextDocumentType;
    m_docTypes["doc"] = TextDocumentType;
    m_docTypes["docx"] = TextDocumentType;
    m_docTypes["odp"] = PresentationType;
    m_docTypes["ppt"] = PresentationType;
    m_docTypes["pptx"] = PresentationType;
    m_docTypes["ods"] = SpreadsheetType;
    m_docTypes["xls"] = SpreadsheetType;
    m_docTypes["xlsx"] = SpreadsheetType;
}

DocumentListModel::~DocumentListModel()
{
    stopSearch();
}

void DocumentListModel::startSearch()
{
    if (m_searchThread) {
        qDebug() << "Already searching or finished search";
        return;
    }
    if(m_documentsFolder.isEmpty()) {
        qDebug() << "No search folder is set - not performing search";
        return;
    }
    m_searchThread = new SearchThread(m_docTypes, m_documentsFolder);
    connect(m_searchThread, SIGNAL(documentFound(DocumentListModel::DocumentInfo)), this, SLOT(addDocument(DocumentListModel::DocumentInfo)));
    connect(m_searchThread, SIGNAL(finished()), this, SLOT(searchFinished()));
    m_searchThread->setAutoDelete(false);
    QThreadPool::globalInstance()->start(m_searchThread);
}

void DocumentListModel::stopSearch()
{
    if (m_searchThread)
        m_searchThread->abort();
}

void DocumentListModel::searchFinished()
{
    Q_ASSERT(m_searchThread);
    delete m_searchThread;
    m_searchThread = 0;
}

void DocumentListModel::addDocument(const DocumentInfo &info)
{
    if(m_allDocumentInfos.contains(info))
    {
        qDebug() << "Attempted to add duplicate entry" << info;
        return;
    }

    m_allDocumentInfos.append(info);

    if(m_filter == UnknownType || info.docType == m_filter) {
        beginInsertRows(QModelIndex(), m_currentDocumentInfos.count(), m_currentDocumentInfos.count());
        m_currentDocumentInfos.append(info);
        endInsertRows();
    }
}

int DocumentListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return m_currentDocumentInfos.count();
}

int DocumentListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 1;
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    const int row = index.row();
    const DocumentInfo &info = m_currentDocumentInfos[row];

    switch (role) {
    case FileNameRole: // intentional fall through
    case Qt::DisplayRole: return info.fileName;
    case FilePathRole: return info.filePath;
    case DocTypeRole: return info.docType;
    case FileSizeRole: return info.fileSize;
    case AuthorNameRole: return info.authorName;
    case AccessedTimeRole: return prettyTime(info.accessedTime);
    case ModifiedTimeRole: return prettyTime(info.modifiedTime);
    case UUIDRole: return info.uuid;
    case SectionCategoryRole: 
        return m_groupBy == GroupByName ? info.fileName[0].toUpper() : info.docType;
    default: return QVariant();
    }
}

QString DocumentListModel::prettyTime(QDateTime theTime)
{
    // QT5TODO: used to be KLocale::FancyShortDate, but no such thing with QLocale (also static anyway)
    return QLocale().toString(theTime, QLocale::ShortFormat);
}

QVariant DocumentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();
    switch (section) {
    case 0: return tr("Filename");
    case 1: return tr("Path");
    case 2: return tr("Type");
    case 3: return tr("Size");
    case 4: return tr("Author");
    case 5: return tr("Last Accessed");
    case 6: return tr("Last Modified");
    default: return QVariant();
    }
}

void DocumentListModel::groupBy(GroupBy role)
{
    if (m_groupBy == role)
        return;
    m_groupBy = role;
    relayout();
}

void DocumentListModel::relayout()
{
    beginResetModel();
    emit layoutAboutToBeChanged();

    QList<DocumentInfo> newList;
    foreach(const DocumentInfo &docInfo, m_allDocumentInfos) {
        if(m_filter == UnknownType || docInfo.docType == m_filter) {
            qDebug() << docInfo.filePath;
            newList.append(docInfo);
        }
    }
    
    m_currentDocumentInfos = newList;
    emit layoutChanged();
    endResetModel();
}

void DocumentListModel::classBegin()
{
}

DocumentListModel::DocumentType DocumentListModel::filter()
{
    return m_filter;
}

QString DocumentListModel::documentsFolder() const
{
    return m_documentsFolder;
}

void DocumentListModel::setDocumentsFolder(const QString& newFolder)
{
    m_documentsFolder = newFolder;
    rescan();
}

void DocumentListModel::rescan()
{
    stopSearch();
    beginResetModel();
    m_allDocumentInfos.clear();
    m_currentDocumentInfos.clear();
    endResetModel();
    qApp->processEvents();
    startSearch();
}

void DocumentListModel::setFilter(DocumentListModel::DocumentType newFilter)
{
    m_filter = newFilter;
    relayout();
    emit filterChanged();
}

void DocumentListModel::componentComplete()
{
    reset();
}

