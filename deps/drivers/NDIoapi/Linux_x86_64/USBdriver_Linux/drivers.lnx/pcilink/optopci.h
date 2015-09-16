/*
 *  Optotrak/PCI interface driver for Linux
 */

#define OPTOPCI_MAJOR   232

#define OPTOPCI_FIFO_STAT	_IOR(OPTOPCI_MAJOR, 1, int)
#define OPTOPCI_LINK_STAT	_IOR(OPTOPCI_MAJOR, 2, int)
#define OPTOPCI_LINK_RESET	_IO(OPTOPCI_MAJOR, 3)
#define OPTOPCI_CARD_INIT	_IO(OPTOPCI_MAJOR, 4)

#define OPTOTRAK_READ     0x01
#define OPTOTRAK_WRITE    0x10

#define MAX_CARDS    1
