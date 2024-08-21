/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QQmlParserStatus>
#include <QRunnable>

class SearchThread;

class DocumentListModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(DocumentType filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString documentsFolder READ documentsFolder CONSTANT)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit DocumentListModel(QObject *parent = nullptr);
    ~DocumentListModel() override;

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
    QString documentsFolder() const;

    static QString prettyTime(QDateTime theTime);

Q_SIGNALS:
    void filterChanged();

public Q_SLOTS:
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

    QHash<QString, DocumentType> m_docTypes;
    QList<DocumentInfo> m_allDocumentInfos;
    QList<DocumentInfo> m_currentDocumentInfos;
    SearchThread *m_searchThread;
    GroupBy m_groupBy;
    DocumentType m_filter;
    QString m_filteredTypes;
    friend class SearchThread;
};

Q_DECLARE_METATYPE(DocumentListModel::DocumentInfo);

class SearchThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SearchThread(const QHash<QString, DocumentListModel::DocumentType> &docTypes, QObject *parent = nullptr);
    ~SearchThread() override;

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
    QHash<QString, DocumentListModel::DocumentType> m_docTypes;
    static const QString textDocumentType;
    static const QString presentationType;
    static const QString spreadsheetType;
};

#endif // CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
