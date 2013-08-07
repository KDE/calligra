#ifndef FAKE_KMESSAGEBOX_H
#define FAKE_KMESSAGEBOX_H

#include <QString>
#include <QMessageBox>
#include <QWidget>
#include <kguiitem.h>
#include <kstandardguiitem.h>

class KMessageBox
{
public:

    enum ButtonCode { Ok = 1, Cancel = 2, Yes = 3, No = 4, Continue = 5 };
    enum DialogType { QuestionYesNo = 1, WarningYesNo = 2, WarningContinueCancel = 3, WarningYesNoCancel = 4, Information = 5, Sorry = 7, Error = 8, QuestionYesNoCancel = 9 };
    enum Option { Notify = 1, AllowLink = 2, Dangerous = 4, PlainCaption = 8, NoExec = 16, WindowModal = 32 };

    Q_DECLARE_FLAGS(Options,Option)

    static void error(QWidget *parent, const QString &text, const QString &caption = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Critical, caption, text, QMessageBox::Ok, parent);
        m.exec();
    }

    static void errorList(QWidget *parent, const QString &text, const QStringList &strlist, const QString &caption = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Critical, caption, text, QMessageBox::Ok, parent);
        m.exec();
    }

    static void sorry(QWidget *parent, const QString &text, const QString &caption = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Warning, caption, text, QMessageBox::Ok, parent);
        m.exec();
    }

    static void information(QWidget *parent, const QString &text, const QString &caption = QString(), const QString &dontShowAgainName = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Information, caption, text, QMessageBox::Ok, parent);
        m.exec();
    }

    static int questionYesNo(QWidget *parent, const QString &text, const QString &caption = QString(), const KGuiItem &buttonYes = KStandardGuiItem::yes(), const KGuiItem &buttonNo =  KStandardGuiItem::no(), const QString &dontAskAgainName = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Question, caption, text, QMessageBox::Yes|QMessageBox::No, parent);
        return m.exec();
    }

    static int warningYesNo(QWidget *parent, const QString &text, const QString &caption = QString(), const KGuiItem &buttonYes = KStandardGuiItem::yes(), const KGuiItem &buttonNo = KStandardGuiItem::no(), const QString &dontAskAgainName = QString(), Options options = Options(Notify | Dangerous))
    {
        QMessageBox m(QMessageBox::Warning, caption, text, QMessageBox::Yes|QMessageBox::No, parent);
        return m.exec();
    }

    static int warningYesNoCancel(QWidget *parent, const QString &text, const QString &caption = QString(), const KGuiItem &buttonYes = KStandardGuiItem::yes(), const KGuiItem &buttonNo = KStandardGuiItem::no(), const KGuiItem &buttonCancel = KStandardGuiItem::cancel(), const QString &dontAskAgainName = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Warning, caption, text, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, parent);
        return m.exec();
    }

    static int warningContinueCancel(QWidget *parent, const QString &text, const QString &caption = QString(), const KGuiItem &buttonContinue = KStandardGuiItem::cont(), const KGuiItem &buttonCancel = KStandardGuiItem::cancel(), const QString &dontAskAgainName = QString(), Options options = Notify)
    {
        QMessageBox m(QMessageBox::Warning, caption, text, QMessageBox::Ok|QMessageBox::Cancel, parent);
        return m.exec();
    }

};

Q_DECLARE_OPERATORS_FOR_FLAGS(KMessageBox::Options)

#endif
