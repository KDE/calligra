/*
 *  Copyright (c) 2015 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_color_manager.h"

#include <kglobal.h>
#include <list>

#include <Windows.h>
#include <Shlobj.h>
#include <Icm.h>

#include "lcms2.h"

#include <QDesktopWidget>
#include <QApplication>

class KisColorManager::Private {
public:
    // we are too dumb for that :)


    bool GetMonitorProfile(char *path, int buf_len, const void *hwnd)
    {
        std::list<std::string> profile_descriptions;
        std::map<std::string, std::string> profile_paths;

        // path to the monitor's profile
        char monitor_profile_path[512] = { '\0' };
        DWORD path_size = 512;
        BOOL get_icm_result = GetICMProfile(GetDC((HWND)hwnd), &path_size, monitor_profile_path);
        Q_ASSERT(get_icm_result);

        // directory where Windows stores its profiles
        char profile_directory[512] = { '\0' };
        DWORD dir_name_size = 512;
        BOOL get_color_dir_result = GetColorDirectory(NULL, profile_directory, &dir_name_size);
        Q_ASSERT(get_color_dir_result);

        // Get the profile file names from Windows
        ENUMTYPE enum_type;
        enum_type.dwSize = sizeof(ENUMTYPE);
        enum_type.dwVersion = ENUM_TYPE_VERSION;
        enum_type.dwFields = ET_DEVICECLASS;  // alternately could use ET_CLASS
        enum_type.dwDeviceClass = CLASS_MONITOR;

        BYTE *buf = NULL;
        DWORD buf_size = 0;
        DWORD num_profiles = 0;

        BOOL other_enum_result = EnumColorProfiles(NULL, &enum_type,
                                                   buf, &buf_size, &num_profiles);

        if (buf_size > 0 && num_profiles > 0)
        {
            buf = (BYTE *)malloc(buf_size);

            other_enum_result = EnumColorProfiles(NULL, &enum_type,
                                                  buf, &buf_size, &num_profiles);

            if (other_enum_result)
            {
                // build a list of the profile descriptions
                // and a map to return the paths
                char *prof_name = (char *)buf;

                for(int i=0; i < num_profiles; i++)
                {
                    std::string prof = prof_name;
                    std::string prof_path = std::string(profile_directory) + "\\" + prof_name;

                    cmsHPROFILE hProfile = cmsOpenProfileFromFile(prof_path.c_str(), "r");

                    // Note: Windows will give us profiles that aren't ICC (.cdmp for example).
                    // Don't worry, LittleCMS will just return NULL for those.
                    if (hProfile)
                    {
                        char profile_description[256];

                        cmsUInt32Number got_desc = cmsGetProfileInfoASCII(hProfile,
                                                                          cmsInfoDescription,
                                                                          "en", "US",
                                                                          profile_description,
                                                                          256);

                        if (got_desc)
                        {
                            profile_descriptions.push_back(profile_description);

                            profile_paths[ profile_description ] = prof_path;
                        }

                        cmsCloseProfile(hProfile);
                    }

                    prof_name += strlen(prof_name) + 1;
                }
            }

            free(buf);
        }


        if (profile_descriptions.size() > 0)
        {
            // set a vector and selected index for building the profile menu
            profile_descriptions.sort();
            profile_descriptions.unique();

            std::vector<std::string> profile_vec;
            int selected = 0;

            for(std::list<std::string>::const_iterator i = profile_descriptions.begin(); i != profile_descriptions.end(); i++)
            {
                profile_vec.push_back( *i );

                if ( profile_paths[ *i ] == monitor_profile_path)
                {
                    selected = profile_vec.size() - 1;
                }
            }

            // run the dialog
            g_profile_vec = &profile_vec;
            g_selected_item = selected;

            int status = DialogBox(hDllInstance, (LPSTR)"PROFILEDIALOG",
                                   (HWND)hwnd, (DLGPROC)DialogProc);


            if (status == -1)
            {
                // dialog didn't open, my bad
                return true;
            }
            else if (g_item_clicked == DLOG_Cancel)
            {
                return false;
            }
            else
            {
                strncpy(path, profile_paths[ profile_vec[ g_selected_item ] ].c_str(), buf_len);

                return true;
            }
        }
        else
            return true;
    }

};




KisColorManager::KisColorManager()
    : QObject()
    , d(new Private())
{
}

KisColorManager::~KisColorManager()
{
}

QString KisColorManager::deviceName(const QString &id)
{
    return QString();
}

QStringList KisColorManager::devices(DeviceType type) const
{
    QDesktopWidget *desktop = qApp->desktop();

}

QByteArray KisColorManager::displayProfile(const QString &device, int profile) const
{
    return QByteArray();
}

KisColorManager *KisColorManager::instance()
{
    K_GLOBAL_STATIC(KisColorManager, s_instance);
    return s_instance;
}

#include "kis_color_manager.moc"
