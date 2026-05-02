#include "stdio.h"
#include "clib.h"
/*
** Flush output buffer for fd to DOS without closing.
** Entry: fd = file descriptor.
** Returns NULL on success, ERR if fd is not open, EOF on write error.
*/
extern int _bufuse[];
int fflush(int fd) {
  if(!_mode(fd)) return (ERR);
  if(_bufuse[fd] == OUT) return (_flush(fd));
  return (NULL);
  }
