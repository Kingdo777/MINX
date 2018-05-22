#include "process.h"
#include "kliba.h"
#include "ipc.h"
#include "const.h"
#include "mystring.h"

void task_fs(){
    printl("Task FS begins.\n");
	/* open the device: hard disk */
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;
	send_recv(BOTH, TASK_HD, &driver_msg);

	spin("FS");
}