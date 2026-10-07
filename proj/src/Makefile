# name of the program (Minix service)
PROG=proj 

# Add search paths for source files
.PATH: ${.CURDIR}/labDrivers/keyboard
.PATH: ${.CURDIR}/labDrivers/mouse
.PATH: ${.CURDIR}/labDrivers/timer
.PATH: ${.CURDIR}/labDrivers/video
.PATH: ${.CURDIR}/utils
.PATH: ${.CURDIR}/gameLogic
.PATH: ${.CURDIR}/projDrivers/keyboard
.PATH: ${.CURDIR}/projDrivers/mouse
.PATH: ${.CURDIR}/projDrivers/timer
.PATH: ${.CURDIR}/states
.PATH: ${.CURDIR}/uiElements
.PATH: ${.CURDIR}/sprites

# source code files to be compiled
# IMPORTANTE, SEMPRE Q SE ADICIONAR UM NOVO FICHEIRO TEMOS Q INCLUIR AQUI
SRCS = proj.c keyboard.c kbc.c mouse.c timer.c video_gr.c game.c cursor.c utils.c settings.c mole.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf -lxpm

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>