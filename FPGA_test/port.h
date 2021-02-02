	/*
 * port.h
 *
 *  Created on: 2011-2-15
 *      Author: Administrator
 */

#ifndef PORT_H_
#define PORT_H_

//port的ioctl指令
#define PORT_IOC_MAGIC			'p'
#define IOCTL_PORT_SET_UP		_IOW(PORT_IOC_MAGIC, 1, unsigned char)
#define IOCTL_PORT_READ   		_IOW(PORT_IOC_MAGIC, 2, unsigned char)
#define IOCTL_PORT_WRITE		_IOW(PORT_IOC_MAGIC, 3, unsigned char)

//读写宽度
typedef enum {
	PORT_WIDTH_8 = 0,
	PORT_WIDTH_16,
	PORT_WIDTH_32
} port_wid_t;

//合法的BANK编号
typedef enum {
	PORT_BANK_2 = 0,
	PORT_BANK_4,
	PORT_BANK_5,
	PORT_BANK_6,
	PORT_BANK_7,
} port_bank_t;

//BANK属性
typedef struct {
	port_bank_t bank;
	port_wid_t width;
	short reserved;
} port_attr_t;

//
typedef struct {
	int regAddr;
	int regData;
} port_data_t;

#endif /* PORT_H_ */
