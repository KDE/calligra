/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GoalSeekDialog.h"

#include "ui/Selection.h"
#include "ui_GoalSeekWidget.h"
#include <ktextedit.h>

#define SELECTOR_COUNT 3

using namespace Calligra::Sheets;

class GoalSeekDialog::Private
{
public:
    Ui::GoalSeekWidget widget;
    Selection *selection;
};

GoalSeekDialog::GoalSeekDialog(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    d->selection = selection;

    setButtons(Apply | Close);
    setButtonText(Apply, i18n("Goal Seek"));
    enableButtonApply(false);
    setModal(false);

    setObjectName(QLatin1String("GoalSeekDialog"));

    setWindowTitle(i18n("Goal Seek"));

    QWidget *mainWidget = new QWidget(this);
    d->widget.setupUi(mainWidget);
    for (int i = 1; i <= SELECTOR_COUNT; ++i) {
        RegionSelector *sel = getSelector(i);
        sel->setDialog(this);
        sel->setSelection(d->selection);
        sel->setSelectionMode(RegionSelector::SingleCell);
        connect(sel->textEdit(), &QTextEdit::textChanged, this, &GoalSeekDialog::textChanged);
    }

    d->widget.outcome->hide();
    setMainWidget(mainWidget);

    connect(this, &KoDialog::closeClicked, this, &GoalSeekDialog::slotClose);
    connect(this, &KoDialog::applyClicked, this, &GoalSeekDialog::slotStart);
}

GoalSeekDialog::~GoalSeekDialog()
{
    delete d;
}

void GoalSeekDialog::slotClose()
{
    accept();
}

void GoalSeekDialog::closeEvent(QCloseEvent *e)
{
    d->selection->endReferenceSelection();
    e->accept();
}

void GoalSeekDialog::textChanged()
{
    d->widget.outcome->hide();
    for (int i = 1; i <= SELECTOR_COUNT; ++i) {
        if (getSelector(i)->textEdit()->toPlainText().isEmpty()) {
            enableButtonApply(false);
            return;
        }
    }
    enableButtonApply(true);
}

RegionSelector *GoalSeekDialog::getSelector(int id)
{
    if (id == 1)
        return d->widget.selector1;
    if (id == 2)
        return d->widget.selector2;
    if (id == 3)
        return d->widget.selector3;
    return nullptr;
}

QString GoalSeekDialog::selectorValue(int id)
{
    RegionSelector *sel = getSelector(id);
    if (!sel)
        return QString();
    return sel->textEdit()->toPlainText();
}

void GoalSeekDialog::focusSelector(int id)
{
    RegionSelector *sel = getSelector(id);
    if (!sel)
        return;

    sel->textEdit()->selectAll();
    sel->textEdit()->setFocus();
}

void GoalSeekDialog::setNotice(const QString &text)
{
    if (text.length())
        d->widget.outcome->show();
    else
        d->widget.outcome->hide();

    d->widget.label4->setText(text);
}

void GoalSeekDialog::slotStart()
{
    Q_EMIT calculate();
}
