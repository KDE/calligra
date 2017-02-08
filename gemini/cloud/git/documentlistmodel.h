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

#ifndef CALLIGRAGEMINI_GIT_DOCUMENTLISTMODEL_H
#define CALLIGRAGEMINI_GIT_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QRunnable>
#include <QDateTime>

class SearchThread;

class DocumentListModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(DocumentListModel::DocumentType filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString documentsFolder READ documentsFolder CONSTANT)
    Q_ENUMS(GroupBy)
    Q_ENUMS(Filter)
    Q_ENUMS(DocumentType)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit DocumentListModel(QObject *parent = 0);
    virtual ~DocumentListModel();

    enum CustomRoles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        DocTypeRole,
        SectionCategoryRole,
        FileSizeRole,
        AuthorNameRole,
        AccessedTimeRole,
        ModifiedTimeRole,
        UUIDRole,
    };

    enum GroupBy { GroupByName, GroupByDocType };

    enum DocumentType
    {
        UnknownType,
        TextDocumentType,
        PresentationType,
        SpreadsheetType,
    };

    struct DocumentInfo {
        bool operator==(const DocumentInfo &other) const { return filePath == other.filePath; }
        QString filePath;
        QString fileName;
        DocumentType docType;
        QString fileSize;
        QString authorName;
        QDateTime accessedTime;
        QDateTime modifiedTime;
        QString uuid;
    };

    // reimp from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // reimp from QDeclarativeParserStatus
    void classBegin();
    void componentComplete();

    DocumentType filter();

    void setDocumentsFolder(const QString& newFolder);
    QString documentsFolder() const;

    static QString prettyTime(QDateTime theTime);

Q_SIGNALS:
    void filterChanged();

public Q_SLOTS:
    void rescan();
    void startSearch();
    void stopSearch();
    void addDocument(const DocumentListModel::DocumentInfo &info);
    void setFilter(DocumentType newFilter);

public:
    Q_INVOKABLE void groupBy(GroupBy role);

private Q_SLOTS:
    void searchFinished();

private:
    void relayout();

    QString m_documentsFolder;
    QHash<QString, DocumentType> m_docTypes;
    QList<DocumentInfo> m_allDocumentInfos;
    QList<DocumentInfo> m_currentDocumentInfos;
    SearchThread *m_searchThread;
    GroupBy m_groupBy;
    DocumentType m_filter;
    QString m_filteredTypes;
    friend class SearchThread;
};

Q_DECLARE_METATYPE(DocumentListModel::DocumentType)

class SearchThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SearchThread(const QHash<QString, DocumentListModel::DocumentType> &docTypes, QString docDir, QObject *parent = 0);
    ~SearchThread();

    void run();

    void abort() { m_abort = true; }

Q_SIGNALS:
    void documentFound(const DocumentListModel::DocumentInfo &);
    void finished();

private:
    bool m_abort;
    QString m_docDir;
    QHash<QString, DocumentListModel::DocumentType> m_docTypes;
};

#endif // CALLIGRAGEMINI_GIT_DOCUMENTLISTMODEL_H

