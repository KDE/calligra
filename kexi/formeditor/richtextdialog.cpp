/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "richtextdialog.h"

#include <KoIcon.h>

#include <QLayout>
#include <QAction>
#include <QVBoxLayout>

#include <ktoolbar.h>
#include <kfontrequester.h>
#include <kcolorcombo.h>
#include <kdebug.h>
#include <klocale.h>

using namespace KFormDesigner;

//////////////////////////////////////////////////////////////////////////////////
//////////////// A simple dialog to edit rich text   ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class RichTextDialog::Private
{
public:
    Private();
    ~Private();

    QAction *fontComboAction, *colorComboAction, *boldTextAction,
    *italicTextAction, *underlineTextAction,
    *subscriptTextAction, *superscriptTextAction,
    *alignLeftAction, *alignRightAction, *alignCenterAction, *alignJustifyAction;
    QActionGroup* alignActionGroup;
    KToolBar  *toolbar;
    KTextEdit  *edit;
    KFontRequester  *fontCombo;
    KColorCombo  *colorCombo;
};

RichTextDialog::Private::Private()
{

}

RichTextDialog::Private::~Private()
{

}

RichTextDialog::RichTextDialog(QWidget *parent, const QString &text)
    : KDialog(parent), d(new Private())
{
    setObjectName("richtext_dialog");
    setModal(true);
    setWindowTitle(i18n("Edit Rich Text"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);

    QFrame *frame = new QFrame(this);
    setMainWidget(frame);
    QVBoxLayout *lyr = new QVBoxLayout(frame);

    d->toolbar = new KToolBar(frame);
    lyr->addWidget(d->toolbar);

    d->fontCombo = new KFontRequester(d->toolbar);
    d->fontComboAction = d->toolbar->addWidget(/*js: TBFont, 40,*/ d->fontCombo);
    connect(d->fontCombo, SIGNAL(textChanged(QString)),
            this, SLOT(changeFont(QString)));

    d->toolbar->addSeparator();

    d->colorCombo = new KColorCombo(d->toolbar);
    d->colorComboAction = d->toolbar->addWidget(/*js: TBColor, 30,*/ d->colorCombo);
    connect(d->colorCombo, SIGNAL(activated(QColor)), this, SLOT(changeColor(QColor)));

    d->boldTextAction = d->toolbar->addAction(koIcon("format-text-bold"), i18n("Bold"));
    d->boldTextAction->setCheckable(true);
    d->italicTextAction = d->toolbar->addAction(koIcon("format-text-italic"), i18n("Italic"));
    d->italicTextAction->setCheckable(true);
    d->underlineTextAction = d->toolbar->addAction(koIcon("format-text-underline"), i18n("Underline"));
    d->underlineTextAction->setCheckable(true);
    d->toolbar->addSeparator();

    d->superscriptTextAction = d->toolbar->addAction(koIcon("format-text-superscript"), i18n("Superscript"));
    d->superscriptTextAction->setCheckable(true);
    d->subscriptTextAction = d->toolbar->addAction(koIcon("format-text-subscript"), i18n("Subscript"));
    d->subscriptTextAction->setCheckable(true);
    d->toolbar->addSeparator();

    d->alignActionGroup = new QActionGroup(this);
    d->alignLeftAction = d->toolbar->addAction(koIcon("format-justify-left"), i18n("Left Align"));
    d->alignLeftAction->setCheckable(true);
    d->alignActionGroup->addAction(d->alignLeftAction);
    d->alignCenterAction = d->toolbar->addAction(koIcon("format-justify-center"), i18n("Centered"));
    d->alignCenterAction->setCheckable(true);
    d->alignActionGroup->addAction(d->alignCenterAction);
    d->alignRightAction = d->toolbar->addAction(koIcon("format-justify-right"), i18n("Right Align"));
    d->alignRightAction->setCheckable(true);
    d->alignActionGroup->addAction(d->alignRightAction);
    d->alignJustifyAction = d->toolbar->addAction(koIcon("format-justify-fill"), i18n("Justified"));
    d->alignJustifyAction->setCheckable(true);
    d->alignActionGroup->addAction(d->alignJustifyAction);

    connect(d->toolbar, SIGNAL(actionTriggered(QAction*)),
            this, SLOT(slotActionTriggered(QAction*)));

    d->edit = new KTextEdit(text, frame);
    lyr->addWidget(d->edit);
    d->edit->setAcceptRichText(true);


    connect(d->edit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(slotCurrentCharFormatChanged(QTextCharFormat)));

    d->edit->moveCursor(QTextCursor::End);
    slotCurrentCharFormatChanged(d->edit->currentCharFormat());
    d->edit->setFocus();
}

RichTextDialog::~RichTextDialog()
{
    delete d;
}

QString
RichTextDialog::text() const
{
    return d->edit->toHtml();
}

void
RichTextDialog::changeFont(const QString &font)
{
    d->edit->setFontFamily(font);
}

void
RichTextDialog::changeColor(const QColor &color)
{
    d->edit->setTextColor(color);
}

void
RichTextDialog::slotActionTriggered(QAction* action)
{
    const bool isChecked = action->isChecked();

    if (action == d->boldTextAction)
        d->edit->setFontWeight(isChecked ? QFont::Bold : QFont::Normal);
    else if (action == d->italicTextAction)
        d->edit->setFontItalic(isChecked);
    else if (action == d->underlineTextAction)
        d->edit->setFontUnderline(isChecked);
    else if (action == d->superscriptTextAction) {
        if (isChecked && d->subscriptTextAction->isChecked()) {
            d->subscriptTextAction->setChecked(false);
            QTextCharFormat currentCharFormat = d->edit->currentCharFormat();
            currentCharFormat.setVerticalAlignment(
                isChecked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);
            d->edit->setCurrentCharFormat(currentCharFormat);
        }
    } else if (action == d->subscriptTextAction) {
        if (isChecked && d->subscriptTextAction->isChecked()) {
            d->subscriptTextAction->setChecked(false);
            QTextCharFormat currentCharFormat = d->edit->currentCharFormat();
            currentCharFormat.setVerticalAlignment(
                isChecked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);
            d->edit->setCurrentCharFormat(currentCharFormat);
        }
    } else if (action == d->alignLeftAction) {
        if (isChecked)
            d->edit->setAlignment(Qt::AlignLeft);
    } else if (action == d->alignCenterAction) {
        if (isChecked)
            d->edit->setAlignment(Qt::AlignCenter);
    } else if (action == d->alignRightAction) {
        if (isChecked)
            d->edit->setAlignment(Qt::AlignRight);
    } else if (action == d->alignJustifyAction) {
        if (isChecked)
            d->edit->setAlignment(Qt::AlignJustify);
    }
}

void
RichTextDialog::slotCurrentCharFormatChanged(const QTextCharFormat& f)
{
    d->superscriptTextAction->setChecked(f.verticalAlignment() == QTextCharFormat::AlignSuperScript);
    d->subscriptTextAction->setChecked(f.verticalAlignment() == QTextCharFormat::AlignSubScript);

    switch (d->edit->alignment()) {
    case Qt::AlignLeft:
        d->alignLeftAction->setChecked(true);
        break;
    case Qt::AlignCenter:
        d->alignCenterAction->setChecked(true);
        break;
    case Qt::AlignRight:
        d->alignRightAction->setChecked(true);
        break;
    case Qt::AlignJustify:
        d->alignJustifyAction->setChecked(true);
        break;
    }

//! @todo add more formatting options (buttons)
}

#include "richtextdialog.moc"
