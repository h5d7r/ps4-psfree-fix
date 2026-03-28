//#define DEBUG_SOCKET
#define DEBUG_IP "192.168.2.2"
#define DEBUG_PORT 9023

#include "ps4.h"
#include "PRX.h"
#include "INI.h"

int checkFileExists(const char* filepath) {
  int fd = open(filepath, O_RDONLY, 0);
  if (fd >= 0) {
    close(fd);
    return 1;
  }
  return 0;
}

int checkStringInFile(const char* filepath, const char* searchStr) {
  int fd = open(filepath, O_RDONLY, 0);
  if (fd < 0) return 0;
  char buf[2048];
  int bytesRead = read(fd, buf, sizeof(buf) - 1);
  close(fd);
  if (bytesRead <= 0) return 0;
  buf[bytesRead] = '\0';
  
  for (int i = 0; buf[i] != '\0'; i++) {
    int j = 0;
    while (searchStr[j] != '\0' && buf[i + j] == searchStr[j]) {
      j++;
    }
    if (searchStr[j] == '\0') {
      return 1;
    }
  }
  return 0;
}

void writePRX(){
  printf_notification("Start adding fix, please wait...");
  size_t len = 0;
  unsigned char* fbuf = base64_decode(PRXStr, sizeof(PRXStr), &len);
  sceKernelSleep(5);
	if (len != 0){
    mkdir("/data/GoldHEN/plugins", 0777);
    int fid = open("/data/GoldHEN/plugins/aio_fix_505.prx", O_WRONLY | O_CREAT | O_TRUNC, 0777);
    write(fid, fbuf, len);
    close(fid);
    sceKernelSleep(5);
    printf_notification("PRX fix install complete");
	}else{
		printf_notification("ERROR:\nNo PRX file found");
	}
}

void writeINI(){
  printf_notification("Start adding ini, please wait...");
  size_t len = 0;
  unsigned char* fbuf = base64_decode(INIStr, sizeof(INIStr), &len);
  sceKernelSleep(5);
	if (len != 0){
    mkdir("/data/GoldHEN", 0777);
    int fid = open("/data/GoldHEN/plugins.ini", O_WRONLY | O_CREAT | O_APPEND, 0777);
    write(fid, fbuf, len);
    close(fid);
    sceKernelSleep(5);
    printf_notification("INI file install complete");
	}else{
		printf_notification("ERROR:\nNo INI file found");
	}
}

int _main(struct thread *td) {
  UNUSED(td);

  initKernel();
  initLibc();

#ifdef DEBUG_SOCKET
  initNetwork();
  DEBUG_SOCK = SckConnect(DEBUG_IP, DEBUG_PORT);
#endif

  jailbreak();

  initSysUtil();

  char fw_version[6] = {0};
  get_firmware_string(fw_version);
  printf_notification("PSFree-Fix\nPS4 Firmware %s", fw_version);
  
  int prx_exists = checkFileExists("/data/GoldHEN/plugins/aio_fix_505.prx");
  int ini_has_path = checkStringInFile("/data/GoldHEN/plugins.ini", "aio_fix_505.prx");

  if (prx_exists && ini_has_path) {
    printf_notification("Fix is already installed");
  } else {
    if (!ini_has_path) {
      writeINI();
    }
    if (!prx_exists) {
      writePRX();
    }
  }

#ifdef DEBUG_SOCKET
  printf_debug("Closing socket...\n");
  SckClose(DEBUG_SOCK);
#endif

  return 0;
}
