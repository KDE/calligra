/*
 *  kis_log.h - part of KImageShop
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


#ifndef __kis_log_h__
#define __kis_log_h__

#include <stream.h>

class KisLog
{
public:
  KisLog() {}

  // write log output to a file instead of cerr
  static void setLogFile(const char *file);

  static ostream &output() { return *m_output; };
  static ostream &log(const char *prefix);

private:

  static ostream *m_output;
  static char    *m_logfile;

};

inline ostream &dbg()   { return KisLog::log("Debug"); };
inline ostream &log()   { return KisLog::log("Log"); };
inline ostream &err()   { return KisLog::log("Error"); };
inline ostream &fatal() { return KisLog::log("Fatal error"); };

#endif
