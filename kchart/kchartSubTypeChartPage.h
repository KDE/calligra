/*
 * Copyright 1999-2000 by Matthias Kalle Dalheimer <kalle@kde.org>,
 *   released under Artistic License
 */

#ifndef __KCHARTSUBTYPECHARTPAGE_H__
#define __KCHARTSUBTYPECHARTPAGE_H__

#include <qwidget.h>

class KDChartParams;
class QLabel;
class QRadioButton;
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


#endif
