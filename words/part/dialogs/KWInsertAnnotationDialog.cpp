#include "KWInsertAnnotationDialog.h"

KWInsertAnnotation::KWInsertAnnotation(QWidget *parent) :
    QWidget(parent)
{
    widget.setupUi(this);
}

QTextDocument *KWInsertAnnotation::textDocument()
{
    return widget.textEdit->document();
}


KWInsertAnnotationDialog::KWInsertAnnotationDialog(QWidget *parent)
        : KDialog(parent)
{
    ui = new KWInsertAnnotation(this);
    setMainWidget(ui);
    setCaption(i18n("Insert Annotation"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    showButtonSeparator(true);
}

QTextDocument *KWInsertAnnotationDialog::annotationTextDocument()
{
    return ui->textDocument();
}
