#ifndef ABSTRACTWIDGETVIEW_H
#define ABSTRACTWIDGETVIEW_H

// lib
#include "kastengui_export.h"

// Kasten core
#include <abstractdocument.h>
#include <abstractview.h>

class QWidget;

namespace Kasten2
{

// TODO: is there a common base for view and document?
class KASTENGUI_EXPORT AbstractWidgetView : public AbstractView
{
    Q_OBJECT

  public:
    explicit AbstractWidgetView( AbstractModel* baseModel = 0 );
    virtual ~AbstractWidgetView();

  public: // API to be implemented
    virtual void setFocus() = 0;

    virtual QWidget* widget() const = 0;
    virtual bool hasFocus() const = 0;

  Q_SIGNALS:
    // view has focus in the window
    void focusChanged( bool hasFocus );
};

}

#endif
