#include "koStoreStream.h"
#include "koStore.h"

// (Werner) I've translated the comments - there might be
// some errors, though :(
// (David) Thanks !!!

int istorestreambuffer::underflow ()
{
    // read position before end of buffer ?
    if (gptr() < egptr() )
    {
        return *gptr();
    }

    /* calculate the Putback-Zone
     *  - how many did we read already?
     *  - maximum 4 chars
     */
    int anzPutback;
    anzPutback = gptr() - eback();
    if (anzPutback > 4)
    {
        anzPutback = 4;
    }

    /* prepend the chars already read (put them
     * back in the buffer (first 4 chars)
     */
    memcpy( puffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    /* read new chars
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
        // either there was an error or we are at the EOF
        return EOF;
    }
    //else
    //  kdebug( KDEBUG_INFO, 30002, "Read %i bytes", anz );
     
    /* set the pointer to our buffer
     */
    setg (puffer+(4-anzPutback),   // start of the Putback-zone
	  puffer+4,                    // read position
	  puffer+4+anz);               // end of the buffer
    
    // return the next char
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

    pbump (-anz);    // restore "write-pointer" position
    return anz;
}

