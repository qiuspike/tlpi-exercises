/*
 * exercise 4.1  version 1
 *
 */
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define BUF_SIZE 4096

void errExit(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
  int choice;
  ssize_t numRead;
  int flags;
  char *buf;
  bool append = false;
  int* fdptrs = NULL;
  int fdind = 0;
  int fcnt = 0;

  while (1)
  {
    static struct option long_options[] =
    {
      {"append", no_argument, 0, 'a'},
      {0,0,0,0}
    };

    choice = getopt_long( argc, argv, "a", long_options, NULL);

    if (choice == -1)
      break;

    switch( choice )
    {
      case 'a':
        append = true;
        break;

      default:
        exit(EXIT_FAILURE);
    }
  }

  /* Deal with non-option arguments here */
  if ( optind < argc )
  {
    fcnt = argc - optind;
    fdptrs = malloc(sizeof(int) * fcnt);
    if (fdptrs == NULL) {
      errExit("malloc");
    }
    while (fdind < fcnt)
    {
      flags = O_RDWR | O_CREAT;
      if (append) {
        flags |= O_APPEND;
      }
      fdptrs[fdind] = open(argv[optind], flags, S_IRUSR | S_IWUSR);
      if (fdptrs[fdind] == -1) {
        errExit("open");
      }
      ++fdind;
      ++optind;
    }
  }

  /* Read from stdin and write to stdout & files */
  buf = malloc(BUF_SIZE);
  while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (write(STDOUT_FILENO, buf, numRead) != numRead)
      errExit("could't write whole buffer");
    if (fdptrs != NULL) {
      fdind = 0;
      while (fdind < fcnt) {
        if (write(fdptrs[fdind], buf, numRead) != numRead)
          errExit("could't write whole buffer");
        ++fdind;
      }
    }
  }
  if (numRead == -1) {
    errExit("read");
  }

  if (fdptrs != NULL) {
    fdind = 0;
    while (fdind < fcnt) {
      if (close(fdptrs[fdind]) == -1)
        errExit("close");
      ++fdind;
    }
  }

  free(buf);
  free(fdptrs);

  exit(EXIT_SUCCESS);
}
