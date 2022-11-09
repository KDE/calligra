/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "selectspecialchardialog.h"
#include <KCharSelect>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>
namespace
{
static const char mySelectSpecialCharDialogConfigGroupName[] = "ConfirmBeforeDeletingDialog";
}

namespace KPIMTextEdit
{
class SelectSpecialCharDialogPrivate
{
public:
    explicit SelectSpecialCharDialogPrivate(SelectSpecialCharDialog *qq)
        : q(qq)
    {
        q->setWindowTitle(i18nc("@title:window", "Select Special Characters"));

        auto lay = new QVBoxLayout(q);

        mCharSelect = new KCharSelect(q, nullptr, KCharSelect::CharacterTable | KCharSelect::BlockCombos);
        q->connect(mCharSelect, &KCharSelect::charSelected, q, &SelectSpecialCharDialog::charSelected);
        lay->addWidget(mCharSelect);

        mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, q);
        QPushButton *okButton = mButtonBox->button(QDialogButtonBox::Ok);
        okButton->setText(i18n("Insert"));
        okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
        lay->addWidget(mButtonBox);
        q->connect(mButtonBox, &QDialogButtonBox::accepted, q, &QDialog::accept);
        q->connect(mButtonBox, &QDialogButtonBox::rejected, q, &QDialog::reject);

        q->connect(okButton, &QPushButton::clicked, q, [this]() {
            _k_slotInsertChar();
        });
    }

    void addSelectButton()
    {
        mSelectButton = new QPushButton(i18n("Select"));
        mButtonBox->addButton(mSelectButton, QDialogButtonBox::ActionRole);
        q->connect(mSelectButton, &QPushButton::clicked, q, [this]() {
            _k_slotInsertChar();
        });
    }

    void _k_slotInsertChar();

    KCharSelect *mCharSelect = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;
    QPushButton *mSelectButton = nullptr;
    SelectSpecialCharDialog *const q;
};

void SelectSpecialCharDialogPrivate::_k_slotInsertChar()
{
    Q_EMIT q->charSelected(mCharSelect->currentChar());
}

SelectSpecialCharDialog::SelectSpecialCharDialog(QWidget *parent)
    : QDialog(parent)
    , d(new SelectSpecialCharDialogPrivate(this))
{
    readConfig();
}

SelectSpecialCharDialog::~SelectSpecialCharDialog()
{
    writeConfig();
}

void SelectSpecialCharDialog::showSelectButton(bool show)
{
    if (show) {
        d->addSelectButton();
    } else {
        d->mButtonBox->removeButton(d->mSelectButton);
    }
}

void SelectSpecialCharDialog::setCurrentChar(QChar c)
{
    d->mCharSelect->setCurrentChar(c);
}

QChar SelectSpecialCharDialog::currentChar() const
{
    return d->mCharSelect->currentChar();
}

void SelectSpecialCharDialog::autoInsertChar()
{
    connect(d->mCharSelect, &KCharSelect::charSelected, this, &SelectSpecialCharDialog::accept);
}

void SelectSpecialCharDialog::setOkButtonText(const QString &text)
{
    d->mButtonBox->button(QDialogButtonBox::Ok)->setText(text);
}

void SelectSpecialCharDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), mySelectSpecialCharDialogConfigGroupName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void SelectSpecialCharDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), mySelectSpecialCharDialogConfigGroupName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
}
}

#include "moc_selectspecialchardialog.cpp"
