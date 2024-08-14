/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef STYLEMANAGERDIALOG_H
#define STYLEMANAGERDIALOG_H

#include <QDialog>

class StyleManager;

class KoCharacterStyle;
class KoParagraphStyle;
class KoStyleManager;
class KoUnit;
class QCloseEvent;

class StyleManagerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StyleManagerDialog(QWidget *parent);
    ~StyleManagerDialog() override;

    void setStyleManager(KoStyleManager *sm);

    void setUnit(const KoUnit &unit);

public Q_SLOTS:
    void setParagraphStyle(KoParagraphStyle *style);
    void setCharacterStyle(KoCharacterStyle *style, bool canDelete = false);

private Q_SLOTS:
    void slotApplyClicked();

protected:
    void closeEvent(QCloseEvent *e) override;

private:
    void accept() override;
    void reject() override;

    StyleManager *m_styleManagerWidget;
};

#endif
