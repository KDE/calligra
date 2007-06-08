#ifndef CREATENEWBOOKMARKDIALOG_H
#define CREATENEWBOOKMARKDIALOG_H

#include "ui_CreateNewBookmark.h"

#include <QWidget>
#include <KDialog>

class CreateNewBookmark : public QWidget {
    Q_OBJECT
public:
    CreateNewBookmark(QList<QString> nameList, QWidget *parent = 0);
    QString bookmarkName();

signals:
    void bookmarkNameChanged(const QString &name);

private:
    Ui::CreateNewBookmark widget;
};

class CreateNewBookmarkDialog : public KDialog {
    Q_OBJECT
public:
    CreateNewBookmarkDialog(QList<QString> nameList, QWidget *parent = 0);
    QString newBookmarkName();

private slots:
    void nameChanged(const QString &name);
private:
    CreateNewBookmark *ui;
};

#endif

