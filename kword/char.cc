/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "char.h"
#include "kword_doc.h"
#include "kword_frame.h"
#include "font.h"
#include "defs.h"

#include <assert.h>

#include <kdebug.h>
#include <koStream.h>
#include <kword_utils.h>

#include <strstream>
#include <fstream>
#include <unistd.h>

#include <qimage.h>

/******************************************************************/
/* Class: KWCharAnchor                                            */
/******************************************************************/

/*================================================================*/
KWCharAnchor::KWCharAnchor() :
    KWCharAttribute()
{
    classId = ID_KWCharAnchor;
    anchored = false;
    origin = QPoint( 0, 0 );
}

KWCharAnchor::KWCharAnchor(const KWCharAnchor &original) :
    KWCharAttribute()
{
kdDebug() << "KWCharAnchor::KWCharAnchor(orig)" << endl;
    classId = ID_KWCharAnchor;
    anchored = original.anchored;
    origin = original.origin;
}

/*================================================================*/
void KWCharAnchor::setAnchored( bool _anchored )
{
    anchored = _anchored;
}

/*================================================================*/
void KWCharAnchor::setOrigin( QPoint _origin )
{
    // If the origin is being changed, and the anchoring logic is enabled,
    // signal the derived object to move!
    if ( anchored )
    {
        int dx = _origin.x() - origin.x();
        int dy = _origin.y() - origin.y();

        if ((dx != 0) || (dy != 0))
            moveBy( dx, dy );
    }
    origin = _origin;
}

/*================================================================*/
void KWCharAnchor::save( QTextStream &out )
{
    out << indent << "<ANCHOR type=\"" << correctQString( anchorType() ) <<
        "\" instance=\"" << correctQString( anchorInstance() ) <<
        "\"/>" << endl;
}

/******************************************************************/
/* Class: KWString                                                */
/******************************************************************/

/*================================================================*/
KWString::KWString( QString _str, KWordDocument *_doc )
    : cache(), allowRemoveFn( FALSE )
{
    if ( _str == 0L ) {
        _data_ = 0L;
        _len_ = 0;
        _max_ = 0;
        return;
    }

    _len_ = _str.length();
    _max_ = _len_;

    _data_ = alloc( _len_ );

    unsigned int i = 0;
    while( _str[ i ] != 0L )
        _data_[ i ].c = _str[ i++ ];

    doc = _doc;
}

/*================================================================*/
KWString::KWString( const KWString &_string )
{
    _data_ = copy( _string._data_, _string._len_ );
    _len_ = _string._len_;
    _max_ = _string._max_;
    doc = _string.doc;
    cache = _string.cache;
    allowRemoveFn = FALSE;
}

/*================================================================*/
KWString &KWString::operator=( const KWString &_string )
{
    _data_ = copy( _string._data_, _string._len_ );
    _len_ = _string._len_;
    _max_ = _string._max_;
    doc = _string.doc;
    cache = _string.cache;

    return *this;
}

/*================================================================*/
KWChar* KWString::alloc( unsigned int _len )
{
    KWChar *c = new KWChar[ _len ];

    KWChar *p = c;
    for ( unsigned int i = 0; i < _len; ++i ) {
        p->c = KWSpecialChar;
        p->attrib = 0L;
        p++;
    }

    return c;
}

/*================================================================*/
void KWString::free( KWChar* _data, unsigned int _len )
{
    for ( unsigned int i = 0; i < _len; ++i )
        freeChar( _data[ i ], doc, allowRemoveFn );
}

/*================================================================*/
void KWString::append( KWChar *_text, unsigned int _len )
{
    unsigned int oldlen = _len_;
    resize( _len + _len_ );
    KWChar *_data = copy( _text, _len );

    for ( unsigned int i = 0; i < _len; i++ ) {
        _data_[ oldlen + i ].c = _data[ i ].c;
        _data_[ oldlen + i ].attrib = _data[ i ].attrib;
        cache += _data[ i ].c;
    }
}

/*================================================================*/
void KWString::append( KWChar _c )
{
    unsigned int oldlen = _len_;
    resize( 1 + _len_ );
    KWChar c = copy( _c );

    _data_[ oldlen ].c = c.c;
    _data_[ oldlen ].attrib = c.attrib;
    cache += _c.c;
}

/*================================================================*/
void KWString::insert( unsigned int _pos, QString _text )
{
    assert( _pos <= _len_ );

    unsigned int nl = _text.length();

    unsigned int l = _len_;

    resize( _len_ + nl );

    if ( _pos < l )
        memmove( _data_ + _pos + nl, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    for ( unsigned int i = 0; i < nl; ++i ) {
        _data_[ _pos + i ].c = _text[ i ];
        _data_[ _pos + i ].attrib = 0L;
    }
    cache.insert( _pos, _text );
}

/*================================================================*/
void KWString::insert( unsigned int _pos, KWString *_text )
{
    assert( _pos <= _len_ );

    unsigned int nl = _text->size();

    unsigned int l = _len_;

    resize( _len_ + nl );

    if ( _pos < l )
        memmove( _data_ + _pos + nl, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    for ( unsigned int i = 0; i < nl; ++i ) {
        _data_[ _pos + i ].c = _text->data()[ i ].c;
        _data_[ _pos + i ].attrib = _text->data()[ i ].attrib;
        cache.insert( _pos + i, _text->data()[ i ].c );
    }
}

/*================================================================*/
void KWString::insert( unsigned int _pos, const char _c )
{
    assert( _pos <= _len_ );

    unsigned int l = _len_;

    resize( _len_ + 1 );

    if ( _pos < l )
        memmove( _data_ + _pos + 1, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    _data_[ _pos ].c = _c;
    _data_[ _pos ].attrib = 0L;
    cache.insert( _pos, _c );
}

/*================================================================*/
void KWString::insert( unsigned int _pos, KWCharImage *_image )
{
    assert( _pos <= _len_ );

    unsigned int l = _len_;

    resize( _len_ + 1 );

    if ( _pos < l )
        memmove( _data_ + _pos + 1, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    _data_[ _pos ].c = KWSpecialChar;
    _data_[ _pos ].attrib = _image;
    cache.insert( _pos, KWSpecialChar );
}

/*================================================================*/
void KWString::insert( unsigned int _pos, KWCharTab *_tab )
{
    assert( _pos <= _len_ );

    unsigned int l = _len_;

    resize( _len_ + 1 );

    if ( _pos < l )
        memmove( _data_ + _pos + 1, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    _data_[ _pos ].c = KWSpecialChar;
    _data_[ _pos ].attrib = _tab;
    cache.insert( _pos, KWSpecialChar );
}

/*================================================================*/
void KWString::insert( unsigned int _pos, KWCharVariable *_var )
{
    assert( _pos <= _len_ );

    unsigned int l = _len_;

    resize( _len_ + 1 );

    if ( _pos < l )
        memmove( _data_ + _pos + 1, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    _data_[ _pos ].c = KWSpecialChar;
    _data_[ _pos ].attrib = _var;
    cache.insert( _pos, KWSpecialChar );
}

/*================================================================*/
void KWString::insert( unsigned int _pos, KWCharFootNote *_fn )
{
    assert( _pos <= _len_ );

    unsigned int l = _len_;

    resize( _len_ + 1 );

    if ( _pos < l )
        memmove( _data_ + _pos + 1, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    _data_[ _pos ].c = KWSpecialChar;
    _data_[ _pos ].attrib = _fn;
    cache.insert( _pos, KWSpecialChar );
}

/*================================================================*/
void KWString::insert( unsigned int _pos, KWCharAnchor *_anchor )
{
    assert( _pos <= _len_ );

    unsigned int l = _len_;

    resize( _len_ + 1 );

    if ( _pos < l )
        memmove( _data_ + _pos + 1, _data_ + _pos, sizeof( KWChar ) * ( l - _pos ) );

    _data_[ _pos ].c = KWSpecialChar;
    _data_[ _pos ].attrib = _anchor;
    cache.insert( _pos, KWSpecialChar );
}

/*================================================================*/
void KWString::clear()
{
    remove( 0, size() );
    cache = QString::null;
}

/*================================================================*/
bool KWString::remove( unsigned int _pos, unsigned int _len )
{
    if ( _pos + _len <= _len_ && ( int )_pos >= 0 ) {
        allowRemoveFn = TRUE;
        for ( unsigned int i = _pos; i < _pos + _len; i++ )
            freeChar( _data_[ i ], doc, allowRemoveFn );
        allowRemoveFn = FALSE;

        memmove( _data_ + _pos, _data_ + _pos + _len, sizeof( KWChar ) * ( _len_ - _pos - _len ) );
        resize( _len_ - _len, false );

        cache.remove( _pos, _len );

        return true;
    }
    return false;
}

/*================================================================*/
KWChar* KWString::split( unsigned int _pos )
{
    KWChar *_data, *__data;
    _data = alloc( _len_ - _pos );
    for ( unsigned int i = _pos; i < _len_; i++ ) {
        _data[ i - _pos ].c = _data_[ i ].c;
        _data[ i - _pos ].attrib = _data_[ i ].attrib;
    }

    __data = copy( _data, _len_ - _pos );
    resize( _pos );
    cache.truncate( _pos );
    return __data;
}

/*================================================================*/
QString KWString::toString( bool cached )
{
    if ( cached )
        return cache;
    return toString( 0, size() );
}

/*================================================================*/
QString KWString::toString( unsigned int _pos, unsigned int _len, bool cached )
{
    if ( cached )
        return cache.mid( _pos, _len );

    QString str = "";
    char c = 1;

    if ( _pos + _len <= _len_ ) {
        for ( unsigned int i = _pos; i <= _len + _pos; i++ ) {
            if ( static_cast<int>( i ) > static_cast<int>( size() - 1 ) ) break;
            if ( _data_[ i ].c != KWSpecialChar )
                str += _data_[ i ].c;
            else
                str += c;
        }
    }
    cached = !str.isEmpty();

    return str;
}

/*================================================================*/
void KWString::saveFormat( QTextStream&out )
{
    unsigned int start = 0;

    for ( unsigned int i = 0; i < _len_; i++ ) {
        if ( _data_[ i ].attrib->getClassId() != ID_KWCharFormat ) {
            if ( start < i ) {
                out << otag << "<FORMAT id=\"" << _data_[ start ].attrib->getClassId() << "\" pos=\"" << start
                    << "\" len=\"" << i - start << "\">" << endl;
                _data_[ start ].attrib->save( out );
                out << etag << "</FORMAT>" << endl;
            }
            switch ( _data_[ i ].attrib->getClassId() ) {
            case ID_KWCharImage: {
                out << otag << "<FORMAT id=\"" << _data_[ i ].attrib->getClassId() << "\" pos=\"" << i << "\">"
                    << endl;
                _data_[ i ].attrib->save( out );
                out << etag << "</FORMAT>" << endl;
            } break;
            case ID_KWCharTab: {
                out << otag << "<FORMAT id=\"" << _data_[ i ].attrib->getClassId() << "\" pos=\"" << i << "\">"
                    << endl;
                out << etag << "</FORMAT>" << endl;
            } break;
            case ID_KWCharVariable: {
                out << otag << "<FORMAT id=\"" << _data_[ i ].attrib->getClassId() << "\" pos=\"" << i << "\">"
                    << endl;
                _data_[ i ].attrib->save( out );
                out << etag << "</FORMAT>" << endl;
            } break;
            case ID_KWCharFootNote: {
                out << otag << "<FORMAT id=\"" << _data_[ i ].attrib->getClassId() << "\" pos=\"" << i << "\">"
                    << endl;
                _data_[ i ].attrib->save( out );
                out << etag << "</FORMAT>" << endl;
            } break;
            case ID_KWCharAnchor: {
                out << otag << "<FORMAT id=\"" << _data_[ i ].attrib->getClassId() << "\" pos=\"" << i << "\">"
                    << endl;
                _data_[ i ].attrib->save( out );
                out << etag << "</FORMAT>" << endl;
            } break;
            default: break;
            }
            start = i + 1;
        } else if ( i > 0 && _data_[ i ].attrib->getClassId() == ID_KWCharFormat &&
                    !( *( ( KWCharFormat* )_data_[ i ].attrib ) == *( ( KWCharFormat* )_data_[ i - 1 ].attrib ) ) ) {
            if ( start < i ) {
                out << otag << "<FORMAT id=\"" << _data_[ start ].attrib->getClassId() << "\" pos=\"" << start
                    << "\" len=\"" << i - start << "\">" << endl;
                _data_[ start ].attrib->save( out );
                out << etag << "</FORMAT>" << endl;
            }
            start = i;
        }
    }

    if ( start < _len_ ) {
        out << otag << "<FORMAT id=\"" << _data_[ start ].attrib->getClassId() << "\" pos=\"" << start
            << "\" len=\"" << _len_ - start << "\">" << endl;
        _data_[ start ].attrib->save( out );
        out << etag << "</FORMAT>" << endl;
    }
}

/*================================================================*/
void KWString::loadFormat( KOMLParser& parser, QValueList<KOMLAttrib>& lst, KWordDocument *_doc, KWTextFrameSet * )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        // format
        if ( name == "FORMAT" ) {
            ClassIDs _id = ID_KWCharNone;
            unsigned int __pos = 0, __len = 0;
            KWFormat *_format = 0L, *format = 0L;
            KWImage *_image = 0L; // , *image = 0L;
            KWCharImage *_kwimage = 0L;
            KWCharFormat *_kwformat = 0L;
            KWCharTab *_kwtab = 0L;
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            bool _load = false;
            for ( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "id" ) {
                    _id = static_cast<ClassIDs>( ( *it ).m_strValue.toInt() );
                    _load = true;
                }
                else if ( ( *it ).m_strName == "pos" )
                    __pos = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "len" )
                    __len = ( *it ).m_strValue.toInt();
            }

            if ( _load ) {
                switch ( _id ) {
                case ID_KWCharFormat: {
                    _format = new KWFormat();
                    _format->load( parser, lst, _doc );
                    format = _doc->getFormatCollection()->getFormat( *_format );
                    for ( unsigned int i = __pos; i < __pos + __len; i++ )
                    {
                        if ( static_cast<int>( i ) > static_cast<int>( size() - 1 ) ) break;
                        freeChar( _data_[ i ], doc, allowRemoveFn );
                        _kwformat = new KWCharFormat( format );
                        _data_[ i ].attrib = _kwformat;
                        format->incRef();
                    }
                    format->decRef();
                    delete _format;
                    _format = 0;
                } break;
                case ID_KWCharImage: {
                    _image = new KWImage();
                    _image->load( parser, lst, _doc );
//                     QString key;
//                     image = _doc->getImageCollection()->getImage( *_image, key );
                    _kwimage = new KWCharImage( 0L );
                    doc->addImageRequest( _image->getFilename(), _kwimage );
                    freeChar( _data_[ __pos ], doc, allowRemoveFn );
                    _data_[ __pos ].c = KWSpecialChar;
                    _data_[ __pos ].attrib = _kwimage;
                    delete _image;
                    _image = 0;
                } break;
                case ID_KWCharTab: {
                    _kwtab = new KWCharTab();
                    freeChar( _data_[ __pos ], doc, allowRemoveFn );
                    _data_[ __pos ].c = KWSpecialChar;
                    _data_[ __pos ].attrib = _kwtab;
                } break;
                case ID_KWCharVariable: {
                    VariableType vart;
                    KWVariable *var = 0L;
                    KWCharVariable *v = 0L;

                    while ( parser.open( QString::null, tag ) ) {
                        parser.parseTag( tag, name, lst );

                        if ( name == "TYPE" ) {
                            parser.parseTag( tag, name, lst );
                            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                            for ( ; it != lst.end(); ++it ) {
                                if ( ( *it ).m_strName == "type" ) {
                                    vart = static_cast<VariableType>( ( *it ).m_strValue.toInt() );
                                    switch ( vart ) {
                                    case VT_DATE_FIX:
                                        var = new KWDateVariable( _doc );
                                        break;
                                    case VT_DATE_VAR:
                                        var = new KWDateVariable( _doc );
                                        break;
                                    case VT_TIME_FIX:
                                        var = new KWTimeVariable( _doc );
                                        break;
                                    case VT_TIME_VAR:
                                        var = new KWTimeVariable( _doc );
                                        break;
                                    case VT_PGNUM:
                                        var = new KWPgNumVariable( _doc );
                                        break;
                                    case VT_CUSTOM:
                                        var = new KWCustomVariable( _doc );
                                        break;
                                    case VT_SERIALLETTER:
                                        var = new KWSerialLetterVariable( _doc );
                                        break;
                                    default: break;
                                    }

                                    var->setVariableFormat( _doc->getVarFormats().find( static_cast<int>( vart ) ) );
                                    v = new KWCharVariable( var );
                                }
                            }
                        } else if ( name == "FRMAT" && v ) {
                            _format = new KWFormat();
                            _format->load( parser, lst, _doc );
                            format = _doc->getFormatCollection()->getFormat( *_format );
                            freeChar( _data_[ __pos ], doc, allowRemoveFn );
                            v->setFormat( format );
                            _data_[ __pos ].attrib = v;
                            _data_[ __pos ].c = KWSpecialChar;
                            delete _format;
                            _format = 0;
                        } else {
                            if ( var )
                                var->load( parser, name, tag, lst );
                        }
                        if ( !parser.close( tag ) ) {
                            kdError(32001) << "Closing " << tag << endl;
                            return;
                        }
                    }
                } break;
                case ID_KWCharFootNote: {
                    KWFootNote *fn = new KWFootNote( doc, new QList<KWFootNote::KWFootNoteInternal>() );
                    KWCharFootNote *v = new KWCharFootNote( fn );

                    while ( parser.open( QString::null, tag ) ) {
                        parser.parseTag( tag, name, lst );

                        if ( name == "FRMAT" && v ) {
                            _format = new KWFormat();
                            _format->load( parser, lst, _doc );
                            format = _doc->getFormatCollection()->getFormat( *_format );
                            freeChar( _data_[ __pos ], doc, allowRemoveFn );
                            v->setFormat( format );
                            _data_[ __pos ].attrib = v;
                            _data_[ __pos ].c = KWSpecialChar;
                            delete _format;
                            _format = 0;
                        } else {
                            if ( fn )
                                fn->load( name, tag, parser, lst );
                        }
                        if ( !parser.close( tag ) ) {
                            kdError(32001) << "Closing " << tag << endl;
                            return;
                        }
                    }

                    doc->getFootNoteManager().insertFootNoteInternal( fn );
                } break;
                case ID_KWCharAnchor: {
                    QString attribute = "";
                    QString type = "";
                    QString instance = "";
                    KWCharAnchor *anchor = NULL;

                    while ( parser.open( QString::null, tag ) ) {
                        parser.parseTag( tag, name, lst );
                        if ( name == "ANCHOR" ) {
                            parser.parseTag( tag, name, lst );
                            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                            for ( ; it != lst.end(); ++it ) {
                                attribute = (*it).m_strName;
                                if ( attribute == "type" ) {
                                    type = (*it).m_strValue;
                                }
                                else if ( attribute == "instance" ) {
                                    instance = (*it).m_strValue;
                                }
                                else {
                                    kdError(32001) << "Unknown " << name <<
                                                " attrib '" << attribute << "'" << endl;
                                }
                            }

                            // Create an anchor object of the right type.
                            if ( type == "grpMgr" ) {
                                if ( !instance.isEmpty() ) {
                                    KWGroupManager *group = new KWGroupManager( doc );
                                    group->setName( instance );
                                    group->setAnchored( true );
                                    doc->addGroupManager( group );
                                    anchor = group;
                                }
                                else {
                                    kdError(32001) << "Missing " << name <<
                                                " attrib: instance" << endl;
                                }
                            }
                            else {
                                kdError(32001) << "Unknown " << name <<
                                        " attrib value type=" << type << endl;
                            }
                        }
                        else {
                            kdError(32001) << "Unknown tag '" << name <<
                                        "' in " << tag << endl;
                        }
                        if ( !parser.close( tag ) ) {
                            kdError(32001) << "Closing " << tag << endl;
                            return;
                        }
                        }

                        // If we managed to find a valid anchor, store it.
                        if ( anchor ) {
                            freeChar( _data_[ __pos ], doc, allowRemoveFn );
                            _data_[ __pos ].c = KWSpecialChar;
                            _data_[ __pos ].attrib = anchor;
                        }
                } break;
                default: break;
                }
                _load = false;
            }
        }

        if ( !parser.close( tag ) ) {
            kdError(32001) << "Closing " << tag << endl;
            return;
        }
    }
    // build cache
    toString( FALSE );
}

/*================================================================*/
void KWString::resize( unsigned int _size, bool del )
{
    if ( _size == _len_ )
        return;

    if ( _size < _len_ ) {
        if ( del ) free( _data_ + _size, _len_ - _size );
        _len_ = _size;

        return;
    }

    /* _size > _len_ */
    if (_size < _max_) {
        _len_ = _size;
        return;
    }

    // Alloc some bytes more => faster when increasing size in steps of 1
    KWChar *d = alloc( _size + 10 );
    if ( _data_ ) {
        memcpy( d, _data_, _len_ * sizeof( KWChar ) );
        delete [] _data_;
    }

    _data_ = d;
    _len_ = _size;
    _max_ = _size + 10;
}

/*================================================================*/
KWChar* KWString::copy( KWChar *_data, unsigned int _len )
{
    KWChar *__data;

    __data = alloc( _len );

    unsigned int i = 0;
    for ( i = 0; i < _len; i++ ) {
        __data[ i ].c = _data[ i ].c;
        if ( _data[ i ].attrib ) {
            switch ( _data[ i ].attrib->getClassId() ) {
            case ID_KWCharFormat: {
                KWCharFormat *attrib = ( KWCharFormat* )_data[ i ].attrib;
                attrib->getFormat()->incRef();
                KWCharFormat *f = new KWCharFormat( attrib->getFormat() );
                __data[ i ].attrib = f;
            } break;
            case ID_KWCharImage: {
                KWCharImage *attrib = ( KWCharImage* )_data[ i ].attrib;
                attrib->getImage()->incRef();
                KWCharImage *f = new KWCharImage( attrib->getImage() );
                __data[ i ].attrib = f;
            } break;
            case ID_KWCharTab: {
                KWCharTab *f = new KWCharTab();
                __data[ i ].attrib = f;
            } break;
            case ID_KWCharVariable: {
                KWCharFormat *attrib = dynamic_cast<KWCharVariable*>( _data[ i ].attrib );
                attrib->getFormat()->incRef();
                KWCharVariable *f = new KWCharVariable( dynamic_cast<KWCharVariable*>( _data[ i ].attrib )->
                                                        getVar()->copy() );
                f->setFormat( attrib->getFormat() );
                __data[ i ].attrib = f;
            } break;
            case ID_KWCharFootNote: {
                KWCharFootNote *attrib = dynamic_cast<KWCharFootNote*>( _data[ i ].attrib );
                attrib->getFormat()->incRef();
                KWCharFootNote *f = new KWCharFootNote( dynamic_cast<KWCharFootNote*>( _data[ i ].attrib )->
                                                        getFootNote()->copy() );
                f->setFormat( attrib->getFormat() );
                __data[ i ].attrib = f;
            } break;
            case ID_KWCharAnchor: {
                KWGroupManager *attrib = dynamic_cast<KWGroupManager*>( _data[ i ].attrib );
                KWGroupManager *a = new KWGroupManager( *attrib );
                __data[ i ].attrib = a;
            } break;
            }
        }
        else __data[ i ].attrib = 0L;
        __data[ i ].autoformat = 0L;
    }
    return __data;
}

/*================================================================*/
KWChar& KWString::copy( KWChar _c )
{
    KWChar *c = new KWChar;

    c->c = _c.c;
    if ( _c.attrib ) {
        switch ( _c.attrib->getClassId() )
        {
        case ID_KWCharFormat: {
            KWCharFormat *attrib = ( KWCharFormat* )_c.attrib;
            attrib->getFormat()->incRef();
            KWCharFormat *f = new KWCharFormat( attrib->getFormat() );
            c->attrib = f;
        } break;
        case ID_KWCharImage: {
            KWCharImage *attrib = ( KWCharImage* )_c.attrib;
            attrib->getImage()->incRef();
            KWCharImage *f = new KWCharImage( attrib->getImage() );
            c->attrib = f;
        } break;
        case ID_KWCharTab: {
            KWCharTab *f = new KWCharTab();
            c->attrib = f;
        } break;
        case ID_KWCharVariable: {
            KWCharFormat *attrib = dynamic_cast<KWCharVariable*>( _c.attrib );
            attrib->getFormat()->incRef();
            KWCharVariable *f = new KWCharVariable( dynamic_cast<KWCharVariable*>( _c.attrib )->getVar()->copy() );
            f->setFormat( attrib->getFormat() );
            c->attrib = f;
        } break;
        case ID_KWCharFootNote: {
            KWCharFootNote *attrib = dynamic_cast<KWCharFootNote*>( _c.attrib );
            attrib->getFormat()->incRef();
            KWCharFootNote *f = new KWCharFootNote( dynamic_cast<KWCharFootNote*>( _c.attrib )->
                                                    getFootNote()->copy() );
            f->setFormat( attrib->getFormat() );
            c->attrib = f;
        } break;
        case ID_KWCharAnchor: {
            KWGroupManager *attrib = dynamic_cast<KWGroupManager*>( _c.attrib );
            KWGroupManager *a = new KWGroupManager( *attrib );
            c->attrib = a;
        } break;
        }
    }
    else c->attrib = 0L;
    c->autoformat = 0L;

    return *c;
}

/*================================================================*/
int KWString::find( QString _expr, KWSearchDia::KWSearchEntry *_format, int _index, bool _cs, bool _whole )
{
    QString str = toString();
    int res = str.find( _expr, _index, _cs );

    if ( res != -1 ) {
        if ( !_format && !_whole ) return res;

        if ( !_format && _whole ) {
            if ( ( res == 0 || res > 0 && str[ res - 1 ] == ' ' ) &&
                 ( res + static_cast<int>( _expr.length() ) == static_cast<int>( _len_ ) ||
                   res + static_cast<int>( _expr.length() ) <
                   static_cast<int>( _len_ ) && str[ res + _expr.length() ] == ' ' ) )
                return res;
            return -2;
        }

        KWFormat *format;
        for ( unsigned int i = 0; i < _expr.length(); i++ ) {
            if ( _data_[ i + res ].attrib->getClassId() != ID_KWCharFormat )
                return -2;

            format = dynamic_cast<KWCharFormat*>( _data_[ i + res ].attrib )->getFormat();

            if ( _format->checkFamily && _format->family != format->getUserFont()->getFontName() )
                return -2;
            if ( _format->checkColor && _format->color != format->getColor() )
                return -2;
            if ( _format->checkSize && _format->size != format->getPTFontSize() )
                return -2;
            if ( _format->checkBold && _format->bold != ( format->getWeight() == QFont::Bold ) )
                return -2;
            if ( _format->checkItalic && _format->italic != format->getItalic() )
                return -2;
            if ( _format->checkUnderline && _format->underline != format->getUnderline() )
                return -2;
            if ( _format->checkVertAlign && _format->vertAlign != format->getVertAlign() )
                return -2;
        }

        if ( !_whole )
            return res;
        else {
            if ( ( res == 0 || res > 0 && str[ res - 1 ] == ' ' ) &&
                 ( res + static_cast<int>( _expr.length() ) == static_cast<int>( _len_ ) ||
                   res + static_cast<int>( _expr.length() ) <
                   static_cast<int>( _len_ ) && str[ res + _expr.length() ] == ' ' ) )
                return res;
            return -2;
        }
    }
    else return -1;
}

/*================================================================*/
int KWString::find( QRegExp _regexp, KWSearchDia::KWSearchEntry *_format, int _index, int &_len,
                    bool _cs, bool _wildcard )
{
    QString str = toString();
    _regexp.setWildcard( _wildcard );
    _regexp.setCaseSensitive( _cs );
    int res = _regexp.match( str, _index, &_len );

    if ( res != -1 ) {
        if ( !_format ) return res;

        KWFormat *format;
        for ( int i = 0; i < _len; i++ ) {
            if ( _data_[ i + res ].attrib->getClassId() != ID_KWCharFormat )
                return -2;

            format = dynamic_cast<KWCharFormat*>( _data_[ i + res ].attrib )->getFormat();

            if ( _format->checkFamily && _format->family != format->getUserFont()->getFontName() )
                return -2;
            if ( _format->checkColor && _format->color != format->getColor() )
                return -2;
            if ( _format->checkSize && _format->size != format->getPTFontSize() )
                return -2;
            if ( _format->checkBold && _format->bold != ( format->getWeight() == QFont::Bold ) )
                return -2;
            if ( _format->checkItalic && _format->italic != format->getItalic() )
                return -2;
            if ( _format->checkUnderline && _format->underline != format->getUnderline() )
                return -2;
            if ( _format->checkVertAlign && _format->vertAlign != format->getVertAlign() )
                return -2;
        }

        return res;
    }
    else return -1;
}

/*================================================================*/
int KWString::findRev( QString _expr, KWSearchDia::KWSearchEntry *_format, int _index, bool _cs, bool _whole )
{
    QString str = toString();
    int res = str.findRev( _expr, _index, _cs );

    if ( res != -1 ) {
        if ( !_format && !_whole ) return res;

        if ( !_format && _whole ) {
            if ( ( res == 0 || res > 0 && str[ res - 1 ] == ' ' ) &&
                 ( res + static_cast<int>( _expr.length() ) == static_cast<int>( _len_ ) ||
                   res + static_cast<int>( _expr.length() ) <
                   static_cast<int>( _len_ ) && str[ res + _expr.length() ] == ' ' ) )
                return res;
            return -2;
        }

        KWFormat *format;
        for ( unsigned int i = 0; i < _expr.length(); i++ ) {
            if ( _data_[ i + res ].attrib->getClassId() != ID_KWCharFormat )
                return -2;

            format = dynamic_cast<KWCharFormat*>( _data_[ i + res ].attrib )->getFormat();

            if ( _format->checkFamily && _format->family != format->getUserFont()->getFontName() )
                return -2;
            if ( _format->checkColor && _format->color != format->getColor() )
                return -2;
            if ( _format->checkSize && _format->size != format->getPTFontSize() )
                return -2;
            if ( _format->checkBold && _format->bold != ( format->getWeight() == QFont::Bold ) )
                return -2;
            if ( _format->checkItalic && _format->italic != format->getItalic() )
                return -2;
            if ( _format->checkUnderline && _format->underline != format->getUnderline() )
                return -2;
            if ( _format->checkVertAlign && _format->vertAlign != format->getVertAlign() )
                return -2;
        }

        if ( !_whole )
            return res;
        else {
            if ( ( res == 0 || res > 0 && str[ res - 1 ] == ' ' ) &&
                 ( res + static_cast<int>( _expr.length() ) == static_cast<int>( _len_ ) ||
                   res + static_cast<int>( _expr.length() ) <
                   static_cast<int>( _len_ ) && str[ res + _expr.length() ] == ' ' ) )
                return res;
            return -2;
        }
    }
    else return -1;
}

/*================================================================*/
int KWString::findRev( QRegExp /*_regexp*/, KWSearchDia::KWSearchEntry */*_format*/,
                       int /*_index*/, int &/*_len*/, bool /*_cs*/, bool /*_wildcard*/ )
{
    return -1;
}

/*================================================================*/
QString KWString::decoded()
{
    QString str = toString();

    str.append( "_" );

    // When encoding the run-time form of text to its stored form,
    // be sure to do the conversion for "&amp;" to "&" first to avoid
    // accidentally converting user text into one of the other escape
    // sequences.
    //
    // Note that the conversion for "&amp;" allows for coexistance
    // with QDom-based input filters.
    str.replace( QRegExp( "&" ), "&amp;" );
    str.replace( QRegExp( "<" ), "&lt;" );
    str.replace( QRegExp( ">" ), "&gt;" );

    str.remove( str.length() - 1, 1 );

    return QString( str );
}

/*================================================================*/
/* obsolete
QCString KWString::utf8( bool _decoded )
{
    QString str;
    if ( _decoded )
        str = decoded();
    else
        str = toString();

    return QCString( str.utf8() );
}
*/

/*================================================================*/
void freeChar( KWChar& _char, KWordDocument *_doc, bool allowRemoveFn )
{
    if ( _char.attrib ) {
        switch( _char.attrib->getClassId() ) {
        case ID_KWCharFormat:
        case ID_KWCharImage:
        case ID_KWCharTab:
        case ID_KWCharVariable:
            delete _char.attrib;
            break;
        case ID_KWCharFootNote: {
            if ( allowRemoveFn ) {
                _doc->getFootNoteManager().
                    removeFootNote( dynamic_cast<KWCharFootNote*>( _char.attrib )->getFootNote() );
            }
            delete _char.attrib;
        } break;
        case ID_KWCharAnchor: {
            // huh? (Werner)
            //KWGroupManager *gm = (KWGroupManager *)_char.attrib;
            delete _char.attrib;
        } break;
        default: ; //assert( 0 );
        }
        _char.attrib = 0L;
        if ( _char.autoformat )
            delete _char.autoformat;
        _char.autoformat = 0L;
    }
}

/*================================================================*/
QTextStream& operator<<( QTextStream &out, KWString &_string )
{
    char c = 1;

    for ( unsigned int i = 0; i < _string.size(); i++ ) {
        if ( _string.data()[ i ].c != KWSpecialChar )
            out << QString( _string.data()[ i ].c );
        else
            out << c;
    }

    return out;
}

