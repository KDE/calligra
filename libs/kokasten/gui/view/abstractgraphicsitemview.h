#ifndef ABSTRACTGRAPHICSITEMVIEW_H
#define ABSTRACTGRAPHICSITEMVIEW_H

// lib
#include "kastengui_export.h"

// Kasten core
#include <abstractdocument.h>
#include <abstractview.h>

class QGraphicsItem;

namespace Kasten2
{

// TODO: is there a common base for view and document?
class KASTENGUI_EXPORT AbstractGraphicsItemView : public AbstractView
{
    Q_OBJECT

  public:
    explicit AbstractGraphicsItemView( AbstractModel* baseModel = 0 );
    virtual ~AbstractGraphicsItemView();

  public: // API to be implemented
    virtual void setFocus() = 0;

    virtual QGraphicsItem* widget() const = 0;
    virtual bool hasFocus() const = 0;

  Q_SIGNALS:
    // view has focus in the window
    void focusChanged( bool hasFocus );
};

}

#endif
