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

#include "kohtmljob.h"
#include "kohtmljob.moc"

#include <sys/stat.h>
#include <unistd.h>

#include <qfile.h>

#include <kurl.h>
#include <kio_error.h>
#include <kapp.h>

#include "kfileio.h"

KoHTMLJob::KoHTMLJob( KMyHTMLView *_view, const char *_url, const char *_dataURL, JobType _jType, bool _reload )
:CachedKIOJob()
{
  m_pView = _view;
  m_strURL = _url;    
  m_strDataURL = _dataURL;
  m_eJType = _jType;
  m_strTmpFile = "";
  m_bReload = _reload;

  KURL u( m_strURL );

  m_bListDir = false;
    
  /*
   * for the ftp protocol we can't use the get method to receive our data
   * because the ftp kioslave currently doesn't support get. So in case
   * the protocol is ftp we just copy the file to a temporary destination.
   */
  m_bIsFTP = (strcasecmp("ftp", u.protocol()) == 0);

  // should we list a directory?
  if ( (m_eJType == HTML) &&
       (strcasecmp("http", u.protocol()) != 0) &&
       (u.filename( false ).isEmpty()) )
     {
       m_bListDir = true;
       
       connect(this, SIGNAL(sigListEntry(int, UDSEntry&)),
               this, SLOT(slotJobListEntry(int, UDSEntry &)));
     }  
  
  setGUImode(NONE);
  
  connect(this, SIGNAL(sigFinished(int)),
          this, SLOT(slotJobFinished()));
  connect(this, SIGNAL(sigError(int, int, const char *)),
          this, SLOT(slotError(int, int, const char *)));	  
  connect(this, SIGNAL(sigData(int, const char *, int)),
          this, SLOT(slotJobData(int, const char *, int)));
  connect(this, SIGNAL(sigRedirection(int, const char *)),
          this, SLOT(slotJobRedirection(int, const char *)));
}

KoHTMLJob::~KoHTMLJob()
{
  cout << "KoHTMLJob::~KoHTMLJob() ( " << m_strURL << " - " << m_strDataURL << " ) " << endl;
  if (!m_strTmpFile.isEmpty())
     unlink(m_strTmpFile.data());
}
  
void KoHTMLJob::start()
{
  if ( m_bListDir )
    {
      KURL u( m_strURL );
    
      listDir( m_strDataURL.data() );

      QString listingBegin = "<html><body><h1>Directory listing of ";
      
      listingBegin += u.path();
      listingBegin += "</h1><table>";
      
      emit jobData( this, listingBegin.data(), listingBegin.length(), false );

      return;
    }

  if ( !m_bIsFTP )
     get(m_strDataURL.data(), m_bReload);
  else	 
     {
       m_strTmpFile = tmpnam(0);
       copy(m_strDataURL.data(), m_strTmpFile.data());
     }
}

void KoHTMLJob::slotJobFinished()
{
  cout << "KoHTMLJob::slotJobFinished()" << endl;

  if ( m_bListDir )
    {
      QString listingEnd = "</table></body></html>";
    
      emit jobData( this, listingEnd.data(), listingEnd.length(), true );
      return;
    }
  
  if ( m_bIsFTP )
     {
       QString m_strData;
     
       QFile f(m_strTmpFile);
       if (f.exists())
          m_strData = kFileToString(m_strTmpFile);
       else
          m_strData = "";
	
       emit jobData( this, m_strData.data(), m_strData.length(), true );
     }
  else
    //hmm...
    emit jobData( this, "", 0, true );     
}

void KoHTMLJob::slotJobRedirection(int id, const char *_url)
{
//  m_strURL = _url;
}

void KoHTMLJob::slotJobData(int id, const char *data, int len)
{
  assert( !m_bIsFTP );
  
  emit jobData( this, data, len, false );
}

void KoHTMLJob::slotJobListEntry( int id, UDSEntry &entry )
{
  UDSEntry::iterator it = entry.begin();

  string name;
  QString size;
  long type = 0;
  
  for (; it != entry.end(); it++ )
    switch ( it->m_uds )
      {
        case UDS_NAME      : name = it->m_str; break;
	case UDS_SIZE      : size.setNum( it->m_long ); break;
	case UDS_FILE_TYPE : type = it->m_long; break;
      }

  QString listing = "<tr><td>";
  listing += "<a href=""";
  listing += m_strURL + "/";
  listing += name.c_str();
  
  if ( S_ISDIR( type ) )
    listing += "/";  
  
  listing += """>";
  listing += name.c_str();
  listing += "</a>";
  listing += "</td><td>";
  listing += size;
  listing += "</td><td>";

  listing += type;
  
  if ( S_ISREG( type ) )
    listing += "file";
  else if ( S_ISDIR( type ) )
    listing += "directory";
  else if ( S_ISCHR( type ) )
    listing += "character device";
  else if ( S_ISBLK( type ) )
    listing += "block device";
  else if ( S_ISFIFO( type ) )
    listing += "fifo";
  else if ( S_ISSOCK( type ) )
    listing += "socket";
  else
    listing += "unknown";    
  
  listing += "</td></tr>";
	
  emit jobData( this, listing.data(), listing.length(), false );
}

void KoHTMLJob::slotError(int id, int errid, const char *txt)
{
  if ( m_eJType == HTML )
     {
       QString msg;
       msg.sprintf(i18n("error while loading:\n%s\nerror message:\n%s"), m_strDataURL.data(), kioErrorString(errid, txt).data());

       emit jobError(this, msg);
     }       

  // YAH (yet another hack) ?
//  emit jobData( this, "", 0, true );
}
