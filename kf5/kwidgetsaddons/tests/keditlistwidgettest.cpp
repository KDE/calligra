#include <qapplication.h>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <keditlistwidget.h>
#include <QDebug>

int main( int argc, char **argv )
{
    QApplication app(argc, argv);

#if 0
    KEditListWidget::CustomEditor editor( new KComboBox( true, 0 ) );
    KEditListWidget *box = new KEditListWidget( editor );

    box->insertItem( QStringLiteral("Test") );
    box->insertItem( QStringLiteral("for") );
    box->insertItem( QStringLiteral("this") );
    box->insertItem( QStringLiteral("KEditListWidget") );
    box->insertItem( QStringLiteral("Widget") );
    box->show();

#else

    // code from kexi
    QStringList list; list << QStringLiteral("one") << QStringLiteral("two");
    QDialog dialog;
    dialog.setObjectName(QStringLiteral("stringlist_dialog"));
    dialog.setModal(true);
    dialog.setWindowTitle(QStringLiteral("Edit List of Items"));

    KEditListWidget *edit = new KEditListWidget(&dialog);
    edit->setObjectName(QStringLiteral("editlist"));
    edit->insertStringList(list);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(&dialog);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(edit);
    layout->addWidget(buttonBox);
    dialog.setLayout(layout);

    if (dialog.exec() == QDialog::Accepted) {
        list = edit->items();
        qDebug() << list;
    }
#endif

    return app.exec();
}
