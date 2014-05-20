#ifndef CALLIGRAMOBILE_DOCUMENTLISTMODEL_H
#define CALLIGRAMOBILE_DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>
#include <QRunnable>
#include <QDateTime>

class SearchThread;

class DocumentListModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(DocumentType filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString documentsFolder READ documentsFolder CONSTANT)
    Q_ENUMS(GroupBy)
    Q_ENUMS(Filter)
    Q_ENUMS(DocumentType)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    DocumentListModel(QObject *parent = 0);
    ~DocumentListModel();

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
    QString documentsFolder() const;

    static QString prettyTime(QDateTime theTime);

signals:
    void filterChanged();

public slots:
    void startSearch();
    void stopSearch();
    void addDocument(const DocumentListModel::DocumentInfo &info);
    void setFilter(DocumentType newFilter);

public:
    Q_INVOKABLE void groupBy(GroupBy role);

private slots:
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
    SearchThread(const QHash<QString, DocumentListModel::DocumentType> &docTypes, QObject *parent = 0);
    ~SearchThread();

    void run();
    
    void abort() { m_abort = true; }

signals:
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

