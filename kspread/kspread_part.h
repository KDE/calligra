#ifndef __kspread_part_h__
#define __kspread_part_h__

class KSpreadView;
class KSpread;
class KSpreadTable;
class ChartPart;

#include "kspread_chart_impl.h"

#undef __Callback_predef__

#include "kspread_table.h"

#include <qrect.h>

#include <part_impl.h>
#include <shell.h>
#include <opstore.h>

/**
 */
class ChartCellBinding : public CellBinding
{
    Q_OBJECT
public:
    ChartCellBinding( KSpreadTable *_table, const QRect &_rect, CHART::Chart_ptr _part );
    
    virtual void cellChanged( Object *_obj );

protected:
    CHART::Chart_ptr pChart;
};

class KSpreadPart : virtual public Part_impl
{
public:
    KSpreadPart( Shell *_shell );
    ~KSpreadPart();

    Shell_ptr shell() { return Shell::_duplicate( (Shell_ptr)vShell ); }

    CORBA::Boolean save( const char *_url, CORBA::Boolean _append );
    CORBA::Boolean load( const char *_url, CORBA::Long _offset, CORBA::Long _size );

    char* mimeType() { return "application/x-kde-kxcl"; }
    char* editorType() { return "IDL:KSpreadFactory:1.0"; }

    OBJECT writeToStore(Store &_store );
    bool readFromStore (Store &_store, OBJECT _obj );

protected:
    KSpreadView *pGui;
    KSpread *pKKSpread;
};

#endif
