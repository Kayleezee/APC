##################################################################
#
## FILENAME    - Makefile
#
## DESCRIPTION - Advanced Parallel Computing, SS15 HD, Sheet 8
#
## AUTHOR      - Naumann, Klein
#
## LAST CHANGE - 2015-06-13
#
##################################################################

CC=gcc
CFLAGS=-pthread

redblacktree:
	$(CC) $(CFLAGS) src/rbtreemain.c src/rbtree.c -o redblacktree

.PHONY: clean
clean :
	rm -f redblacktree
