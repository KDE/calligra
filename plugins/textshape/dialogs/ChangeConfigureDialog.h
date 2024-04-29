/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __CHANGE_CONFIGURE_DIALOG_H__
#define __CHANGE_CONFIGURE_DIALOG_H__

#include <KoChangeTracker.h>
#include <QLabel>

class ColorDisplayLabel : public QLabel
{
public:
    explicit ColorDisplayLabel(QWidget *parent = nullptr);
    ~ColorDisplayLabel() override;
    void paintEvent(QPaintEvent *event) override;
    const QColor &color() const;
    void setColor(const QColor &color);

private:
    QColor labelColor;
};

#include <ui_ChangeConfigureDialog.h>

class ChangeConfigureDialog : public QDialog
{
    Q_OBJECT

    typedef enum { eInsert, eDelete, eFormatChange, eChangeTypeNone } ChangeType;

public:
    ChangeConfigureDialog(const QColor &insertionColor,
                          const QColor &deletionColor,
                          const QColor &formatChangeColor,
                          const QString &authorName,
                          KoChangeTracker::ChangeSaveFormat changeSaveFormat,
                          QWidget *parent = nullptr);
    ~ChangeConfigureDialog();

    const QColor &getInsertionBgColor();
    const QColor &getDeletionBgColor();
    const QColor &getFormatChangeBgColor();
    const QString authorName();
    KoChangeTracker::ChangeSaveFormat saveFormat();

private:
    Ui::ChangeConfigureDialog ui;
    void updatePreviewText();
    void colorSelect(ChangeType type);

private Q_SLOTS:
    void insertionColorSelect();
    void deletionColorSelect();
    void formatChangeColorSelect();
};
#endif
