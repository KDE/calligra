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
#ifndef __kohtmljob_h__
#define __kohtmljob_h__

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
  
  JobType getType() { return jType; }
  const char *getURL() { return url.data(); }
  KHTMLView *getParent() { return parent; }
  KHTMLView *getTopParent() { return topParent; }
  
signals:  
  void jobDone(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *filename);
  void jobDone(KoHTMLJob *job, KHTMLView *topParent, KHTMLView *parent, const char *url, const char *data, int len);
  void jobError(const char *errMsg);  
  
protected slots:
  void slotJobFinished();
  void slotJobRedirection(int id, const char *_url);
  void slotJobData(int id, const char *data, int len);
  void slotError(int id, int errid, const char *txt);
  
private:
  KHTMLView *topParent, *parent;
  QString url;
  QString tmpFile;
  JobType jType;
  QString html;
  int htmlLen;
  bool isHTTP;
};

#endif
