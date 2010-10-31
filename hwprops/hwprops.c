#include <string.h>
#include <fcntl.h>
//#include <sys/mount.h>

/* Set baseband and serialnumber properties on FIH devices, since they're
 * not exported by the bootloader to cmdline as expected */


/* Ugly hack is ugly...
 *
 * There's a very narrow window in which the hardware properties
 * can be written, since init sets them immediately after launching
 * the property service, and they can only be set once.
 * So our only chance to overwrite them is to set them during that 
 * launch, by inserting them into default.prop */

int main() {
	int fd;
	char b[256];
	int bc;

	//mount("rootfs","/","rootfs",MS_REMOUNT|0,NULL);

	fd = open("/proc/baseband",O_RDONLY);
	if (fd<0)
		return 0;

	bc = read(fd, b, sizeof(b)-1);
	close(fd);
	fd = open("/default.prop", O_WRONLY|O_APPEND);
	if (fd>0){
		write(fd,"ro.baseband=",12);
		write(fd,b,strlen(b));
		close (fd);
	}

	fd = open("/proc/serialnumber",O_RDONLY);
	if (fd<0)
		return 0;

	bc = read(fd, b, sizeof(b)-1);
	close(fd);
	fd = open("/default.prop", O_WRONLY|O_APPEND);
	if (fd>0){
		write(fd,"ro.serialno=",12);
		write(fd,b,strlen(b));
		close (fd);
	}

	//mount("rootfs","/","rootfs",MS_REMOUNT|MS_RDONLY,NULL);
	return 0;
}
