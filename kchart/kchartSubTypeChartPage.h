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

class KDChartParams;

class KChartSubTypeChartPage : public QWidget
{
    Q_OBJECT

public:
    KChartSubTypeChartPage( KDChartParams* params, QWidget* parent ) :
        QWidget( parent ), _params( params ) {}
    virtual void init() = 0;
    virtual void apply() = 0;

protected:
    KDChartParams* _params;
};

class KChartLineSubTypeChartPage : public KChartSubTypeChartPage
{
    Q_OBJECT

public:
    KChartLineSubTypeChartPage( KDChartParams* params,
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
    KChartAreaSubTypeChartPage( KDChartParams* params,
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
    KChartBarSubTypeChartPage( KDChartParams* params, QWidget* parent );
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
    KChartHiloSubTypeChartPage( KDChartParams* params,
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
