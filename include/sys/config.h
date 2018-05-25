#ifndef	_CONFIG_CONFIG_H_
#define	_CONFIG_CONFIG_H_
#include "const.h"

#define	MINOR_BOOT			MINOR_hd2a//可启动分区的次设备号


/*
 * disk log
 */
// #define ENABLE_DISK_LOG//决定了是否打开文件系统log
#define SET_LOG_SECT_SMAP_AT_STARTUP
#define MEMSET_LOG_SECTS
#define	NR_SECTS_FOR_LOG		NR_DEFAULT_FILE_SECTS

#endif
