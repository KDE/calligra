/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef CALLIGRAGEMINI_GIT_DOCUMENTLISTMODEL_H
#define CALLIGRAGEMINI_GIT_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QQmlParserStatus>
#include <QRunnable>

class SearchThread;

class DocumentListModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(DocumentListModel::DocumentType filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString documentsFolder READ documentsFolder CONSTANT)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit DocumentListModel(QObject *parent = nullptr);
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
    Q_ENUM(GroupBy);

    enum DocumentType {
        UnknownType,
        TextDocumentType,
        PresentationType,
        SpreadsheetType,
    };
    Q_ENUM(DocumentType);

    struct DocumentInfo {
        bool operator==(const DocumentInfo &other) const
        {
            return filePath == other.filePath;
        }
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
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // reimp from QDeclarativeParserStatus
    void classBegin() override;
    void componentComplete() override;

    DocumentType filter();

    void setDocumentsFolder(const QString &newFolder);
    QString documentsFolder() const;

    static QString prettyTime(QDateTime theTime);

Q_SIGNALS:
    void filterChanged();

public Q_SLOTS:
    void rescan();
    void startSearch();
    void stopSearch();
    void addDocument(const DocumentListModel::DocumentInfo &info);
    void setFilter(DocumentListModel::DocumentType newFilter);

public:
    Q_INVOKABLE void groupBy(DocumentListModel::GroupBy role);

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
    SearchThread(const QHash<QString, DocumentListModel::DocumentType> &docTypes, QString docDir, QObject *parent = nullptr);
    ~SearchThread();

    void run() override;

    void abort()
    {
        m_abort = true;
    }

Q_SIGNALS:
    void documentFound(const DocumentListModel::DocumentInfo &);
    void finished();

private:
    bool m_abort;
    QString m_docDir;
    QHash<QString, DocumentListModel::DocumentType> m_docTypes;
};

#endif // CALLIGRAGEMINI_GIT_DOCUMENTLISTMODEL_H
