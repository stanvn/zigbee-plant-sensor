#pragma once 

#include <zephyr.h>
#include <stdint.h>

#define STATUS_CODE_MAX_TEXT_LEN 75

typedef uint16_t status_code_t;

#define STATUS_SUCCESS                    0               // Success!!
#define STATUS_ERROR_ADDR_IN_USE          EADDRINUSE      // Address already in use
#define STATUS_ERROR_ADDR_UNAVAILABLE     EADDRNOTAVAIL   // Can't assign requested address
#define STATUS_ERROR_ALREADY_CONNECTED    EISCONN         // Already connected
#define STATUS_ERROR_ALREADY_INITIALIZED  141             // Already initialized
#define STATUS_ERROR_BUSY                 EBUSY           // Process busy
#define STATUS_ERROR_CANCELED             ECANCELED       // Operation cancled
#define STATUS_ERROR_CONN_ABORTED         ECONNABORTED    // Connection aborted
#define STATUS_ERROR_CONN_REFUSED         ECONNREFUSED    // Connection refused
#define STATUS_ERROR_CONN_RESET           ECONNRESET      // Connection reset by peer
#define STATUS_ERROR_DEVICE_NOT_READY     142             // Device not ready
#define STATUS_ERROR_DEVICE_NOT_FOUND     143             // Device not found
#define STATUS_ERROR_FILE_EXISTS          EEXIST          // File already exists
#define STATUS_ERROR_FILE_TOO_LARGE       EFBIG           // File to large
#define STATUS_ERROR_FORBIDDEN            EACCES          // Premission denied
#define STATUS_ERROR_HOST_DOWN            EHOSTDOWN       // Host is down
#define STATUS_ERROR_HTTP                 145             // HTTP error
#define STATUS_ERROR_I_AM_A_TEAPOT        418             // I'm a teapot
#define STATUS_ERROR_INVALID_ADDR         EFAULT          // Bad address
#define STATUS_ERROR_INVALID_ARGUMENT     EINVAL          // Invalid argument
#define STATUS_ERROR_INVALID_DEVICE       ENODEV          // No such device or device invalid
#define STATUS_ERROR_INVALID_FILE         EBADF           // Bad file number
#define STATUS_ERROR_INVALID_FORMAT       ENOEXEC         // Invalid argument format
#define STATUS_ERROR_INVALID_LENGTH       E2BIG           // Arg list too long
#define STATUS_ERROR_INVALID_MESSAGE      EBADMSG         // Invalid (STREAMS) message
#define STATUS_ERROR_INVALID_PATH         ENOENT          // Invalid path, no such file or directory
#define STATUS_ERROR_IN_PROGRESS          EALREADY        // Operation already in progress
#define STATUS_ERROR_IO                   EIO             // I/O error
#define STATUS_ERROR_IS_DIR               EISDIR          // Is a directory
#define STATUS_ERROR_NO_BUF_SPACE         ENOBUFS         // No buffer space available
#define STATUS_ERROR_NO_CONN              ENOTCONN        // Device not connected
#define STATUS_ERROR_NO_DATA              ENODATA         // Missing expected data
#define STATUS_ERROR_NO_DST_ADDR          EDESTADDRREQ    // Destination address required
#define STATUS_ERROR_NO_MEM               ENOSPC          // No space left on device 
#define STATUS_ERROR_NOT_INITIALIZED      144             // Not initialized
#define STATUS_ERROR_OVERFLOW             EOVERFLOW       // Value/buffer overflow
#define STATUS_ERROR_TIMEOUT              ETIMEDOUT       // General timeout
#define STATUS_ERROR_TOO_LARGE            EDOM            // Argument too large
#define STATUS_ERROR_UNSUPPORTED_ADDR     EAFNOSUPPORT    // Unsuported address

void print_status_code(status_code_t code);
void get_status_code_text(status_code_t code, char* text);
void check_error(status_code_t code, const char* msg);
