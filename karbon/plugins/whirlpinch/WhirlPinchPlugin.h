/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WHIRLPINCHPLUGIN_H
#define WHIRLPINCHPLUGIN_H

#include <KXMLGUIClient>
#include <QDialog>

#include <QVariantList>

class WhirlPinchDlg;

class WhirlPinchPlugin : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    WhirlPinchPlugin(QObject *parent, const QVariantList &);
    ~WhirlPinchPlugin() override = default;

private Q_SLOTS:
    void slotWhirlPinch();

private:
    WhirlPinchDlg *m_whirlPinchDlg;
};

class QDoubleSpinBox;
class KoUnitDoubleSpinBox;
class KoUnit;

class WhirlPinchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit WhirlPinchDlg(QWidget *parent = nullptr, const char *name = nullptr);

    qreal angle() const;
    qreal pinch() const;
    qreal radius() const;
    void setAngle(qreal value);
    void setPinch(qreal value);
    void setRadius(qreal value);
    void setUnit(const KoUnit &unit);

private:
    QDoubleSpinBox *m_angle;
    QDoubleSpinBox *m_pinch;
    KoUnitDoubleSpinBox *m_radius;
};

#endif // WHIRLPINCHPLUGIN_H
