#include "CreateNewBookmarkDialog.h"

CreateNewBookmark::CreateNewBookmark(QList<QString> nameList, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkName->insertItems(0, nameList);
    widget.bookmarkName->clearEditText();
    connect( widget.bookmarkName, SIGNAL( editTextChanged(const QString &) ), this, SIGNAL( bookmarkNameChanged(const QString &) ) );
}

QString CreateNewBookmark::bookmarkName()
{
    return widget.bookmarkName->currentText();
}

CreateNewBookmarkDialog::CreateNewBookmarkDialog(QList<QString> nameList, QWidget *parent)
    : KDialog(parent)
{
    ui = new CreateNewBookmark( nameList, this );
    setMainWidget( ui );
    setCaption(i18n("Create New Bookmark") );
    setModal( true );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    enableButtonOk( false );
    showButtonSeparator( true );
    connect( ui, SIGNAL( bookmarkNameChanged(const QString &) ), this, SLOT( nameChanged(const QString &) ) );
}

QString CreateNewBookmarkDialog::newBookmarkName()
{
    return ui->bookmarkName();
}

void CreateNewBookmarkDialog::nameChanged(const QString &bookmarkName)
{
    enableButtonOk( !bookmarkName.isEmpty() );
}

#include <CreateNewBookmarkDialog.moc>

