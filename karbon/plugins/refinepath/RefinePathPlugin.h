/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REFINEPATHPLUGIN_H
#define REFINEPATHPLUGIN_H

#include <KXMLGUIClient>
#include <QDialog>
#include <QVariantList>

class RefinePathDlg;

class RefinePathPlugin : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    RefinePathPlugin(QObject *parent, const QVariantList &);
    ~RefinePathPlugin() override = default;

private Q_SLOTS:
    void slotRefinePath();

private:
    RefinePathDlg *m_RefinePathDlg;
};

class QSpinBox;

class RefinePathDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RefinePathDlg(QWidget *parent = nullptr, const char *name = nullptr);

    uint knots() const;
    void setKnots(uint value);

private:
    QSpinBox *m_knots;
};

#endif // REFINEPATHPLUGIN_H
