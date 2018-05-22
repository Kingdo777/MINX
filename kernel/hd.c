#include <stdint.h>
#include "const.h"
#include "protect.h"
#include "mystring.h"
#include "process.h"
#include "tty.h"
#include "global.h"
#include "interrupt.h"
#include "clock.h"
#include "kliba.h"
#include "ipc.h"
#include "hd.h"
#include "system_call.h"

uint8_t hd_status;
uint8_t hdbuf[SECTOR_SIZE * 2];

/*****************************************************************************
 *                                task_hd
 *****************************************************************************/
/**
 * Main loop of HD driver.
 * 
 *****************************************************************************/
void task_hd()
{
    MESSAGE msg;

    init_hd();

    while (1)
    {
        send_recv(RECEIVE, ANY, &msg);

        int src = msg.source;

        switch (msg.type)
        {
        case DEV_OPEN:
            hd_identify(0);
            break;

        default:
            dump_msg("HD driver::unknown msg", &msg);
            spin("FS::main_loop (invalid msg.type)");
            break;
        }

        send_recv(SEND, src, &msg);
    }
}

/*****************************************************************************
 *                                init_hd
 *****************************************************************************/
/**
 * <Ring 1> Check hard drive, set IRQ handler, enable IRQ and initialize data
 *          structures.
 *****************************************************************************/
void init_hd()
{
    /* Get the number of drives from the BIOS data area */
    uint8_t *pNrDrives = (uint8_t *)(0x475);//打印硬盘数目
    printl("NrDrives:%d.\n", *pNrDrives);
    assert(*pNrDrives);

    set_irq_table(AT_WINI_IRQ, hd_handler);//打开硬盘硬件中断

}

/*****************************************************************************
 *                                hd_identify
 *****************************************************************************/
/**
 * <Ring 1> Get the disk information.
 * 
 * @param drive  Drive Nr.
 *****************************************************************************/
void hd_identify(int drive)
{
    struct hd_cmd cmd;
    cmd.device = MAKE_DEVICE_REG(1, drive, 0);
    cmd.command = ATA_IDENTIFY;
    hd_cmd_out(&cmd);
    interrupt_wait();
    port_read(REG_DATA, hdbuf, SECTOR_SIZE);

    print_identify_info((uint16_t *)hdbuf);
}

/*****************************************************************************
 *                            print_identify_info
 *****************************************************************************/
/**
 * <Ring 1> Print the hdinfo retrieved via ATA_IDENTIFY command.
 * 
 * @param hdinfo  The buffer read from the disk i/o port.
 *****************************************************************************/
void print_identify_info(uint16_t *hdinfo)
{
    int i, k;
    char s[64];

    struct iden_info_ascii
    {
        int idx;
        int len;
        char *desc;
    } iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
                 {27, 40, "HD Model"} /* Model number in ASCII */};

    for (k = 0; k < sizeof(iinfo) / sizeof(iinfo[0]); k++)
    {
        char *p = (char *)&hdinfo[iinfo[k].idx];
        for (i = 0; i < iinfo[k].len / 2; i++)
        {
            s[i * 2 + 1] = *p++;
            s[i * 2] = *p++;
        }
        s[i * 2] = 0;
        printl("%s: %s\n", iinfo[k].desc, s);
    }

    int capabilities = hdinfo[49];
    printl("LBA supported: %s\n",
           (capabilities & 0x0200) ? "Yes" : "No");

    int cmd_set_supported = hdinfo[83];
    printl("LBA48 supported: %s\n",
           (cmd_set_supported & 0x0400) ? "Yes" : "No");

    int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
    printl("HD size: %dMB\n", sectors * 512 / 1000000);
}

/*****************************************************************************
 *                                hd_cmd_out
 *****************************************************************************/
/**
 * <Ring 1> Output a command to HD controller.
 * 
 * @param cmd  The command struct ptr.
 *****************************************************************************/
void hd_cmd_out(struct hd_cmd *cmd)
{
    /**
	 * For all commands, the host must first check if BSY=1,
	 * and should proceed no further unless and until BSY=0
	 */
    if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT))
        panic("hd error.");

    /* Activate the Interrupt Enable (nIEN) bit */
    out_port(REG_DEV_CTRL, 0);
    /* Load required parameters in the Command Block Registers */
    out_port(REG_FEATURES, cmd->features);
    out_port(REG_NSECTOR, cmd->count);
    out_port(REG_LBA_LOW, cmd->lba_low);
    out_port(REG_LBA_MID, cmd->lba_mid);
    out_port(REG_LBA_HIGH, cmd->lba_high);
    out_port(REG_DEVICE, cmd->device);
    /* Write the command code to the Command Register */
    out_port(REG_CMD, cmd->command);
}

/*****************************************************************************
 *                                interrupt_wait
 *****************************************************************************/
/**
 * <Ring 1> Wait until a disk interrupt occurs.
 * 
 *****************************************************************************/
void interrupt_wait()
{
    MESSAGE msg;
    send_recv(RECEIVE, INTERRUPT, &msg);
}

/*****************************************************************************
 *                                waitfor
 *****************************************************************************/
/**
 * <Ring 1> Wait for a certain status.
 * 
 * @param mask    Status mask.
 * @param val     Required status.
 * @param timeout Timeout in milliseconds.
 * 
 * @return One if sucess, zero if timeout.
 *****************************************************************************/
int waitfor(int mask, int val, int timeout)
{
    int t = get_ticks();

    while (((get_ticks() - t) * 1000 / HZ) < timeout)
        if ((in_port(REG_STATUS) & mask) == val)
            return 1;

    return 0;
}

/*****************************************************************************
 *                                hd_handler
 *****************************************************************************/
/**
 * <Ring 0> Interrupt handler.
 * 
 * @param irq  IRQ nr of the disk interrupt.
 *****************************************************************************/
void hd_handler(int irq)
{
    /*
	 * Interrupts are cleared when the host
	 *   - reads the Status Register,
	 *   - issues a reset, or
	 *   - writes to the Command Register.
	 */
    hd_status = in_port(REG_STATUS);

    inform_int(TASK_HD);
}