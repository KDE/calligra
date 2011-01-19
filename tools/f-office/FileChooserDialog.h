#ifndef FILECHOOSERDIALOG_H
#define FILECHOOSERDIALOG_H

#include <QtGui/QDialog>
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

signals:

public slots:
    void changeRootIndex(QModelIndex index);
    void tabChanged(int index);
    void moveLevelUp();

private:
    Ui::FileChooserDialog *fileChooser;
    KConfig *m_config;
    QStandardItemModel *recentModel;
    QFileSystemModel   *fileModel;

};

#endif // FILECHOOSERDIALOG_H
