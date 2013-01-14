#ifndef KWINSERTANNOTATIONDIALOG_H
#define KWINSERTANNOTATIONDIALOG_H

#include "ui_KWInsertAnnotationDialog.h"
#include <QWidget>
#include <KDialog>

class QTextDocument;

/// This class is just a temporary class to handle inser annotaion.
class KWInsertAnnotation : public QWidget
{
    Q_OBJECT
    
public:
    explicit KWInsertAnnotation(QWidget *parent = 0);
    QTextDocument *textDocument();

private:
    Ui::KWInsertAnnotation widget;
};

class KWInsertAnnotationDialog : public KDialog
{
    Q_OBJECT
public:
    explicit KWInsertAnnotationDialog(QWidget *parent = 0);

    QTextDocument *annotationTextDocument();

private:
    KWInsertAnnotation *ui;
};


#endif // KWINSERTANNOTATIONDIALOG_H
