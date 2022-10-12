#ifndef _KERNEL_VERSION_H_
#define _KERNEL_VERSION_H_

/* KERNEL and ZEPHYR_VERSION  values come from cmake/version.cmake
 * BUILD_VERSION  will be 'git describe', alternatively user defined BUILD_VERSION.
 */

#define ZEPHYR_VERSION_CODE 196707
#define ZEPHYR_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))

#define KERNELVERSION          0x3006300
#define KERNEL_VERSION_NUMBER  0x30063
#define KERNEL_VERSION_MAJOR   3
#define KERNEL_VERSION_MINOR   0
#define KERNEL_PATCHLEVEL      99
#define KERNEL_VERSION_STRING  "3.0.99"

#define BUILD_VERSION          v3.0.99-ncs1

#endif /* _KERNEL_VERSION_H_ */
