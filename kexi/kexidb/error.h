#ifndef _KEXI_ERROR_H_
#define _KEXI_ERROR_H_

#define ERR_NONE 0
#define ERR_DRIVERMANAGER 1
#define ERR_MISSING_DB_LOCATION 2
#define ERR_ALREADY_CONNECTED 3
#define ERR_NO_CONNECTION 4
#define ERR_OBJECT_EXISTS 5
#define ERR_OBJECT_NOT_EXISTING 6
#define ERR_ACCESS_RIGHTS 6
#define ERR_TRANSACTION_ACTIVE 7
#define ERR_NO_TRANSACTION_ACTIVE 8
#define ERR_DB_SPECIFIC 9

#define ERR_OTHER 0xffff //! use this if you have not (yet?) the name for given error 
#endif
