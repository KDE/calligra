#ifndef KSpreadPaperBorder_included
#define KSpreadPaperBorder_included

#include <qwidget.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qlabel.h>

class KSpreadPaperBorderData
{
public:
    KSpreadPaperBorderData( QWidget* parent );

    QLineEdit* leftBorder;
    QLineEdit* rightBorder;
    QLineEdit* topBorder;
    QLineEdit* bottomBorder;
    QRadioButton* a3;
    QRadioButton* a4;
    QRadioButton* a5;
    QRadioButton* letter;
    QRadioButton* legal;
    QRadioButton* executive;
};

class KSpreadPaperBorder : public QWidget, public KSpreadPaperBorderData
{
    Q_OBJECT
public:
    KSpreadPaperBorder( QWidget* parent = NULL, const char* name = NULL );

    virtual ~KSpreadPaperBorder();
};

#endif



