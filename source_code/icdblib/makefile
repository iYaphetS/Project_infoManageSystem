
PRJHOME:=.

CC = gcc

INCDIR:=$(PRJHOME)/inc $(ORACLE_HOME)/rdbms/public $(ORACLE_HOME)/rdbms/demo 
SRCDIR:=$(PRJHOME)/src
BINDIR:=$(PRJHOME)/bin
OLIBDIR:=$(PRJHOME)/lib
TESTDIR:=$(PRJHOME)/test
ILIBDIR:=$(HOME)/lib


LIBS:=icdbapi pthread
#LIBS:=DAEsqlDB pthread

VISIBLEFLAG= -fvisibility=hidden

#SHAREDFLAG= -shared -fpic -Wl,-Bsymbolic
SHAREDFLAG= -shared -fpic

ORALIBFLAG= -L$ORACLE_HOME/lib -lclntsh -lpthread

INCFLAG:= $(addprefix -I,$(INCDIR))
LIBFLAG:= $(addprefix -L,$(ILIBDIR)) $(addprefix -l,$(LIBS))
CFLAG := -g -Wall -DHAVE_GCCVISIBILITYPATCH $(VISIBLEFLAG) 

EXEC1 = $(OLIBDIR)/libicdbapi.so
EXEC2 = $(BINDIR)/testicdbapi

OBJS1 = $(SRCDIR)/icdbsqlcheck.o $(SRCDIR)/icdblog.o $(SRCDIR)/icdbapi.o
OBJS2 = $(TESTDIR)/testicdbapi.o 

all: $(EXEC1) $(EXEC2)

include $(ORACLE_HOME)/precomp/lib/env_precomp.mk

CC = gcc

$(SRCDIR)/icdbapi.o: $(SRCDIR)/icdbapi.pc
	@echo 'icdbapi.pc start....111111111111111111-----------------------------------------------'
	proc iname=./src/icdbapi.pc oname=./src/icdbapi.c THREADS=YES include='(/home/oracle_11/app/oracle/product/11.2.0/db_1/rdbms/public,$(PRJHOME)/inc,$(ORACLE_HOME)/rdbms/demo)'
	gcc -c -shared -fPIC -o $(SRCDIR)/icdbapi.o -g -Wall -DHAVE_GCCVISIBILITYPATCH -fvisibility=hidden -I${ORACLE_HOME}/rdbms/public  -I${ORACLE_HOME}/precomp/public -I./inc -L${ORACLE_HOME}/lib $(SRCDIR)/icdbapi.c 
	@echo 'icdbapi.pc end....222222222222-----------------------------------------------'
#	@-rm -f tp*	

$(EXEC1): $(OBJS1)
	gcc  -fPIC -shared -o $@ $^  -DHAVE_GCCVISIBILITYPATCH
	@cp $@ $(ILIBDIR)
	
$(EXEC2): $(OBJS2)
	$(CC) -o $@ $^ -g -Wall $(INCFLAG) $(LIBFLAG)  -lclntsh
#	@cp $@ $(HOME)/bin 


.c.o:
	$(CC) -c -o $@ $(CFLAG) $(INCFLAG) $< -shared -fPIC -DHAVE_GCCVISIBILITYPATCH  
		
.PHONY: clean rebuild

clean:
	@-rm -f $(SRCDIR)/*.o
	@-rm -f $(TESTDIR)/*.o
	@-rm -f $(SRCDIR)/icdbapi.c
#	@-rm -f $(EXEC1)
	@-rm -f $(EXEC2)
#	@-rm -f $(ILIBDIR)/$(EXEC1)
	@-rm -f $(HOME)/bin/$(EXEC2)
	@-rm -f tp*	
	
rebuild: clean all
