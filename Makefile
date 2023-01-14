export PATH:=3rdParty/linux_armhf_musl/bin:3rdParty/linux_x86_64_musl/bin:3rdParty/linux_i686_musl/bin:3rdParty/linux_mips_musl/bin:3rdParty/linux_mipsel_musl/bin:3rdParty/linux_ppc_musl/bin:$(PATH)
CPP=main.cpp View.cpp NormalPage.cpp PrivEscalate.cpp Zip.cpp
NAME=sfx
PARAMS=-static -std=c++17 -g0 -O3 -s -static -lboost_system -lboost_iostreams -lboost_program_options -lssl -lcrypto -lstdc++fs -lfltk -lfltk_images -lfltk -lpng -ljpeg -lfreetype -lminizip -lz -lcrypto -lssl -lpthread -latomic -lm
PARAMS_LINUX=$(PARAMS) -lXinerama -lXft  -lXcursor -lXrender -lXfixes -lXext -lX11 -lxcb -lXau -lXdmcp -lfontconfig -lfreetype -lpng -lz -lexpat -lrt -ldl
PARAMS_WINDOWS=$(PARAMS) -DWIN32 -D_WIN32 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -mconsole -lole32 -luuid -lcomctl32 -lwsock32 -lws2_32 -lksuser -lwinmm -lcrypt32 -lgdi32

all: linux_x86_64_musl
all_musl: linux_x86_64_musl linux_i686_musl linux_armhf_musl linux_mips_musl linux_mipsel_musl linux_ppc_musl
all_gnu: linux_x86_64_gnu linux_i686_gnu linux_armhf_gnu linux_mips_gnu linux_mipsel_gnu linux_ppc_gnu linux_s390x_gnu
all_windows: windows_32 windows_64

linux_x86_64_gnu:
	x86_64-linux-gnu-g++ -I. -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_x86_64_musl:
	x86_64-linux-musl-g++ -I. -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_i686_gnu:
	i686-linux-gnu-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_i686_musl:
	i686-linux-musl-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_armhf_gnu:
	arm-linux-gnueabihf-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_armhf_musl:
	arm-linux-musleabihf-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_mipsel_gnu:
	mipsel-linux-gnu-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_mipsel_musl:
	mipsel-linux-musl-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_mips_gnu:
	mips-linux-gnu-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_mips_musl:
	mips-linux-musl-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_ppc_musl:
	powerpc-linux-musl-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_ppc_gnu:
	powerpc-linux-gnu-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

linux_s390x_gnu:
	s390x-linux-gnu-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $CPP $(CPP) $(PARAMS_LINUX) -o $(NAME).$@

windows_32:
	i686-w64-mingw32-windres main.32.rc mainrc.32.o
	i686-w64-mingw32-g++ -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) mainrc.32.o $(PARAMS_WINDOWS) -o $(NAME).$@.exe

windows_64:
	x86_64-w64-mingw32-windres main.64.rc mainrc.64.o
	x86_64-w64-mingw32-g++ -I. -I3rdParty/$@/include -L3rdParty/$@/lib $(CPP) mainrc.64.o $(PARAMS_WINDOWS) -o $(NAME).$@.exe

clean:
	rm -rf mainrc.32.o mainrc.64.o $(NAME).*
