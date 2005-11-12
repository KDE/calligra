/*
KSpread's code is IMO often hard to read. The cause of this is that there
is no clear distinction between coordinates on the screen and coordinates
in a sheet. Therefore I want to propose to define our own types to refer to
coordinates on the sheet. 
This is a quick draft of how it could look like. It is definitely not complete.
So feel free to change it and add comments. The types are very straightforward,
so I think no further comments are needed yet.  -- Wilco --
*/

namespace KSpread
{

typedef Column unsigned long int;
typedef Row unsigned long int;


class Coordinate
{
public:
    Coordinate( Row, Column );

    Row row() const { return m_iRow; }
    Column column() const { return m_iColumn; }

private:
    Row m_iRow;
    Column m_iColumn;
};


class Selection
{
public:
    Selection( Coordinate, Coordinate );

    Coordinate begin() const { return m_crdBegin; }
    Coordinate end() const { return m_crdEnd; }
    
private:
    Coordinate m_crdBegin;
    Coordinate m_crdEnd;
};

} // namespace KSpread
