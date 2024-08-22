/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FLATTENPATHPLUGIN_H
#define FLATTENPATHPLUGIN_H

#include <KXMLGUIClient>
#include <QDialog>

class FlattenDlg;

class FlattenPathPlugin : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    FlattenPathPlugin(QObject *parent, const QVariantList &);
    ~FlattenPathPlugin() override = default;

private Q_SLOTS:
    void slotFlattenPath();

private:
    FlattenDlg *m_flattenPathDlg;
};

class QDoubleSpinBox;

class FlattenDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FlattenDlg(QWidget *parent = nullptr, const char *name = nullptr);

    qreal flatness() const;
    void setFlatness(qreal value);

private:
    QDoubleSpinBox *m_flatness;
};

#endif
