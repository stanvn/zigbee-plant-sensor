#include "status_codes.hpp"
#include <logging/log.h>
#include <zephyr.h>
LOG_MODULE_REGISTER(status_codes, 4);

/// @brief Print status code
/// @detail Print a status code to readable text. For debug usage
/// @param print_status_code
void print_status_code(status_code_t code){
  char text[STATUS_CODE_MAX_TEXT_LEN];
  get_status_code_text(code, text);
  LOG_ERR("%s", text);
}

void get_status_code_text(status_code_t code, char* text){
  switch(code){
    case(STATUS_SUCCESS):
      strcpy(text, "Success");
      break;
    case(STATUS_ERROR_INVALID_LENGTH):
      strcpy(text, "Invalid argument length");
      break;
    case(STATUS_ERROR_INVALID_ADDR):
      strcpy(text, "Invalid address");
      break;
    case(STATUS_ERROR_FORBIDDEN):
      strcpy(text, "Permission denied");
      break;
    case(STATUS_ERROR_ADDR_IN_USE):
      strcpy(text, "Address already in use");
      break;
    case(STATUS_ERROR_ADDR_UNAVAILABLE):
      strcpy(text, "Can't assign requested address. Addres unavailable");
      break;
    case(STATUS_ERROR_IN_PROGRESS):
      strcpy(text, "Operation already in progress");
      break;
    case(STATUS_ERROR_INVALID_FILE):
      strcpy(text, "Invalid file");
      break;
    case(STATUS_ERROR_INVALID_MESSAGE):
      strcpy(text, "Invalid message");
      break;
    case(STATUS_ERROR_BUSY):
      strcpy(text, "Process is busy");
      break;
    case(STATUS_ERROR_CANCELED):
      strcpy(text, "Operation cancled");
      break;
    case(STATUS_ERROR_CONN_ABORTED):
      strcpy(text, "Connection aborted");
      break;
    case(STATUS_ERROR_CONN_REFUSED):
      strcpy(text, "Connection refused");
      break;
    case(STATUS_ERROR_CONN_RESET):
      strcpy(text, "Connection reset by peer");
      break;
    case(STATUS_ERROR_NO_CONN):
      strcpy(text, "Device not connected");
      break;
    case(STATUS_ERROR_NO_DST_ADDR):
      strcpy(text, "Destination address required");
      break;
    case(STATUS_ERROR_TOO_LARGE):
      strcpy(text, "Argument too large");
      break;
    case(STATUS_ERROR_FILE_EXISTS):
      strcpy(text, "File already exists");
      break;
    case(STATUS_ERROR_UNSUPPORTED_ADDR):
      strcpy(text, "Unsuported address");
      break;
    case(STATUS_ERROR_FILE_TOO_LARGE):
      strcpy(text, "File too large");
      break;
    case(STATUS_ERROR_HOST_DOWN):
      strcpy(text, "Host is down");
      break;
    case(STATUS_ERROR_HTTP):
      strcpy(text, "HTTP error");
      break;
    case(STATUS_ERROR_INVALID_ARGUMENT):
      strcpy(text, "Invalid argument(s)");
      break;
    case(STATUS_ERROR_IO):
      strcpy(text, "I/O error");
      break;
    case(STATUS_ERROR_ALREADY_CONNECTED):
      strcpy(text, "Already connected");
      break;
    case(STATUS_ERROR_IS_DIR):
      strcpy(text, "Is a directory");
      break;
    case(STATUS_ERROR_NO_BUF_SPACE):
      strcpy(text, "No buffer space available");
      break;
    case(STATUS_ERROR_NO_DATA):
      strcpy(text, "Missing expected data");
      break;
    case(STATUS_ERROR_INVALID_DEVICE):
      strcpy(text, "No such device or device invalid");
      break;
    case(STATUS_ERROR_INVALID_PATH):
      strcpy(text, "Invalid path, no such file or directory");
      break;
    case(STATUS_ERROR_INVALID_FORMAT):
      strcpy(text, "Invalid argument format");
      break;
    case(STATUS_ERROR_NO_MEM):
      strcpy(text, "No spce left on device");
      break;
    case(STATUS_ERROR_OVERFLOW):
      strcpy(text, "Value/buffer overflow");
      break;
    case(STATUS_ERROR_TIMEOUT):
      strcpy(text, "Timeout");
      break;
    case(STATUS_ERROR_ALREADY_INITIALIZED):
      strcpy(text, "Already initialized");
      break;
    case(STATUS_ERROR_I_AM_A_TEAPOT):
      strcpy(text, "Don't look at me, I'm just a theapot!!");
      break;
    case(STATUS_ERROR_NOT_INITIALIZED):
      strcpy(text, "Service is not yet initialized");
      break;
    case(STATUS_ERROR_DEVICE_NOT_READY):
      strcpy(text, "Device not ready");
      break;
    case(STATUS_ERROR_DEVICE_NOT_FOUND):
      strcpy(text, "Device not found");
      break;
    default:
      strcpy(text, "Unknow error");
      break;
  }
}

void check_error(status_code_t code, const char* msg){
  char text[STATUS_CODE_MAX_TEXT_LEN];
  if(code != STATUS_SUCCESS){
    get_status_code_text(code, text);
    LOG_ERR("%s (Error: %s)", log_strdup(msg), log_strdup(text));
  }
}
