/*
 *  kis_log.cc - part of Krayon
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "kis_log.h"

// output to cerr if no logfile is set
ostream *KisLog::m_output = &cerr;
char    *KisLog::m_logfile = 0;

void KisLog::setLogFile(const char *file)
{
    // remove old logfile
    ::unlink(file);

    // delete old output stream
    if (m_logfile)
        delete m_output;

    // open new output
    m_output = new ofstream(file, ofstream::app);
    m_logfile = strdup(file);
  
    // fall back to cerr
    if (!m_output)
    {
        m_output = &cerr;
        m_logfile = 0;
    }
}
