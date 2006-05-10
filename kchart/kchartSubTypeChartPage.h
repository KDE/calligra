/* This file is part of the KDE project
   Copyright (C) 1999,2000 Matthias Kalle Dalheimer <kalle@kde.org>

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


#ifndef __KCHARTSUBTYPECHARTPAGE_H__
#define __KCHARTSUBTYPECHARTPAGE_H__

#include <QWidget>
//Added by qt3to4:
#include <QLabel>

class KDChartParams;
class QLabel;
class QRadioButton;
class QSpinBox;

namespace KChart
{

class KChartParams;

class KChartSubTypeChartPage : public QWidget
{
    Q_OBJECT

public:
    KChartSubTypeChartPage( KChartParams* params, QWidget* parent ) :
        QWidget( parent ), m_params( params ) {}
    virtual void init() = 0;
    virtual void apply() = 0;

protected:
    KChartParams* m_params;
};

class KChartLineSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartLineSubTypeChartPage( KChartParams* params,
                                QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton* normal;
    QRadioButton* stacked;
    QRadioButton* percent;
    QLabel* exampleLA;
};


class KChartAreaSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartAreaSubTypeChartPage( KChartParams* params,
                                QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton* normal;
    QRadioButton* stacked;
    QRadioButton* percent;
    QLabel* exampleLA;
};

class KChartBarSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartBarSubTypeChartPage( KChartParams* params, QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton  *normal;
    QRadioButton  *stacked;
    QRadioButton  *percent;
    QLabel        *exampleLA;
    QSpinBox      *m_numLines;
};


class KChartHiloSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartHiloSubTypeChartPage( KChartParams* params,
                                QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton* normal;
    QRadioButton* stacked;
    QRadioButton* percent;
    QLabel* exampleLA;
};

class KChartPolarSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartPolarSubTypeChartPage( KChartParams* params,
                                QWidget* parent );
    virtual void init();
    virtual void apply();

private slots:
    void slotChangeSubType( int type );

private:
    QRadioButton* normal;
    QRadioButton* stacked;
    QRadioButton* percent;
    QLabel* exampleLA;
};

}  //KChart namespace

#endif
