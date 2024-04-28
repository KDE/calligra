/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
              SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
              SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
              SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
              SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
              SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
              SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
              SPDX-FileCopyrightText: 2002 Joseph Wenninger <jowenn@kde.org>
              SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
              SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>
              SPDX-FileCopyrightText: 2007 Clarence Dang <dang@kde.org>
              SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>

    SPDX-License-Identifier: LGPL-2.0-only
*/

// This is a minorly modified version of the KFontAction class. It exists
// entirely because there's a hang bug on windows at the moment.

#include "FontFamilyAction.h"

#include "TextShapeDebug.h"

#include <QToolBar>

#include <KLocalizedString>
#include <KoFontComboBox.h>
#include <QIcon>
#include <kfontchooser.h>

class KoFontFamilyAction::KoFontFamilyActionPrivate
{
public:
    KoFontFamilyActionPrivate(KoFontFamilyAction *parent)
        : q(parent)
        , settingFont(0)
    {
    }

    void _ko_slotFontChanged(const QFont &font)
    {
        debugTextShape << "KoFontComboBox - slotFontChanged(" << font.family() << ") settingFont=" << settingFont;
        if (settingFont)
            return;

        q->setFont(font.family());
        q->textTriggered(font.family());

        debugTextShape << "\tslotFontChanged done";
    }

    KoFontFamilyAction *q;
    int settingFont;
};

KoFontFamilyAction::KoFontFamilyAction(uint fontListCriteria, QObject *parent)
    : KSelectAction(parent)
    , d(new KoFontFamilyActionPrivate(this))
{
    QStringList list = KFontChooser::createFontList(fontListCriteria);
    KSelectAction::setItems(list);
    setEditable(true);
}

KoFontFamilyAction::KoFontFamilyAction(QObject *parent)
    : KSelectAction(parent)
    , d(new KoFontFamilyActionPrivate(this))
{
    QStringList list = KFontChooser::createFontList(0);
    KSelectAction::setItems(list);
    setEditable(true);
}

KoFontFamilyAction::KoFontFamilyAction(const QString &text, QObject *parent)
    : KSelectAction(text, parent)
    , d(new KoFontFamilyActionPrivate(this))
{
    QStringList list = KFontChooser::createFontList(0);
    KSelectAction::setItems(list);
    setEditable(true);
}

KoFontFamilyAction::KoFontFamilyAction(const QIcon &icon, const QString &text, QObject *parent)
    : KSelectAction(icon, text, parent)
    , d(new KoFontFamilyActionPrivate(this))
{
    QStringList list = KFontChooser::createFontList(0);
    KSelectAction::setItems(list);
    setEditable(true);
}

KoFontFamilyAction::~KoFontFamilyAction()
{
    delete d;
}

QString KoFontFamilyAction::font() const
{
    return currentText();
}

QWidget *KoFontFamilyAction::createWidget(QWidget *parent)
{
    debugTextShape << "KoFontFamilyAction::createWidget()";
    // silence unclear warning from original kfontaction.acpp
    // #ifdef __GNUC__
    // #warning FIXME: items need to be converted
    // #endif
    // This is the visual element on the screen.  This method overrides
    // the KSelectAction one, preventing KSelectAction from creating its
    // regular KComboBox.
    KoFontComboBox *cb = new KoFontComboBox(parent);
    // cb->setFontList(items());

    debugTextShape << "\tset=" << font();
    // Do this before connecting the signal so that nothing will fire.
    cb->setCurrentFont(QFont(font().toLower()));
    debugTextShape << "\tspit back=" << cb->currentFont().family();

    connect(cb, &KoFontComboBox::currentFontChanged, this, [this](const QFont &ft) {
        d->_ko_slotFontChanged(ft);
    });
    cb->setMinimumWidth(cb->sizeHint().width());
    return cb;
}

/*
 * Maintenance note: Keep in sync with KFontComboBox::setCurrentFont()
 */
void KoFontFamilyAction::setFont(const QString &family)
{
    debugTextShape << "KoFontFamilyAction::setFont(" << family << ")";

    // Suppress triggered(QString) signal and prevent recursive call to ourself.
    d->settingFont++;

    foreach (QWidget *w, createdWidgets()) {
        KoFontComboBox *cb = qobject_cast<KoFontComboBox *>(w);
        debugTextShape << "\tw=" << w << "cb=" << cb;

        if (!cb)
            continue;

        cb->setCurrentFont(QFont(family.toLower()));
        debugTextShape << "\t\tw spit back=" << cb->currentFont().family();
    }

    d->settingFont--;

    debugTextShape << "\tcalling setCurrentAction()";

    QString lowerName = family.toLower();
    if (setCurrentAction(lowerName, Qt::CaseInsensitive))
        return;

    int i = lowerName.indexOf(" [");
    if (i > -1) {
        lowerName = lowerName.left(i);
        i = 0;
        if (setCurrentAction(lowerName, Qt::CaseInsensitive))
            return;
    }

    lowerName += " [";
    if (setCurrentAction(lowerName, Qt::CaseInsensitive))
        return;

    // TODO: Inconsistent state if KFontComboBox::setCurrentFont() succeeded
    //       but setCurrentAction() did not and vice-versa.
    debugTextShape << "Font not found " << family.toLower();
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_FontFamilyAction.cpp"
