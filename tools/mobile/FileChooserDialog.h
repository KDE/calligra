#ifndef FILECHOOSERDIALOG_H
#define FILECHOOSERDIALOG_H

#include <QDialog>
#include <QModelIndex>

class KConfig;
class QStandardItemModel;
class QFileSystemModel;

namespace Ui
{
class FileChooserDialog;
}

class FileChooserDialog : public QDialog
{
    Q_OBJECT
public:
    FileChooserDialog(QWidget *parent = 0);
    void initUi();
    void updateRecentDocumentModel();
    QString getFilePath() { return filePath; }
    int checkFileType(QString type);

    enum FILE_TYPE {
        WORDS = 1,
        STAGE,
        TABLES,
        UNKNOWN = -1
    };

signals:

public slots:
    void changeRootIndex(QModelIndex index);
    void tabChanged(int index);
    void moveLevelUp();
    void openRecentDocument(QModelIndex);

private:
    void addPredefinedDir();

    Ui::FileChooserDialog *fileChooser;
    KConfig *m_config;
    QStandardItemModel *recentModel;
    QFileSystemModel   *fileModel;
    QString             filePath;

};

#endif // FILECHOOSERDIALOG_H
