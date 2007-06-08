#ifndef SELECTBOOKMARKDIALOG_H
#define SELECTBOOKMARKDIALOG_H

#include "ui_SelectBookmark.h"

#include <QWidget>
#include <KDialog>

class SelectBookmark : public QWidget {
    Q_OBJECT
public:
    SelectBookmark(QList<QString> nameList, QWidget *parent = 0);
    QString bookmarkName();

signals:
    void bookmarkSelectionChanged(int currentRow);

private:
    Ui::SelectBookmark widget;
};

class SelectBookmarkDialog : public KDialog {
    Q_OBJECT
public:
    SelectBookmarkDialog(QList<QString> nameList, QWidget *parent = 0);
    QString selectedBookmarkName();

private slots:
    void selectionChanged(int currentRow);

private:
    SelectBookmark *ui;
};

#endif

