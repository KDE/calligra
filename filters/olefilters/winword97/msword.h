/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project
 
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

DESCRIPTION

    This file impements an abstraction of the on-disk format of Microsoft 
    Word documents. The public interface implements a iterators that allow
    convenient traversal of the types of data inthe document.

    We also add hand-generated structures and routines to complement all
    the machine-generated ones in our superclass.
*/

#ifndef MSWORD_H
#define MSWORD_H

#include <kdebug.h>
#include <mswordgenerated.h>

class MsWord: protected MsWordGenerated {

public:
    MsWord(
        const U8 *mainStream,
        const U8 *table0Stream,
        const U8 *table1Stream,
        const U8 *dataStream);
    virtual ~MsWord();

    void getText();
    void getPAPX();

protected:
    virtual void gotText(const QString &data) {};

    FIB m_fib;
    const U8 *m_mainStream;
    const U8 *m_tableStream;
    const U8 *m_dataStream;

    // For the grpprl and plcfpcd arrays, we store the offset to the
    // byte count preceeding the first entry, and the number of entries.

    struct
    {
    	U32 byteCountOffset;
        U32 count;
    } m_grpprls;

    struct
    {
    	U32 byteCountOffset;
        U32 count;
    } m_plcfpcds;

    // This class allows standardised treatment of plexes of different types.
    // It is designed to be instantiated locally, once for each plex we are
    // interested in.

    template <class T>
    class Plex
    {
    public:

        // We would like to define the constructor in terms of a callback to
        // take the data within the plex. Unfortunately, egcs-2.91-66 does not
        // support taking the address of bound pointer-to-member.
	//
        // Plex(bool (*callback)(unsigned start, unsigned end, const <T>& data), const U8 *plexPtr);

	Plex(MsWord *client, const U8 *plexPtr) :
	    m_client(client),
	    m_plexPtr(plexPtr)
        {
        };
	void iterate(U32 requestedStart, U32 requestedEnd);
        MsWord *m_client;
        const U8 *m_plexPtr;
    };

    // This class allows standardised treatment of plexes of different types.
    // It is designed to be instantiated locally, once for each plex we are
    // interested in.

    template <class T, class secondaryT>
    class FKP
    {
    public:

        // We would like to define the constructor in terms of a callback to
        // take the data within the plex. Unfortunately, egcs-2.91-66 does not
        // support taking the address of bound pointer-to-member.
	//
        // FKP(bool (*callback)(unsigned start, unsigned end, const <T>& data), const U32 pn);

	FKP(MsWord *client, const U16 pn) :
	    m_client(client),
	    m_pn(pn)
        {
        };
	void iterate(U32 requestedStart, U32 requestedEnd);
        MsWord *m_client;
        const U16 m_pn;
    };

    Plex<PCD> *m_pcd;

    class PAPXBTE: public BTE
    {
    };

    class PAPXFKP
    {
    public:
        U16 istd;
        U16 grpprlBytes;
        U8 *grpprl;
    };

public:
    static unsigned read(const U8 *in, QString *out, unsigned count = 1, bool unicode = true);
    static unsigned read(const U8 *in, PAPXFKP *out, unsigned count = 1);

    // TBD: make these non-public when FKP and Plex are implemented in terms of a callback (see above).

    virtual bool fkpCallback(unsigned start, unsigned end, const PAPXFKP& data);
    virtual bool plexCallback(unsigned start, unsigned end, const PCD& data);
    virtual bool plexCallback(unsigned start, unsigned end, const PAPXBTE& data);
};
#endif

