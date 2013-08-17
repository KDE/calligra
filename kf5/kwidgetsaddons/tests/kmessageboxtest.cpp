#undef QT_NO_CAST_FROM_ASCII

#include "kmessagebox.h"
#include <QLabel>

#include <stdlib.h>
#include <stdio.h>

#include <qapplication.h>

class ExampleWidget : public QLabel
{
public:
    ExampleWidget( QWidget *parent = 0 );
};

ExampleWidget::ExampleWidget( QWidget *parent )
    : QLabel(parent )
{
    // Make the top-level layout; a vertical box to contain all widgets
    // and sub-layouts.
    QSize sh;
    setText("<p>Hello.</p>");
    sh = sizeHint();
    qWarning("SizeHint = %d x %d", sh.width(), sh.height());
    setText("Hello.");
    sh = sizeHint();
    qWarning("SizeHint = %d x %d", sh.width(), sh.height());
    setText("<p>Hello<br>World</p>");
    sh = sizeHint();
    qWarning("SizeHint = %d x %d", sh.width(), sh.height());
//    setText("Hello\nWorld");
    sh = sizeHint();
    qWarning("SizeHint = %d x %d", sh.width(), sh.height());
    setMinimumSize(sizeHint());
}


class Foo: public QDialog
{
 public:
   Foo() : QDialog(0) {
	   setObjectName("foo");
	   setModal(true);
	   resize(200,200);
	   new QLabel("Hello World", this);
	   show();
   }
};

void showResult(int test, int i)
{
    printf("Test %d. returned %d ", test, i);
    switch( i) {
    case KMessageBox::Ok : printf("(%s)\n", "Ok"); break;
    case KMessageBox::Cancel : printf("(%s)\n", "Cancel"); break;
    case KMessageBox::Yes : printf("(%s)\n", "Yes"); break;
    case KMessageBox::No : printf("(%s)\n", "No"); break;
    case KMessageBox::Continue : printf("(%s)\n", "Continue"); break;
    default: printf("(%s)\n", "ERROR!"); exit(1);
    }
}

bool testMessageBox(int test)
{
    QStringList list; list.append("Hello"); list.append("World");

    int i;
    switch(test)
    {
    case 1:
    {
        ExampleWidget *w = new ExampleWidget();
        w->show();

        i = KMessageBox::warningContinueCancel(w,
                                               "You are about to <Print>.\n"
                                               "Are you sure?",
                                               "Print", KGuiItem( QLatin1String("&Print") ), KStandardGuiItem::cancel(), "dontask");
        i = KMessageBox::warningContinueCancel(0,
                                               "You are about to <Print>.\n"
                                               "Are you sure?",
                                               "Print", KGuiItem( QLatin1String("&Print") ), KStandardGuiItem::cancel(), "dontask", KMessageBox::AllowLink);
        i = KMessageBox::questionYesNo(0, "<p>Do you have a printer? thisisaverylongdkldhklghklghklashgkllasghkdlsghkldfghklsabla bla bbla bla. It also has <a href=http://www.kde.org>this URL</a>.</p>",
                                       QString("Bla"), KGuiItem(QString("Yes")), KGuiItem(QString("No")), "bla", KMessageBox::AllowLink);
        break;
    }
    case 2:
        i = KMessageBox::questionYesNo(0, "Do you have a printer?",
                                       QString("Printer setup"));
        break;

    case 3:
        i = KMessageBox::questionYesNo(0,
                                       "Does your printer support color or only black and white?",
                                       "Printer setup", KGuiItem( QLatin1String("&Color") ), KGuiItem(QString::fromLatin1("&Black & White")));
        break;

    case 4:
        i = KMessageBox::warningYesNo(0,
                                      "KDVI could not locate the program 'dvipdfm' on your computer. That program is "
                                      "absolutely needed by the export function. You can, however, convert "
                                      "the DVI-file to PDF using the print function of KDVI, but that will often "
                                      "produce files which print ok, but are of inferior quality if viewed in the "
                                      "Acrobat Reader. It may be wise to upgrade to a more recent version of your "
                                      "TeX distribution which includes the 'dvipdfm' program.\n"
                                      "Hint to the perplexed system administrator: KDVI uses the shell's PATH variable "
                                      "when looking for programs."
            );
        break;


    case 5:
        i = KMessageBox::warningYesNo(0, "Your printer has been added.\n"
                                      "Do you want to update your configuration?",
                                      "Printer Setup");
        break;

    case 6:
        i = KMessageBox::warningContinueCancel(0,
                                               "You are about to print.\n"
                                               "Are you sure?",
                                               "Print", KGuiItem( QLatin1String("&Print") ) );
        break;
    case 7:
        i = KMessageBox::warningContinueCancel(0,
                                               "You are about to <Print>.\n"
                                               "Are you sure?",
                                               "Print", KGuiItem( QLatin1String("&Print") ), KStandardGuiItem::cancel(), "dontask");
        i = KMessageBox::warningContinueCancel(0,
                                               "You are about to <Print>.\n"
                                               "Are you sure?",
                                               "Print", KGuiItem( QLatin1String("&Print") ), KStandardGuiItem::cancel(), "dontask");
        break;

    case 8:
        i = KMessageBox::warningYesNoCancel(0,
                                            "Your document contains unsaved changes.\n"
                                            "Do you want to save your changes?\n");
        break;

    case 9:
        i = KMessageBox::warningYesNoCancel(0,
                                            "Your document contains unsaved changes.\n"
                                            "Do you want to save your changes?\n",
                                            QLatin1String("Close"));
        break;

    case 10:
        i = KMessageBox::warningYesNoCancel(0,
                                            "Your document contains unsaved changes.\n"
                                            "Do you want to save or discard your changes?\n",
                                            "Close", KGuiItem( QLatin1String("&Save") ), KGuiItem( QLatin1String("&Discard") ) );
        break;

    case 11:
        i = KMessageBox::Ok;
        KMessageBox::error(0, "Oops, Your harddisk is unreadable.");
        break;

    case 12:
        i = KMessageBox::Ok;
        KMessageBox::detailedError(0, "Oops, Your harddisk is unreadable." , "We don't know more yet.", "Uh ooh");
        break;

    case 13:
        i = KMessageBox::Ok;
        KMessageBox::sorry(0, "Sorry, Your harddisk appears to be empty.");
        break;

    case 14:
        i = KMessageBox::Ok;
        KMessageBox::detailedSorry(0, "Sorry, Your harddisk appears to be empty.", "We don't know more yet.", "Oops");
        break;

    case 15:
        i = KMessageBox::Ok;
        KMessageBox::information(0, "You can enable the menubar again "
                                 "with the right mouse button menu.");
        break;

    case 16:
        i = KMessageBox::Ok;
        KMessageBox::information(0, "You can enable the menubar again "
                                 "with the right mouse button menu.", "Menubar Info");
        break;

    case 17:
        i = KMessageBox::Ok;
        KMessageBox::information(0, "You can enable the menubar again\nwith the right mouse button menu.", QString(), "Enable_Menubar");
        break;

    case 18:
        i = KMessageBox::Ok;
        KMessageBox::enableAllMessages();
        break;

    case 19:
        i = KMessageBox::Ok;
        KMessageBox::information(0, "Return of the annoying popup message.", QString(), "Enable_Menubar");
        break;
    case 20:
    {
        QStringList strlist;
        strlist << "/dev/hda" << "/etc/inittab" << "/usr/somefile" << "/some/really/"
            "long/file/name/which/is/in/a/really/deep/directory/in/a/really/large/"
            "hard/disk/of/your/system" << "/and/another/one" ;
        i = KMessageBox::questionYesNoList(0, "Do you want to delete the following files?",strlist);
    }
    break;
    case 21:
    {
        QStringList strlist;
        printf("Filling StringList...\n");
        for (int j=1;j<=6000;j++) strlist.append(QString("/tmp/tmp.%1").arg(j));
        printf("Completed...\n");
        i = KMessageBox::questionYesNoList(0, "Do you want to delete the following files?",strlist);
    }
    break;

    case 22:
        i = KMessageBox::Ok;
        KMessageBox::informationList(0, "The following words have been found:",list);
        break;

    case 23:
        i = KMessageBox::Ok;
        KMessageBox::informationList(0, "The following words have been found:", list, "Search Words");
        break;

    case 24:
        i = KMessageBox::Ok;
        KMessageBox::informationList(0, "The follwoing words have been found:", list, QString(), "Search_Words");
        break;

    default:
        return false; // done
    } // Switch

    showResult(test, i);
    return true;
}


int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);
    app.setApplicationName("kmessageboxtest");

    // Syntax: kmessageboxtest <test_number>
    if (argc > 1) {
        testMessageBox(QByteArray(argv[1]).toInt());
        return 0;
    }

    // No argument specified, run all tests
    int test = 1;
    while (++test) {
        if (!testMessageBox(test))
            break;
    }
    return 0;
}





#if 0
//this is my sequence for testing messagebox layout:

    KMessageBox::questionYesNoCancel(
    0, "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd", "long",
        KStandardGuiItem::saveAs(), KGuiItem("dsdddddd"), KStandardGuiItem::cancel()
        );
    KMessageBox::questionYesNoCancel(
    0, "ddddddddddddddddddddd ddddddddddddddddddddd dddddddddd dddddddddd ddddddddddddddddddd dddddddddddd ddddddddd", "long wrap",
        KStandardGuiItem::saveAs(), KGuiItem("dsdddddd"), KStandardGuiItem::cancel()
        );

    KMessageBox::questionYesNoCancel(
    0, "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd", "height",
        KStandardGuiItem::saveAs(), KGuiItem("dsdddddd"), KStandardGuiItem::cancel()
        );

    QStringList strlist;
    strlist<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfgh\nghghgfhgf"<<"f\ngfg\nhg\nhghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf"<<"fgfghghghgfhgf";
   KMessageBox::errorList(0,
		"short\n",
		strlist,"short");
   KMessageBox::errorList(0,
		"dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
		strlist,"short");
   KMessageBox::errorList(0,
		"ddddddddddddddddddddd ddddddddddddddddddddd dddddddddd dddddddddd ddddddddddddddddddd dddddddddddd ddddddddd",
		strlist,"short");
   KMessageBox::errorList(0,
		"dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd",
		strlist,"short");

  KMessageBox::detailedError(0,
                    "sss",
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                   );
  KMessageBox::detailedError(0,
                    "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                   );
  KMessageBox::detailedError(0,
                    "ddddddddddddddddddddd ddddddddddddddddddddd dddddddddd dddddddddd ddddddddddddddddddd dddddddddddd ddddddddd",
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                   );
  KMessageBox::detailedError(0,
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd",
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
            "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                    "dddddd\ndddddd\nddddddddd ddddd\ndddd\ndddddddddddd \ndddddddddd dddddddddd dd\nddddddddddd\ndd\ndddd dddd\ndddddddd ddd\ndd\ndddd"
                   );

#endif
