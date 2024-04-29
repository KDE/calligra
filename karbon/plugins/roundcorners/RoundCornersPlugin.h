/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ROUNDCORNERSPLUGIN_H
#define ROUNDCORNERSPLUGIN_H

#include <KXMLGUIClient>
#include <QDialog>
#include <QVariantList>

class RoundCornersDlg;

class RoundCornersPlugin : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    RoundCornersPlugin(QObject *parent, const QVariantList &);
    ~RoundCornersPlugin() override;

private Q_SLOTS:
    void slotRoundCorners();

private:
    RoundCornersDlg *m_roundCornersDlg;
};

class KoUnitDoubleSpinBox;
class KoUnit;

class RoundCornersDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RoundCornersDlg(QWidget *parent = nullptr, const char *name = nullptr);

    qreal radius() const;
    void setRadius(qreal value);
    void setUnit(const KoUnit &unit);

private:
    KoUnitDoubleSpinBox *m_radius;
};

#endif // ROUNDCORNERSPLUGIN_H
