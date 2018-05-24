#ifndef	_MINX_FS_H_
#define	_MINX_FS_H_

#include <stdint.h>

/**
 * @struct dev_drv_map fs.h "include/sys/fs.h"
 * @brief  The Device_nr.\ - Driver_nr.\ MAP.
 */
struct dev_drv_map {
	int driver_nr; /**< The proc nr.\ of the device driver. */
};

/**
 * @def   MAGIC_V1
 * @brief Magic number of FS v1.0
 */
#define	MAGIC_V1	0x111

/**
 * @struct super_block fs.h "include/fs.h"
 * @brief  The 2nd sector of the FS
 *
 * Remember to change SUPER_BLOCK_SIZE if the members are changed.
 */
struct super_block {
	uint32_t	magic;		  /**< Magic number */
	uint32_t	nr_inodes;	  /**< How many inodes */
	uint32_t	nr_sects;	  /**< How many sectors */
	uint32_t	nr_imap_sects;	  /**< How many inode-map sectors */
	uint32_t	nr_smap_sects;	  /**< How many sector-map sectors */
	uint32_t	n_1st_sect;	  /**< Number of the 1st data sector */
	uint32_t	nr_inode_sects;   /**< How many inode sectors */
	uint32_t	root_inode;       /**< Inode nr of root directory */
	uint32_t	inode_size;       /**< INODE_SIZE */
	uint32_t	inode_isize_off;  /**< Offset of `struct inode::i_size' */
	uint32_t	inode_start_off;  /**< Offset of `struct inode::i_start_sect' */
	uint32_t	dir_ent_size;     /**< DIR_ENTRY_SIZE */
	uint32_t	dir_ent_inode_off;/**< Offset of `struct dir_entry::inode_nr' */
	uint32_t	dir_ent_fname_off;/**< Offset of `struct dir_entry::name' */

	/*
	 * the following item(s) are only present in memory
	 */
	int	sb_dev; 	/**< the super block's home device */
};

/**
 * @def   SUPER_BLOCK_SIZE
 * @brief The size of super block \b in \b the \b device.
 *
 * Note that this is the size of the struct in the device, \b NOT in memory.
 * The size in memory is larger because of some more members.
 */
#define	SUPER_BLOCK_SIZE	56

/**
 * @struct inode
 * @brief  i-node
 *
 * The \c start_sect and\c nr_sects locate the file in the device,
 * and the size show how many bytes is used.
 * If <tt> size < (nr_sects * SECTOR_SIZE) </tt>, the rest bytes
 * are wasted and reserved for later writing.
 *
 * \b NOTE: Remember to change INODE_SIZE if the members are changed
 */
typedef struct inode {
	uint32_t	i_mode;		/**< Accsess mode */
	uint32_t	i_size;		/**< File size */
	uint32_t	i_start_sect;	/**< The first sector of the data */
	uint32_t	i_nr_sects;	/**< How many sectors the file occupies */
	uint8_t	_unused[16];	/**< Stuff for alignment */

	/* the following items are only present in memory */
	int	i_dev;
	int	i_cnt;		/**< How many procs share this inode  */
	int	i_num;		/**< inode nr.  */
}INODE;

/**
 * @def   INODE_SIZE
 * @brief The size of i-node stored \b in \b the \b device.
 *
 * Note that this is the size of the struct in the device, \b NOT in memory.
 * The size in memory is larger because of some more members.
 */
#define	INODE_SIZE	32

/**
 * @def   MAX_FILENAME_LEN
 * @brief Max len of a filename
 * @see   dir_entry
 */
#define	MAX_FILENAME_LEN	12

/**
 * @struct dir_entry
 * @brief  Directory Entry
 */
struct dir_entry {
	int	inode_nr;		/**< inode nr. */
	char	name[MAX_FILENAME_LEN];	/**< Filename */
};

/**
 * @def   DIR_ENTRY_SIZE
 * @brief The size of directory entry in the device.
 *
 * It is as same as the size in memory.
 */
#define	DIR_ENTRY_SIZE	sizeof(struct dir_entry)


/**
 *	文件描述符表
 */
typedef struct file_desc{
	int 	fd_mode;//读写模式，w/r
	int		fd_pos;//文件读写位置
	INODE	*fd_inode;//指向对应文件的Indoe
}FILE_DESC;
/**
 * Since all invocations of `rw_sector()' in FS look similar (most of the
 * params are the same), we use this macro to make code more readable.
 */
#define RD_SECT(dev,sect_nr) rw_sector(DEV_READ, \
				       dev,				\
				       (sect_nr) * SECTOR_SIZE,		\
				       SECTOR_SIZE, /* read one sector */ \
				       TASK_FS,				\
				       fsbuf);
#define WR_SECT(dev,sect_nr) rw_sector(DEV_WRITE, \
				       dev,				\
				       (sect_nr) * SECTOR_SIZE,		\
				       SECTOR_SIZE, /* write one sector */ \
				       TASK_FS,				\
				       fsbuf);

void init_fs();
void mkfs();
int rw_sector(int io_type, int dev, uint64_t pos, int bytes, int proc_nr,void* buf);
void read_super_block(int dev);
struct super_block * get_super_block(int dev);
struct inode * get_inode(int dev, int num);
void put_inode(struct inode * pinode);
void sync_inode(struct inode * p);
#endif /* _MINX_FS_H_ */