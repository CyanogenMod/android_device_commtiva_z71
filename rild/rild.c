/* //device/system/rild/rild.c
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <telephony/ril.h>
#define LOG_TAG "RILD"
#include <utils/Log.h>
#include <cutils/properties.h>
#include <cutils/sockets.h>
#include <linux/capability.h>
#include <linux/prctl.h>

#include <private/android_filesystem_config.h>

#define LIB_PATH_PROPERTY   "rild.libpath"
#define LIB_ARGS_PROPERTY   "rild.libargs"
#define MAX_LIB_ARGS        16

static void usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s -l <ril impl library> [-- <args for impl library>]\n", argv0);
    exit(-1);
}

extern void RIL_register (const RIL_RadioFunctions *callbacks);

extern void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
                           void *response, size_t responselen);

extern void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen);

extern void RIL_requestTimedCallback (RIL_TimedCallback callback,
                               void *param, const struct timeval *relativeTime);


typedef struct {
    int requestNumber;
    void *dispatchFunction;
    int *responseFunction;
} CommandInfo;

typedef struct RequestInfo {
    int32_t token;      //this is not RIL_Token
    CommandInfo *pCI;
    struct RequestInfo *p_next;
    char cancelled;
    char local;         // responses to local commands do not go back to command process
} RequestInfo;

char *getNextValueFromAtLine(char **atresponse) {
	char *value = NULL;

	char *readbuf = *atresponse;

	while (*readbuf != '+' && *readbuf !='\0') {
		readbuf++;
	}
	if (strlen(readbuf)) {
		value = readbuf;
		while (*value != '"' && strlen(value)) {
			value++;
		}
		value+=1;
		readbuf=value;
		while (*readbuf != '"' && strlen(readbuf)) {
			readbuf++;
		}
		*readbuf='\0';
		readbuf++;
	}

	*atresponse = readbuf;
	return value;
}


void getNetworksFromModem(char **response) {
	struct termios  ios;
	char sync_buf[256];
	char *readbuf = sync_buf;
	char *states[4] = {"unknown","available","current","forbidden"}; 
	int seen[16] = {0,}; /* Max 16 operators */
	int old_flags, i, networkCount=0;
	int fd=open("/dev/smd0",O_RDWR);

	if (fd<=0) {
		return;
	}
	tcgetattr( fd, &ios );
	ios.c_lflag = 0;
	tcsetattr( fd, TCSANOW, &ios );
	old_flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, old_flags | O_NONBLOCK);

	write(fd,"AT+COPS=?\r",10);
	sleep(1);	

	if (read(fd,sync_buf,sizeof(sync_buf))) {
		sync_buf[255]='\0';
		char *readbuf = sync_buf;
		while (strlen(readbuf)) {
			char *output[4];
			while (*readbuf != '(' && *readbuf !='\0') {
				readbuf++;
			}
			output[0]=++readbuf;
			while (*readbuf != ',') {
				readbuf++;
			}
			*readbuf='\0'; readbuf+=2;
			output[1]=readbuf;
			while (*readbuf != ',') {
				readbuf++;
			}
			*--readbuf='\0'; readbuf+=3;
			output[2]=readbuf;
			while (*readbuf != ',') {
				readbuf++;
			}
			*--readbuf='\0'; readbuf+=3;
			output[3]=readbuf;
			while (*readbuf != ',') {
				readbuf++;
			}
			*--readbuf='\0'; 
			while (*readbuf != ')') {
				readbuf++;
			}
			*readbuf++='\0';

			int numericOperator = atoi(output[3]);
			for (i=0; i<16 && seen[i]!=0; i++) {
				if (seen[i] == numericOperator)
					goto skipop;
			}
			seen[networkCount] = numericOperator;

			response[(networkCount*4)+0]=strdup(output[1]);
			response[(networkCount*4)+1]=strdup(output[2]);
			response[(networkCount*4)+2]=strdup(output[3]);
			response[(networkCount*4)+3]=strdup(states[atoi(output[0])]);
		
			networkCount++;
skipop:	
			if (!strncmp(readbuf,",,",2)) {
				break;
			}
	
		}
	}
	close(fd);
}

void getOperatorFromModem(char **p_cur) {
    struct termios  ios;
    char sync_buf[256];
    char *readbuf = sync_buf;
    char *plmn=NULL;
    char *spn=NULL;
	int fd=open("/dev/smd0",O_RDWR);
    int old_flags;

	if (fd<=0) {
		return;
	}
    old_flags = fcntl(fd, F_GETFL, 0);
	tcgetattr( fd, &ios );
    ios.c_lflag = 0;
    tcsetattr( fd, TCSANOW, &ios );

	write(fd,"AT+COPS=3,0;+COPS?;+COPS=3,1;+COPS?\r",36);
	sleep(1);
    read(fd,sync_buf,sizeof(sync_buf));
    if (strlen(sync_buf)) {
        /* Skip first echoed line */
        while (*readbuf != '\n' && *readbuf !='\0') {
            readbuf++;
        }
        /* Find PLMN */
        plmn = getNextValueFromAtLine(&readbuf);
        /* Find SPN */
        spn = getNextValueFromAtLine(&readbuf);

		if (strlen(plmn) && strlen(spn)) {
			p_cur[0]=strdup(plmn);
			p_cur[1]=strdup(spn);
		}
	}
	close(fd);
}

void
RIL_InterceptOnRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
    RequestInfo *pRI;
    pRI = (RequestInfo *)t;

	if (pRI->pCI->requestNumber == RIL_REQUEST_OPERATOR) {
		char **p_cur = (char **) response;
		if (!strncmp(p_cur[0],"Unknown",7)) {
			getOperatorFromModem(p_cur);
		}
	} else if (pRI->pCI->requestNumber == RIL_REQUEST_QUERY_AVAILABLE_NETWORKS) {
		getNetworksFromModem((char **)response);
	} else if (pRI->pCI->requestNumber == RIL_REQUEST_BASEBAND_VERSION) {
		char baseband[PROPERTY_VALUE_MAX];

		property_get("ro.baseband", baseband, "QCT unknown");
		response=strdup(baseband);
	}

	RIL_onRequestComplete(t,e,response,responselen);
}

static struct RIL_Env s_rilEnv = {
    RIL_InterceptOnRequestComplete,
    RIL_onUnsolicitedResponse,
    RIL_requestTimedCallback
};

extern void RIL_startEventLoop();

static int make_argv(char * args, char ** argv)
{
    // Note: reserve argv[0]
    int count = 1;
    char * tok;
    char * s = args;

    while ((tok = strtok(s, " \0"))) {
        argv[count] = tok;
        s = NULL;
        count++;
    }
    return count;
}

/*
 * switchUser - Switches UID to radio, preserving CAP_NET_ADMIN capabilities.
 * Our group, cache, was set by init.
 */
void switchUser() {
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    setuid(AID_RADIO);

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;
    cap.effective = cap.permitted = 1 << CAP_NET_ADMIN;
    cap.inheritable = 0;
    capset(&header, &cap);
}

int main(int argc, char **argv)
{
    const char * rilLibPath = NULL;
    char **rilArgv;
    void *dlHandle;
    const RIL_RadioFunctions *(*rilInit)(const struct RIL_Env *, int, char **);
    const RIL_RadioFunctions *funcs;
    char libPath[PROPERTY_VALUE_MAX];
    unsigned char hasLibArgs = 0;

    int i;

    for (i = 1; i < argc ;) {
        if (0 == strcmp(argv[i], "-l") && (argc - i > 1)) {
            rilLibPath = argv[i + 1];
            i += 2;
        } else if (0 == strcmp(argv[i], "--")) {
            i++;
            hasLibArgs = 1;
            break;
        } else {
            usage(argv[0]);
        }
    }

    if (rilLibPath == NULL) {
        if ( 0 == property_get(LIB_PATH_PROPERTY, libPath, NULL)) {
            // No lib sepcified on the command line, and nothing set in props.
            // Assume "no-ril" case.
            goto done;
        } else {
            rilLibPath = libPath;
        }
    }

    /* special override when in the emulator */
#if 1
    {
        static char*  arg_overrides[3];
        static char   arg_device[32];
        int           done = 0;

#define  REFERENCE_RIL_PATH  "/system/lib/libreference-ril.so"

        /* first, read /proc/cmdline into memory */
        char          buffer[1024], *p, *q;
        int           len;
        int           fd = open("/proc/cmdline",O_RDONLY);

        if (fd < 0) {
            LOGD("could not open /proc/cmdline:%s", strerror(errno));
            goto OpenLib;
        }

        do {
            len = read(fd,buffer,sizeof(buffer)); }
        while (len == -1 && errno == EINTR);

        if (len < 0) {
            LOGD("could not read /proc/cmdline:%s", strerror(errno));
            close(fd);
            goto OpenLib;
        }
        close(fd);

        if (strstr(buffer, "android.qemud=") != NULL)
        {
            /* the qemud daemon is launched after rild, so
            * give it some time to create its GSM socket
            */
            int  tries = 5;
#define  QEMUD_SOCKET_NAME    "qemud"

            while (1) {
                int  fd;

                sleep(1);

                fd = socket_local_client(
                            QEMUD_SOCKET_NAME,
                            ANDROID_SOCKET_NAMESPACE_RESERVED,
                            SOCK_STREAM );

                if (fd >= 0) {
                    close(fd);
                    snprintf( arg_device, sizeof(arg_device), "%s/%s",
                                ANDROID_SOCKET_DIR, QEMUD_SOCKET_NAME );

                    arg_overrides[1] = "-s";
                    arg_overrides[2] = arg_device;
                    done = 1;
                    break;
                }
                LOGD("could not connect to %s socket: %s",
                    QEMUD_SOCKET_NAME, strerror(errno));
                if (--tries == 0)
                    break;
            }
            if (!done) {
                LOGE("could not connect to %s socket (giving up): %s",
                    QEMUD_SOCKET_NAME, strerror(errno));
                while(1)
                    sleep(0x00ffffff);
            }
        }

        /* otherwise, try to see if we passed a device name from the kernel */
        if (!done) do {
#define  KERNEL_OPTION  "android.ril="
#define  DEV_PREFIX     "/dev/"

            p = strstr( buffer, KERNEL_OPTION );
            if (p == NULL)
                break;

            p += sizeof(KERNEL_OPTION)-1;
            q  = strpbrk( p, " \t\n\r" );
            if (q != NULL)
                *q = 0;

            snprintf( arg_device, sizeof(arg_device), DEV_PREFIX "%s", p );
            arg_device[sizeof(arg_device)-1] = 0;
            arg_overrides[1] = "-d";
            arg_overrides[2] = arg_device;
            done = 1;

        } while (0);

        if (done) {
            argv = arg_overrides;
            argc = 3;
            i    = 1;
            hasLibArgs = 1;
            rilLibPath = REFERENCE_RIL_PATH;

            LOGD("overriding with %s %s", arg_overrides[1], arg_overrides[2]);
        }
    }
OpenLib:
#endif
    switchUser();

    dlHandle = dlopen(rilLibPath, RTLD_NOW);

    if (dlHandle == NULL) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(-1);
    }

    RIL_startEventLoop();

    rilInit = (const RIL_RadioFunctions *(*)(const struct RIL_Env *, int, char **))dlsym(dlHandle, "RIL_Init");

    if (rilInit == NULL) {
        fprintf(stderr, "RIL_Init not defined or exported in %s\n", rilLibPath);
        exit(-1);
    }

    if (hasLibArgs) {
        rilArgv = argv + i - 1;
        argc = argc -i + 1;
    } else {
        static char * newArgv[MAX_LIB_ARGS];
        static char args[PROPERTY_VALUE_MAX];
        rilArgv = newArgv;
        property_get(LIB_ARGS_PROPERTY, args, "");
        argc = make_argv(args, rilArgv);
    }

    // Make sure there's a reasonable argv[0]
    rilArgv[0] = argv[0];

    funcs = rilInit(&s_rilEnv, argc, rilArgv);

    RIL_register(funcs);

done:

    while(1) {
        // sleep(UINT32_MAX) seems to return immediately on bionic
        sleep(0x00ffffff);
    }
}

