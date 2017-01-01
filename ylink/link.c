// YLINK - the Ypsilon Executable Linker
// Copyright 2016 Zane Wagner. All rights reserved.
#include "stdio.h"
#include "notice.h"
#include "link.h"

#define LINEMAX  127
#define LINESIZE 128
#define ERROR 0xFFFF

char *line;

main(int argc, int *argv) {
    uint fd;
    fputs(VERSION, stderr);
    fputs(CRIGHT1, stderr);
    allocatevars();
    if ((fd = ask(argc, argv)) == ERROR) {
        abort(1);
    }
    readFile(fd);
    cleanup(fd);
}

allocatevars() {
    line = allocvar(LINESIZE, 1);
}

allocvar(int nItems, int itemSize) {
    int result;
    result = calloc(nItems, itemSize);
    if (result == 0) {
        printf("Could not allocate mem: %u x %u\n", nItems, itemSize);
        abort(1);
    }
}

ask(int argc, int *argv) {
    int input;
    if (argc != 2) {
        printf("Requires exactly one argument, received %u.\n", argc);
        return ERROR;
    }
    getarg(1, line, LINESIZE, argc, argv);
    if(!(input = fopen(line, "r"))) {
        erroutf("Could not open file: ", line);
        return ERROR;
    }
	return input;
}

cleanup(uint fd) {
    if (fd != 0) {
        fclose(fd);
    }
    return;
}

// === Reading OBJ Format =====================================================

readFile(uint fd) {
    uint outfd;
    byte recType;
    uint length;
    outfd = fopen("link.txt", "w");
    while (1) {
        recType = read_u8(fd);
        if (feof(fd)) {
            fputs("eof", stderr);
            break;
        }
        if (ferror(fd)) {
            fputs("ferr", stderr);
            break;
        }
        length = read_u16(fd);
        do_record(recType, length, fd);
    }
    writeLibData(outfd);
    fclose(outfd);
    return;
}

prnhexint(uint value) {
    prnhexch(value >> 8);
    prnhexch(value & 0x00ff);
    return;
}

prnhexch(byte value) {
    byte ch0;
    ch0 = (value & 0xf0) >> 4;
    if (ch0 < 10) {
        ch0 += 48;
    }
    else {
        ch0 += 55;
    }
    fputc(ch0, stdout);
    ch0 = (value & 0x0f);
    if (ch0 < 10) {
        ch0 += 48;
    }
    else {
        ch0 += 55;
    }
    fputc(ch0, stdout);
    return;
}

// === Binary Reading Routines ================================================
read_u8(uint fd) {
    byte ch;
    ch = _read(fd);
    return ch;
}

read_u16(uint fd) {
    uint i;
    i = (_read(fd) & 0x00ff);
    i += (_read(fd) & 0x00ff) << 8;
    return i;
}

// read string that is prefixed by length.
read_strpre(char* str, uint fd) {
    byte length, retval;
    char* next;
    next = str;
    retval = length = read_u8(fd);
    while (length > 0) {
        *next++ = read_u8(fd);
        length--;
    }
    *next++ = NULL;
    return retval + 1;
}

offsetfd(uint fd, uint base[], uint offset[]) {
    bseek(fd, base, 0);
    bseek(fd, offset, 1);
}

// === Error Routines =========================================================

errout(char *line) {
    fputs("*** Error: ", stderr); 
    lineout(line, stderr);
}

erroutf(char *format, char *arg) {
    fputs("*** Error: ", stderr); 
    fprintf(stderr, format, arg);
    lineout(line, stderr);
}

lineout(char *line, uint fd) {
    fputs(line, fd);
    fputc(NEWLINE, fd);
}