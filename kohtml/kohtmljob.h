/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#ifndef __kohtmljob_h__
#define __kohtmljob_h__

#include <kio_job.h>
#include <kio_cache.h>
#include <khtmlview.h>

#include "htmview.h"

class KoHTMLJob: public CachedKIOJob
{
  Q_OBJECT
public:
  enum JobType { HTML, Image };

  KoHTMLJob( KMyHTMLView *_view, const char *_url, const char *_dataURL, JobType _jType, bool _reload );
  ~KoHTMLJob();
  
  void start();
  
  JobType type() const { return m_eJType; }
  const char *url() const { return m_strURL.data(); }
  KMyHTMLView *view() const { return m_pView; }
  
signals:  
  void jobData( KoHTMLJob *job, const char *data, int len, bool eof );
  void jobError( KoHTMLJob *job, const char *errMsg );
  
protected slots:
  void slotJobFinished();
  void slotJobRedirection( int id, const char *_url );
  void slotJobData( int id, const char *data, int len );
  void slotJobListEntry( int id, UDSEntry &entry );
  void slotError( int id, int errid, const char *txt );
  
private:
  KMyHTMLView *m_pView;
  QString m_strURL;
  QString m_strDataURL;
  QString m_strTmpFile;
  JobType m_eJType;
  bool m_bIsFTP;
  bool m_bReload;
  bool m_bListDir;
};

#endif
