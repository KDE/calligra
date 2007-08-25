#ifndef _KCWIZARDCHARTTYPEPAGE_H
#define _KCWIZARDCHARTTYPEPAGE_H


#include <QWidget>
#include <QPixmap>

#include <kvbox.h>

#include "KoShapeConfigWidgetBase.h"

class QGridLayout;
class QPushButton;
class QButtonGroup;

class KoShape;

namespace KChart
{
class ChartShape;

/**
 * Chart type configuration widget.
 */
class ChartTypeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT

public:
    ChartTypeConfigWidget();
    ~ChartTypeConfigWidget();

    void open( KoShape* shape );
    void save();
    KAction* createAction();

    /// reimplemented 
    virtual bool showOnShapeCreate() { return true; }

public slots:
    void apply();

private slots:
    void chartTypeSelected( int type );

signals:
    void chartChange(int);

private:
    class Private;
    Private * const d;
};

}  //namespace KChart

#endif
