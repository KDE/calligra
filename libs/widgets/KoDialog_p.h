/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only

*/

#ifndef KODIALOG_P_H
#define KODIALOG_P_H

#include "KoDialog.h"
#include <QHash>
#include <QPointer>
#include <QSize>

class QBoxLayout;
class QPushButton;
class KUrlLabel;
class KSeparator;
class QDialogButtonBox;

class KoDialogPrivate
{
    Q_DECLARE_PUBLIC(KoDialog)
protected:
    KoDialogPrivate()
        : mDetailsVisible(false)
        , mSettingDetails(false)
        , mDeferredDelete(false)
        , mDetailsWidget(nullptr)
        , mTopLayout(nullptr)
        , mMainWidget(nullptr)
        , mUrlHelp(nullptr)
        , mActionSeparator(nullptr)
        , mButtonOrientation(Qt::Horizontal)
        , mDefaultButton(KoDialog::NoDefault)
        , mButtonBox(nullptr)
    {
    }

    virtual ~KoDialogPrivate() = default;

    KoDialog *q_ptr;

    void setupLayout();
    void appendButton(KoDialog::ButtonCode code, const KGuiItem &item);

    bool mDetailsVisible;
    bool mSettingDetails;
    bool mDeferredDelete;
    QWidget *mDetailsWidget;
    QSize mIncSize;
    QSize mMinSize;
    QString mDetailsButtonText;

    QBoxLayout *mTopLayout;
    QPointer<QWidget> mMainWidget;
    KUrlLabel *mUrlHelp;
    KSeparator *mActionSeparator;

    QString mAnchor;
    QString mHelpApp;
    QString mHelpLinkText;

    Qt::Orientation mButtonOrientation;
    KoDialog::ButtonCode mDefaultButton;
    KoDialog::ButtonCode mEscapeButton;

    QDialogButtonBox *mButtonBox;
    QHash<int, QPushButton *> mButtonList;

protected Q_SLOTS:
    void queuedLayoutUpdate();
    void helpLinkClicked();

private:
    void init(KoDialog *);
    bool dirty : 1;
};

#endif // KDEUI_KDIALOG_P_H
