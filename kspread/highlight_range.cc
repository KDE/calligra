#include "highlight_range.h"

HighlightRange::HighlightRange(const HighlightRange& rhs) 
		{	
			rhs._firstCell ? _firstCell=new KSpreadPoint(*(rhs._firstCell )) : _firstCell=0;
			rhs._lastCell  ? _lastCell=new KSpreadPoint(*(rhs._lastCell )) : _lastCell=0;
			_color=QColor(rhs._color);
		}

void HighlightRange::getRange(KSpreadRange& rg)
{ 
			if (!_firstCell) 
			{ 
				rg=KSpreadRange();
				return;
			}

			if (_lastCell) 
			{
				rg=KSpreadRange(*_firstCell,*_lastCell); 	
			}
			else
			{
				rg=KSpreadRange(*_firstCell,*_firstCell);
			}
}