#include "ui_FileChooserDialog.h"
#include "FileChooserDialog.h"

#include <KConfig>
#include <KUrl>

#include <QStandardItemModel>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QTabWidget>
#include <QDebug>

FileChooserDialog::FileChooserDialog(QWidget *parent) :
      QDialog(parent)
      , fileChooser(new Ui::FileChooserDialog)
      , recentModel(0)
      , fileModel(0)
{
    m_config = new KConfig("wordsrc");
    initUi();
}

void FileChooserDialog::initUi()
{
    fileChooser->setupUi(this);
    recentModel = new QStandardItemModel(0, 2, fileChooser->recentView);
    fileChooser->recentView->setModel(recentModel);
    updateRecentDocumentModel();
    fileChooser->horizontalLayoutWidget->setVisible(false);

    connect(fileChooser->fileChooser, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(fileChooser->levelUp, SIGNAL(clicked()), this, SLOT(moveLevelUp()));

    QDir *dir = new QDir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    fileChooser->filePath->setText(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    fileModel = new QFileSystemModel();
    fileModel->setRootPath("");

    fileChooser->documentView->setModel(fileModel);
    fileChooser->documentView->setRootIndex(fileModel->index(dir->path()));
    connect(fileChooser->documentView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRootIndex(QModelIndex)));

    fileChooser->presentationView->setModel(fileModel);
    fileChooser->presentationView->setRootIndex(fileModel->index(dir->path()));
    connect(fileChooser->presentationView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRootIndex(QModelIndex)));

    fileChooser->spreadsheetView->setModel(fileModel);
    fileChooser->spreadsheetView->setRootIndex(fileModel->index(dir->path()));
    connect(fileChooser->spreadsheetView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRootIndex(QModelIndex)));
    fileModel->setFilter(QDir::Hidden);
    fileModel->setNameFilterDisables(true);
}

void FileChooserDialog::updateRecentDocumentModel()
{
    KConfigGroup config(m_config, "RecentFiles");
    int i = 0;
    QString path;
    do {
        path = config.readPathEntry(QString("File%1").arg(i+1), QString());
        qDebug() << "++++ Debug ++++" << path;
        if (!path.isEmpty()) {
            QString name = config.readPathEntry(QString("Name%1").arg(i+1), QString());
            KUrl url(path);
            if (name.isEmpty())
                name = url.fileName();
            recentModel->insertRow(i, QModelIndex());
            recentModel->setData(recentModel->index(i, 0, QModelIndex()), path);
            recentModel->setData(recentModel->index(i, 1, QModelIndex()), name);
            recentModel->setData(recentModel->index(i, 0, QModelIndex()),
                                 QPixmap(":/images/48x48/Document.png"),
                                 Qt::DecorationRole);
        }
        i++;
    } while (!path.isEmpty() || i < 10);
}

void FileChooserDialog::changeRootIndex(QModelIndex index)
{
    QDir dir(fileChooser->filePath->text() + "/" + index.data().toString());
    fileChooser->documentView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->presentationView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->spreadsheetView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->filePath->setText(dir.path());
}

void FileChooserDialog::tabChanged(int index)
{
    switch(index) {
    case 0:
        fileChooser->horizontalLayoutWidget->setVisible(false);
        break;
    case 1:
        fileChooser->horizontalLayoutWidget->setVisible(true);
        fileModel->setNameFilters(QStringList() << "*.doc" << "*.docx" << "*.odt");
        break;
    case 2:
        fileChooser->horizontalLayoutWidget->setVisible(true);
        fileModel->setNameFilters(QStringList() << "*.ppt" << "*.pptx" << "*.pps" << "*.ppsx" << "*.odp");
        break;
    case 3:
        fileChooser->horizontalLayoutWidget->setVisible(true);
        fileModel->setNameFilters(QStringList() << "*.xls" << "*.xlsx" << "*.ods");
        break;
    }
}

void FileChooserDialog::moveLevelUp()
{
    QDir dir(fileChooser->filePath->text());
    dir.cdUp();
    qDebug() << dir.path();
    fileChooser->documentView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->presentationView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->spreadsheetView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->filePath->setText(dir.path());
}
