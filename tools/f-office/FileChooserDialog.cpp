#include "ui_FileChooserDialog.h"
#include "FileChooserDialog.h"

#include <KConfig>
#include <KConfigGroup>
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
    m_config = new KConfig("calligramobilerc");
    initUi();
}

void FileChooserDialog::initUi()
{
    fileChooser->setupUi(this);
    recentModel = new QStandardItemModel(0, 2, fileChooser->recentView);
    fileChooser->recentView->setModel(recentModel);
    updateRecentDocumentModel();
    connect(fileChooser->recentView, SIGNAL(clicked(QModelIndex)), this, SLOT(openRecentDocument(QModelIndex)));
    fileChooser->horizontalLayoutWidget->setVisible(false);

    connect(fileChooser->fileChooser, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(fileChooser->levelUp, SIGNAL(clicked()), this, SLOT(moveLevelUp()));

    fileChooser->filePath->setText("/home/meego");
    fileModel = new QFileSystemModel();
    fileModel->setRootPath("");

    fileChooser->documentView->setModel(fileModel);
    fileChooser->documentView->setRootIndex(fileModel->index("/home/meego"));
    connect(fileChooser->documentView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRootIndex(QModelIndex)));

    fileChooser->presentationView->setModel(fileModel);
    fileChooser->presentationView->setRootIndex(fileModel->index("/home/meego"));
    connect(fileChooser->presentationView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRootIndex(QModelIndex)));

    fileChooser->spreadsheetView->setModel(fileModel);
    fileChooser->spreadsheetView->setRootIndex(fileModel->index("/home/meego"));
    connect(fileChooser->spreadsheetView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRootIndex(QModelIndex)));
    fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
    fileModel->setNameFilterDisables(true);
}

void FileChooserDialog::updateRecentDocumentModel()
{
    KConfigGroup config(m_config, "RecentFiles");
    int i = 0;
    QString path;
    do {
        path = config.readPathEntry(QString("File%1").arg(i+1), QString());
        if (!path.isEmpty()) {
            QString name = config.readPathEntry(QString("Name%1").arg(i+1), QString());
            KUrl url(path);
            if (name.isEmpty())
                name = url.fileName();
            QFileInfo fileInfo(path);
            QString iconPath;
            switch (checkFileType(fileInfo.suffix()))
            {
            case WORDS:
                iconPath = ":/images/48x48/Document.png";
                break;
            case STAGE:
                iconPath = ":/images/48x48/Presentation.png";
                break;
            case TABLES:
                iconPath = ":/images/48x48/Spreadsheet.png";
                break;
            }

            recentModel->insertRow(i, QModelIndex());
            recentModel->setData(recentModel->index(i, 0, QModelIndex()), path);
            recentModel->setData(recentModel->index(i, 1, QModelIndex()), name);
            recentModel->setData(recentModel->index(i, 0, QModelIndex()),
                                 QPixmap(iconPath),
                                 Qt::DecorationRole);
        }
        i++;
    } while (!path.isEmpty() || i < 10);
}

void FileChooserDialog::changeRootIndex(QModelIndex index)
{
    QString rootPath = fileModel->filePath(index);
    if ( !fileModel->isDir(index) ) {
        int type = checkFileType(fileModel->fileInfo(index).suffix());
        if (type == fileChooser->fileChooser->currentIndex()) {
            filePath = rootPath;
            accept();
        } else {
            return;
        }
    }
    fileChooser->documentView->setRootIndex(fileModel->index(rootPath));
    fileChooser->presentationView->setRootIndex(fileModel->index(rootPath));
    fileChooser->spreadsheetView->setRootIndex(fileModel->index(rootPath));
    fileChooser->filePath->setText(rootPath);
}

void FileChooserDialog::tabChanged(int index)
{
    switch(index) {
    case 0:
        fileChooser->horizontalLayoutWidget->setVisible(false);
        break;
    case 1:
        fileChooser->horizontalLayoutWidget->setVisible(true);
        fileModel->setNameFilters(QStringList() << "*.doc" << "*.docx" << "*.odt" << "*.txt");
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

    fileChooser->documentView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->presentationView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->spreadsheetView->setRootIndex(fileModel->index(dir.path()));
    fileChooser->filePath->setText(dir.path());
}

int FileChooserDialog::checkFileType(QString ext)
{
    QStringList wordExtensions, stageExtensions, tableExtensions;
    wordExtensions << "doc" << "docx" << "odt" << "txt";
    stageExtensions << "ppt" << "pptx" << "odp" << "pps" << "ppsx";
    tableExtensions << "ods" << "xls" << "xlsx";

    if (wordExtensions.contains(ext))
        return WORDS;
    else if (stageExtensions.contains(ext))
        return STAGE;
    else if (tableExtensions.contains("ext"))
        return TABLES;

    return UNKNOWN;
}

void FileChooserDialog::openRecentDocument(QModelIndex index)
{
    qDebug() << recentModel->index(index.row(), 1).data().toString();
}
