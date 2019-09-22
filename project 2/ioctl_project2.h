#include <linux/ioctl.h>

#define IO_KEY 'e'

#define INCLUDE_READ _IOW(IO_KEY,0,int)
#define UNREAD_LIMIT _IOW(IO_KEY,1,int)
#define DELETE_MESSAGES _IOW(IO_KEY,2,char*)

