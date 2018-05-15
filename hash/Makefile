PROGRAMS = test_hash

#CROSS = mipsel-linux-
CROSS = 
CC = $(CROSS)gcc
CXX = $(CROSS)g++
STRIP = $(CROSS)strip
LINKER = $(CROSS)g++
AR = $(CROSS)ar

INCLUDE =

SRCDIRS = .

SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCS_CPP = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

OBJS = $(SRCS:.c=.o)
OBJS_CPP =  $(SRCS_CPP:.cpp=.o)

CFLAGS := -O2 -Wundef -Wall -Wchar-subscripts -Wsign-compare -Wuninitialized -O -Wno-missing-braces -Werror -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

CXXFLAGS := -O2 -Wundef -Wall -Wchar-subscripts -Wsign-compare -Wuninitialized -O -Wno-missing-braces -Werror -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

LDFLAGS := -lpthread

all: $(PROGRAMS)
#	cp $(PROGRAMS) $(INSTALL_DIR)

$(PROGRAMS): $(OBJS) $(OBJS_CPP)
	$(LINKER) -o $@ $^ $(LDFLAGS)
#	$(STRIP) $(PROGRAMS)
%.o: %.c 
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c  $< -o $@

clean: 
	rm -f $(OBJS) $(OBJS_CPP) $(PROGRAMS)
