#include "koStoreStream.h"

int istorestreambuffer::underflow ()
{
    // Leseposition vor Puffer-Ende ?
    if (gptr() < egptr() )
    {
      return *gptr();
    }

    /* Anzahl Putback-Bereich ermitteln
     *  - Anzahl bereits gelesener Zeichen
     *  - maximal 4
     */
    int anzPutback;
    anzPutback = gptr() - eback();
    if (anzPutback > 4)
    {
      anzPutback = 4;
    }

    /* die bis zu vier vorher gelesenen Zeichen nach vorne
     * in den Putback-Puffer (erste 4 Zeichen)
     */
    memcpy( puffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    /* neue Zeichen lesen
     */
    long anz = 8192;
    if ( !CORBA::is_nil( m_vStore ) )
    {
      KOStore::Data* p;
      //kdebug( KDEBUG_INFO, 30002, "--->read" );
      p = m_vStore->read( pufferSize - 4 );
      //kdebug( KDEBUG_INFO, 30002, "<---" );
      if ( !p )
	return EOF;
      anz = p->length();
      for( int i = 0; i < anz; i++ )
	puffer[ 4 + i ] = (char)((*p)[i]);
      delete p;
    }
    else
      anz = m_pStore->read( puffer + 4, pufferSize - 4 );
    if ( anz <= 0 )
    {
      // Fehler oder EOF
      return EOF;
    }
    //else
    //  kdebug( KDEBUG_INFO, 30002, "Read %i bytes", anz );
     
    /* Puffer-Zeiger neu setzen
     */
    setg (puffer+(4-anzPutback),   // Putback-Anfang
	  puffer+4,                // Leseposition
	  puffer+4+anz);           // Puffer-Ende
    
    // naechstes Zeichen zurueckliefern
    unsigned char c = *((unsigned char*)gptr());
    return c;
    // return *gptr();
}

int ostorestreambuffer::emptybuffer()
{
    int anz = pptr()-pbase();
    if ( !CORBA::is_nil( m_vStore ) )
    {
      KOStore::Data data;
      data.length( anz );
      for( int i = 0; i < anz; i++ )
	  data[ i ] = m_buffer[ i ];
      m_vStore->write( data );
    }
    else
      m_pStore->write( m_buffer, anz );

    pbump (-anz);    // Schreibzeiger entspr. zuruecksetzen
    return anz;
}
