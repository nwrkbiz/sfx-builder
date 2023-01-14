#include <PrivEscalate.h>

#include <string>

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <securitybaseapi.h>
#else
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/Xfuncs.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <X11/Xauth.h>
#include <stdlib.h>
#endif // _WIN32

bool isElevated() {
#ifdef _WIN32
	BOOL fIsElevated = FALSE;
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		goto Cleanup;
	}


	if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {	
		goto Cleanup;
	}

	fIsElevated = elevation.TokenIsElevated;

Cleanup:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	return fIsElevated;
#else
    return geteuid() == 0;
#endif // _WIN32
}

void elevatePrivilege(int argc, char **argv) {
    std::string params = "";
    for(size_t i = 1; i < argc; i++) {
        params += " " + std::string(argv[i]);
    }
#ifdef _WIN32
        ShellExecute(NULL,
                    "runas",
                    argv[0],
                    params.c_str(),
                    NULL,
                    SW_SHOWNORMAL);
#else
        fl_message_title("root needed!");
        const char* pwd = fl_password("Enter sudo password:", "");
        if(pwd) {
            Display *dpy;
            if ((dpy = XOpenDisplay(NULL)) != NULL) {
                XServerInterpretedAddress siaddr;
                XHostAddress ha;
                siaddr.type = (char*)"localuser";
                siaddr.typelength = 9;
                siaddr.value = (char*)"root";
                siaddr.valuelength = 4;
                ha.family = FamilyServerInterpreted;
                ha.address = (char *) &siaddr;
                bool rc = XAddHost(dpy, &ha);
                XCloseDisplay(dpy);
                if(rc){
                    std::string cmd = "echo '" + std::string(pwd) + "' | sudo -S " + argv[0] + " " + params + " &";
                    system(cmd.c_str());
                }
            }
        }
#endif
}
