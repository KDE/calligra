#include <qimage.h>
#include <qdstream.h>

#include <ctype.h>

/*
Block Name                  Required   Label       Ext.   Vers.
Application Extension       Opt. (*)   0xFF (255)  yes    89a
Comment Extension           Opt. (*)   0xFE (254)  yes    89a
Global Color Table          Opt. (1)   none        no     87a
Graphic Control Extension   Opt. (*)   0xF9 (249)  yes    89a
Header                      Req. (1)   none        no     N/A
Image Descriptor            Opt. (*)   0x2C (044)  no     87a (89a)
Local Color Table           Opt. (*)   none        no     87a
Logical Screen Descriptor   Req. (1)   none        no     87a (89a)
Plain Text Extension        Opt. (*)   0x01 (001)  yes    89a
Trailer                     Req. (1)   0x3B (059)  no     87a

Unlabeled Blocks
Header                      Req. (1)   none        no     N/A
Logical Screen Descriptor   Req. (1)   none        no     87a (89a)
Global Color Table          Opt. (1)   none        no     87a
Local Color Table           Opt. (*)   none        no     87a

Graphic-Rendering Blocks
Plain Text Extension        Opt. (*)   0x01 (001)  yes    89a
Image Descriptor            Opt. (*)   0x2C (044)  no     87a (89a)

Control Blocks
Graphic Control Extension   Opt. (*)   0xF9 (249)  yes    89a

Special Purpose Blocks
Trailer                     Req. (1)   0x3B (059)  no     87a
Comment Extension           Opt. (*)   0xFE (254)  yes    89a
Application Extension       Opt. (*)   0xFF (255)  yes    89a
*/


static inline int gif_next_y( int oldY, int h ) {
    int y = oldY;
    switch( y%8 ) {
    case 0:
	y += 8;
	if ( y >= h )
	    y = 4;
	break;
    case 4:
	y += 8;
	if ( y >= h )
	    y = 2;
	break;
    case 2: // FALL THROUGH
    case 6:
	y += 4;
	if ( y >= h )
	    y = 1;
	break;
    default:
	y += 2;
	if ( y >= h )
	    y = -1;
	break;
    }
    return y;
}


static inline int gif_get_pixel( QImage& image, int& x, int& y ) {
    static uchar * p;
    int color;

    if ( x == 0 )
	p = image.scanLine( y );

    color = (int)*(p++);
    x++;
    return color;
}


static void gif_write_content( Q_UINT8 packet[], int& packetPointer,
			       Q_UINT32 & accumulator,
			       unsigned int & shift,
			       QDataStream * s )
{
    while ( shift > 7 ) {
	packet[packetPointer++] = (accumulator & 255);
	accumulator = accumulator >> 8;
	shift -= 8;
	if ( packetPointer == 256 ) {
	    s->device()->writeBlock( (char *)packet, 256 );
	    packetPointer = 1;
	}
    }
}


extern "C" void write_gif_image( QImageIO * iio )
{
    if ( iio )
	iio->setStatus( 1 );
    else
	return;

    QImage image;
    if ( iio->image().depth() != 8 )
	image = iio->image().convertDepth( 8 );
    else
	image = iio->image();

    int w = image.width(), h = image.height();
    int x, y;

    unsigned int i;
    int palette[256];
    int ncols = 0;
    for( i=0; i<256; i++ )
	palette[i] = -1;

    bool need89 = FALSE;
    int transparentColor = 0;
    for( y=0; y<h; y++ ) {
	uchar * yp = image.scanLine( y );
	for( x=0; x<w; x++ ) {
	    int color = (int)*(yp + x);
	    if ( need89 && (image.color( color ) & ~RGB_MASK) == 0 ) {
		palette[color] = transparentColor;
	    } else if ( palette[color] < 0 ) {
		if ( need89 == FALSE && image.hasAlphaBuffer() &&
		     (image.color( color ) & ~RGB_MASK) != 0 ) {
		    need89 = TRUE;
		    transparentColor = ncols;
		}
		palette[color] = ncols++;
	    }
	}
    }

    int colorTableSize = 2;
    int colorTableCode = 1;
    while ( colorTableSize < ncols ) {
	colorTableSize *= 2;
	colorTableCode++;
    }

    // write the file header
    QDataStream * s = new QDataStream( iio->ioDevice() );
    if ( s->device()->writeBlock( need89 ? "GIF89a" : "GIF87a", 6 ) < 6 )
	return;

    s->setByteOrder( QDataStream::LittleEndian );
    *s << (Q_UINT16)w
       << (Q_UINT16)h
       << (Q_UINT8) (128 + 17*(colorTableCode-1))
       << (Q_UINT8) 0 // some random color is "background"
       << (Q_UINT8) 0; // no aspect ratio information

    // build and write the global color table
    char * globalColorTable = new char[3*colorTableSize];
    memset( globalColorTable, 0, 3*colorTableSize );

    for( i=0; i<256; i++ )
	if ( palette[i] >= 0 ) {
	    QRgb c = image.color( i );
	    int p = 3*palette[i];
	    globalColorTable[p++] = (unsigned char)(qRed(c));
	    globalColorTable[p++] = (unsigned char)(qGreen(c));
	    globalColorTable[p] = (unsigned char)(qBlue(c));
	}

    if ( s->device()->writeBlock( globalColorTable, 3*colorTableSize ) <
	 3*colorTableSize )
	return;

    delete[]globalColorTable;
    globalColorTable = 0;

    // write transparent color index if there is one
    if ( need89 )
	*s << (Q_UINT8) 0x21 // extension block
	   << (Q_UINT8) 0xf9 // graphic control extension
	   << (Q_UINT8) 4 // size
	   << (Q_UINT8) 5 // no not dispose, transparent color follows
	   << (Q_UINT16) 0 // no delay time
	   << (Q_UINT8) transparentColor
	   << (Q_UINT8) 0; // terminate the extension block

    *s << (Q_UINT8) 0x2C // image separator, constant
       << (Q_UINT16) 0 // x
       << (Q_UINT16) 0 // y
       << (Q_UINT16) w
       << (Q_UINT16) h
       << (Q_UINT8) 64; // interlaced

    // write pixels

    unsigned int codeSize = colorTableCode;
    if ( codeSize < 2 )
	codeSize = 2;

    *s << (Q_UINT8) codeSize;

    // stuff for the compressor...

    struct {
	unsigned int child : 12;
	unsigned int sibling : 12;
	unsigned int pixel : 8;
    } codeTable[4096];

    Q_UINT8 packet[257]; // data sub-block and one byte for a hack below
    int packetPointer = 1;
    packet[0] = (Q_UINT8) 255;

    unsigned int clear = 1 << codeSize++; // gif says
    unsigned int endOfInformation = clear+1; // gif says
    unsigned int ncodes = clear+1; // highest-numbered code in use - mine

    unsigned int entry = clear; // clear, here, means Do Not Add
    codeTable[clear].child = clear;

    for( i=0; i<clear; i++ ) {
	codeTable[i].child = clear; // meaning: invalid
	codeTable[i].pixel = i;
    }

    UINT32 accumulator = 0;
    unsigned int shift = 0;

    x = 0;
    y = 0;
    unsigned int pixel, child;

    while( y >= 0 ) {
	// output to make space, if necessary
	if ( shift + codeSize > 30 )
	    gif_write_content( packet, packetPointer, accumulator, shift, s );

	pixel = palette[gif_get_pixel( image, x, y )];
	if ( x >= w ) {
	    y = gif_next_y( y, h );
	    x = 0;
	}

	// see whether entry+child exists in the dictionary
	child = codeTable[entry].child;
	while ( child != clear && codeTable[child].pixel != pixel )
	    child = codeTable[child].sibling;
	if ( child != clear ) {
	    // yes, use that entry
	    entry = child;
	} else if ( ncodes < 4095 && entry != clear ) {
	    // no, and entry is not 'clear', and there is space to add
	    // entry+child to the dictionary
	    child = ++ncodes;
	    codeTable[child].child = clear;
	    codeTable[child].pixel = pixel;
	    codeTable[child].sibling = codeTable[entry].child;
	    codeTable[entry].child = child;
	    accumulator = accumulator | ( entry << shift );
	    shift += codeSize;
	    // may need to increase the code size
	    if ( codeSize < 12 && ncodes >= (unsigned int)(1 << codeSize ) )
		codeSize++;
	    entry = pixel;
	} else {
	    // dictionary is full, so send the entry and then a clear
	    // (unless the entry is the initial clear)
	    accumulator = accumulator | ( entry << shift );
	    shift += codeSize;
	    // may need to make space in the accumulator
	    if ( shift + codeSize > 30 )
		gif_write_content( packet, packetPointer,
				   accumulator, shift, s );
	    if ( entry != clear ) {
		for( i=0; i<clear; i++ ) {
		    codeTable[i].child = clear; // meaning: invalid
		    codeTable[i].pixel = i;
		}
		accumulator = accumulator | ( clear << shift );
		shift += codeSize;
	    }
	    ncodes = clear+1;
	    codeSize = colorTableCode+1;
	    if ( codeSize < 2 )
		codeSize = 2;
	    entry = pixel;
	}
    }

    // write the last entry
    if ( shift + codeSize > 30 )
	gif_write_content( packet, packetPointer, accumulator, shift, s );
    accumulator = accumulator | ( entry << shift );
    shift += codeSize;

    // add the end of information marker
    accumulator = accumulator | ( endOfInformation << shift );
    shift = shift + codeSize + 7; // to make sure the final byte is sent off
    gif_write_content( packet, packetPointer, accumulator, shift, s );

    // add the zero-length sub-block and gif trailer
    packet[0] = (Q_UINT8)(packetPointer-1);
    if ( packetPointer > 1 )
	packet[packetPointer++] = (Q_UINT8) 0; // zero length sub-block
    packet[packetPointer++] = (Q_UINT8) 0x3B; // gif trailer
    if ( s->device()->writeBlock( (char *)packet,
				  packetPointer ) < packetPointer )
	return; // assumption: if this succeeds, all earlier writes succeeded

    iio->setStatus( 0 );
}


