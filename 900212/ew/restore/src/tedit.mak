# TASKING EDE (Windows based integrated Embedded Development Environment)
#
# This makefile has been generated by TASKING EDE
#
# Toolchain: TASKING 196/296 v5.0 r4
#

SHELLCMD=       break call cd chcp chdir cls copy ctty date del dir echo\
                erase exit for goto if md mkdir path pause prompt rd rem\
                ren rename rmdir set shift time type ver verify vol

BINDIR = d:\C196\bin
INCDIR = d:\C196\include
LIBDIR = d:\C196\lib

CSTART = cstart.obj
LIBRARIES= c96.lib 

PROJ    = tedit
PROJDIR = d:\sw_dev\visualstate\projectm16motor\ew\src
PRODDIR = d:\C196

all         : $(PROJ).abs 

devicesetup.obj: devicesetup.c vsmain.h simpleeventhandler.h eventhandler.h
	d:\C196\bin\c196.exe devicesetup.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

generic.obj : generic.c vsmain.h simpleeventhandler.h eventhandler.h usart2.h defs.h
	d:\C196\bin\c196.exe generic.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

init.obj    : init.c vsmain.h simpleeventhandler.h eventhandler.h
	d:\C196\bin\c196.exe init.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

simpleeventhandler.obj: simpleeventhandler.c simpleeventhandler.h eventhandler.h
	d:\C196\bin\c196.exe simpleeventhandler.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

timers.obj  : timers.c vsmain.h simpleeventhandler.h eventhandler.h usart2.h defs.h
	d:\C196\bin\c196.exe timers.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

usart2.obj  : usart2.c usart2.h simpleeventhandler.h eventhandler.h vsmain.h defs.h
	d:\C196\bin\c196.exe usart2.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

userloop.obj: userloop.c vsmain.h simpleeventhandler.h eventhandler.h usart2.h defs.h globals.h
	d:\C196\bin\c196.exe userloop.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

vsactiontooutput.obj: vsactiontooutput.c usart2.h vsmain.h simpleeventhandler.h eventhandler.h

vsactiontooutput.obj: defs.h
	d:\C196\bin\c196.exe vsactiontooutput.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

vsmain.obj  : vsmain.c vsmain.h simpleeventhandler.h eventhandler.h
	d:\C196\bin\c196.exe vsmain.c -f <<EOF 
  ${separate " &\n"  ms co pl(60) pw(120) dn(1) ot(1) sp(0) db si(d:\C196\include) omf(2) md(sa) nc nk nd}
  EOF

$(PROJ).abs : devicesetup.obj generic.obj init.obj simpleeventhandler.obj timers.obj usart2.obj userloop.obj vsactiontooutput.obj vsmain.obj
	d:\C196\bin\rl196.exe -f <<EOF 
  ${separate ", &\n" $(CSTART) $(match .obj $!) $(match .lib $!) $(LIBRARIES)}
  TO $@ &
  ${separate " &\n"  li(pl,sb,ln,sm) pw(120) sfr sl(d:\C196\lib) omf(2) md(sa) nc nk nd bu ss(+6) }
  EOF

# Copyright (c) 1998 TASKING, Inc.
# Serial:	602207-Apt
# EDE Version:	2.0 r7
