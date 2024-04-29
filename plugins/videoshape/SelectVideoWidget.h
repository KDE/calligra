/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SELECTVIDEOWIDGET_H
#define SELECTVIDEOWIDGET_H

#include <QWidget>

class KFileWidget;
class QUrl;
class QCheckBox;

class SelectVideoWidget : public QWidget
{
public:
    explicit SelectVideoWidget(QWidget *parent = nullptr);
    ~SelectVideoWidget();

    QUrl selectedUrl() const;
    bool saveEmbedded();

    void accept();
    void cancel();

private:
    KFileWidget *m_fileWidget;
    QCheckBox *m_saveEmbedded;
};

#endif // SELECTVIDEOWIDGET_H
