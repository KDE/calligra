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

#include <qcache.h>
#include <kio_job.h>
#include <khtmlview.h>

class KoHTMLJob: public KIOJob
{
  Q_OBJECT
public:
  enum JobType { HTML, Image };

  KoHTMLJob(KHTMLView *_topParent, KHTMLView *_parent, const char *_url, JobType _jType);
  ~KoHTMLJob();
  
  void start();
  
  JobType type() { return m_eJType; }
  const char *url() { return m_strURL.data(); }
  KHTMLView *parent() { return m_pParent; }
  KHTMLView *topParent() { return m_pTopParent; }
  
  static bool isCacheEnabled() { return m_sbEnableCache; }
  static void enableCache( bool flag ) { m_sbEnableCache = flag; }
  static QCache<QString> *cache() { return s_jobCache; }
  
  static void initStatic();
  
signals:  
  void jobDone(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename);
  void jobDone(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *data, int len);
  void jobError(const char *errMsg);  
  
protected slots:
  void slotJobFinished();
  void slotJobRedirection(int id, const char *_url);
  void slotJobData(int id, const char *data, int len);
  void slotError(int id, int errid, const char *txt);
  void slotJobSize(int id, unsigned long bytes);
  
private:
  KHTMLView *m_pTopParent, *m_pParent;
  QString m_strURL;
  QString m_strTmpFile;
  JobType m_eJType;
  QString m_strHTML;
  int m_htmlLen;
  bool m_bIsHTTP;
  bool m_bIsCached;
  unsigned int m_sizeInKBytes;

  static bool m_sbEnableCache;  
  static QCache<QString> *s_jobCache;
};

#endif
