//Arthor: Hongyang Li
//This is the source module for lab4b of CS111.

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <math.h>
#include "mraa/aio.h"

int log_flag = 0;
int log_fd;

mraa_aio_context tmp;
mraa_gpio_context btn;

void Shutdown() {
	time_t rawtime;
  struct tm *info;
  char time_str[9];

	time(&rawtime);
  info = localtime(&rawtime);
  strftime(time_str, 9, "%H:%M:%S", info);
  if (log_flag) {
    dprintf(log_fd, "%s SHUTDOWN\n", time_str);
  } else {
    printf("%s SHUTDOWN\n", time_str);
  }
  mraa_aio_close(tmp);
  mraa_gpio_close(btn);
  exit(0);
}

int main(int argc, char *argv[]) {

	static struct option args[] = {
    {"period", 1, NULL, 'p'},
    {"scale", 1, NULL, 's'},
    {"log", 1, NULL, 'l'},
    {0, 0, 0, 0}
  };


  int scale_flag = 0;
  int period = 1;
  int arg_get;

  while ((arg_get = getopt_long(argc, argv, "", args, NULL)) != -1) {
    switch(arg_get) {
      case 'p': {
        period = atoi(optarg);
        break;
      }
      case 'l': {
        log_flag = 1;
        log_fd = open(optarg, O_CREAT | O_NONBLOCK | O_APPEND | O_WRONLY, 0666);
        break;
      }
      case 's': {
        if (!strcmp(optarg, "C")) {
        	scale_flag = 1;
        	break;
        } else {
        	if (!strcmp(optarg, "F"))
        		break;
        }
      }
      default: {
        printf("Please enter correct commands as shown below!\n");
        printf("  --period=# ... specify a sampling interval in seconds\n");
        printf("  --scale=C/F ... temperatures reported in Celsius or Fahrenheit\n");
        printf("  --log=pathname ... append report to a logfile\n");
        fprintf(stderr, "unrecognized argument\n");
        exit(1);
      }
    }
  }

  const int B = 4275;
  const int R0 = 100000;


  tmp = mraa_aio_init(0);
  btn = mraa_gpio_init(3);
  if (tmp == NULL) {
  	fprintf(stderr, "mraa_aio_init() fail\n");
  	exit(1);
  }
  if (btn == NULL) {
  	fprintf(stderr, "mraa_gpio_init() fail\n");
  	mraa_aio_close(tmp);
  	exit(1);
  }

  mraa_gpio_dir(btn, MRAA_GPIO_IN);

  struct pollfd pfd[1];
  pfd[0].fd = 0;
  pfd[0].events = POLLIN | POLLERR;

  time_t rawtime;
  struct tm *info;
  char time_str[9];
  char buffer[100];
  bzero(buffer, 100);

  while (1) {
    ret_poll = poll(pfd, 1, 0);
    if (ret_poll == -1) {
      fprintf(stderr, "poll() failed: %s\n", strerror(errno));
      mraa_aio_close(tmp);
      exit(1);
    } else {
      if (ret_poll == 1) {
        if (pfd[0].revents & POLLIN) {
          int read_count = read(pfd[0].fd, buffer, 100);
          if (read_count == -1) {
            fprintf(stderr, "read() failed: %s\n", strerror(errno));
            mraa_aio_close(tmp);
            mraa_gpio_close(btn);
            exit(1);
          } else {
          }
        }
        if (pfd[0].revents & POLLERR) {
        	fprintf(stderr, "read() failed: %s\n", strerror(errno));
          mraa_aio_close(tmp);
          mraa_gpio_close(btn);
          exit(1);
        }
      } else {
      	int btn_status = mraa_gpio_read(btn);
      	if (btn) {
      		if (btn > 0) {
      			Shutdown();
      		}
      	}
      }
    }
  }

  if (log_flag) dprintf(log_fd, "abcdefg\n");
  return 0;
}