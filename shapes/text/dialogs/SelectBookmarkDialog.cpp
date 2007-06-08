#include "SelectBookmarkDialog.h"

SelectBookmark::SelectBookmark(QList<QString> nameList, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkList->addItems(nameList);
    connect( widget.bookmarkList, SIGNAL( currentRowChanged(int) ), this, SIGNAL( bookmarkSelectionChanged(int) ) );
}

QString SelectBookmark::bookmarkName()
{
    return widget.bookmarkList->currentItem()->text();
}

SelectBookmarkDialog::SelectBookmarkDialog(QList<QString> nameList, QWidget *parent)
    : KDialog(parent)
{
    ui = new SelectBookmark( nameList, this );
    setMainWidget( ui );
    setCaption(i18n("Select Bookmark") );
    setModal( true );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    enableButtonOk( false );
    showButtonSeparator( true );
    connect( ui, SIGNAL( bookmarkSelectionChanged(int) ), this, SLOT( selectionChanged(int) ) );
    // TODO close dialog if one item is double-clicked
    // TODO Rename and Delete
}

QString SelectBookmarkDialog::selectedBookmarkName()
{
    return ui->bookmarkName();
}

void SelectBookmarkDialog::selectionChanged(int currentRow)
{
    if (currentRow != -1)
        enableButtonOk( true );
    else
        enableButtonOk( false );
}

#include <SelectBookmarkDialog.moc>

