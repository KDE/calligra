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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#ifndef __KCHARTSUBTYPECHARTPAGE_H__
#define __KCHARTSUBTYPECHARTPAGE_H__

#include <qwidget.h>

class KDChartParams;
class QLabel;
class QRadioButton;

namespace KChart
{

class KChartParams;

class KChartSubTypeChartPage : public QWidget
{
    Q_OBJECT

public:
    KChartSubTypeChartPage( KChartParams* params, QWidget* parent ) :
        QWidget( parent ), _params( params ) {}
    virtual void init() = 0;
    virtual void apply() = 0;

protected:
    KChartParams* _params;
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
    QRadioButton *depth;
    QRadioButton *beside;
    QRadioButton *layer;
    QRadioButton *percent;
    QLabel* exampleLA;
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
