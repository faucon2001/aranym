/*
 * $Id$
 *
 * The ARAnyM BetaDOS driver.
 *
 * 2001/2002 STan
 *
 * Based on:
 * @(#)cookiefs/dosmain.c
 *
 * Copyright (c) Julian F. Reschke, 28. November 1995
 * All rights reserved.
 *
 **/

#include <mintbind.h>
#include <mint/basepage.h>
#include <stdlib.h>
#include <string.h>

#include "mint/mint.h"
#include "mint/filedesc.h"

#include "../hostfs_xfs.h"
#include "../hostfs_dev.h"

#include "mintproc.h"
#include "mintfake.h"


#define DEVNAME "ARAnyM Host Filesystem"
#define VERSION "0.60"

char DriverName[] = DEVNAME" "VERSION;
long ldp;


void _cdecl ShowBanner( void );
void* _cdecl InitDevice( long bosDevID, long dosDevID );
long set_cookie (ulong tag, ulong val);
ulong get_cookie (ulong tag);

/* Diverse Utility-Funktionen */

static int Bconws( char *str )
{
    int cnt = 0;

    while (*str) {
        cnt++;
        if (*str == '\n') {
            Bconout (2, '\r');
            cnt++;
        }
        Bconout (2, *str++);
    }

    return cnt;
}

void _cdecl ShowBanner( void )
{
    Bconws (
            "\n\033p "DEVNAME" "VERSION" \033q "
            "\nCopyright (c) ARAnyM Development Team, "__DATE__"\n"
            );
}

struct cookie
{
	long tag;
	long value;
};

ulong get_cookie (ulong tag)
{
	struct cookie *cookie = *(struct cookie **)0x5a0;
	if (!cookie) return 0;

	while (cookie->tag) {
		if (cookie->tag == tag) return cookie->value;
		cookie++;
	}

	return 0;
}

long set_cookie (ulong tag, ulong val)
{
	struct cookie *cookie = *(struct cookie **)0x5a0;
	if (!cookie) return 0;

	while (cookie->tag) cookie++;
	cookie->tag = tag;
	cookie->value = val;
	return 1;
}


/* ../hostfs_xfs and ../hostfs_dev internals used */
extern long     _cdecl aranym_fs_native_init(int fs_devnum, char *mountpoint, char *hostroot, int halfsensitive,
											 void *fs, void *fs_dev);
extern void* ara_fs_root;
extern FILESYS aranym_fs;
extern DEVDRV  aranym_fs_devdrv;


void* _cdecl InitDevice( long bosDevID, long dosDevID )
{
	char mountPoint[] = "A:";
	mountPoint[0] += (dosDevID = (dosDevID&0x1f)); // mask out bad values of the dosDevID

	/*
	 * Hack to get the drive table the same for all hostfs.dos
	 * instances loaded by BetaDOS into memory.
         *
	 * Note: This is definitely not MP friendly, but FreeMiNT
	 *       doesn't support B(M)etaDOS anyway, so: Do not do
	 *       this when using 'MiNT' or 'MagX' it crashes then.
	 */	
	if (!get_cookie(0x4D694E54L /*'MiNT'*/) &&
	    !get_cookie(0x4D616758L /*'MagX'*/))
	{
 		/* BetaDOS Host Filesystem cookie */
		ulong p = get_cookie(0x42446866L /*'BDfh'*/);
		if ( p ) curproc = (void*)p;
		else set_cookie(0x42446866L /*'BDfh'*/, (ulong)curproc);
	}

	/*
	 * We _must_ use the bosDevID to define the drive letter here
	 * because MetaDOS (in contrary to BetaDOS) does not provide
	 * the dosDevID
	 */
	DEBUG(("InitDevice: %s [dosDev=%ld, bosDev=%ld] addr: %lx", mountPoint, dosDevID, bosDevID, &ara_fs_root ));

	aranym_fs_init( NULL );

	/* map the BetaDOS drive to some bosDrive | 0x6000 so that the mapping would
	   not colide with the MiNT one */
	aranym_fs_native_init( dosDevID | 0x6000, mountPoint, "/tmp", 1,
						   &aranym_fs, &aranym_fs_devdrv );

	aranym_fs.root( dosDevID | 0x6000, &curproc->p_cwd->root[dosDevID] );

#ifdef DEBUG_INFO
	{
		fcookie *relto = &curproc->p_cwd->root[dosDevID];
		DEBUG (("InitDevice: root (%08lx, %08lx, %04x)", relto->fs, relto->index, relto->dev));
	}
#endif

	return &ldp;
}



/**
 * $Log$
 * Revision 1.9  2005/09/26 22:18:05  standa
 * Build warnings removal.
 *
 * Revision 1.8  2004/05/07 11:31:07  standa
 * The BDhf cookie is not used in MagiC or MiNT because it was causing
 * ARAnyM to crash.
 *
 * Revision 1.7  2004/04/26 07:14:04  standa
 * Adjusted to the recent FreeMiNT CVS state to compile and also made
 * BetaDOS only. No more MetaDOS compatibility attempts.
 *
 * Dfree() fix - for Calamus to be able to save its documents.
 *
 * Some minor bugfix backports from the current FreeMiNTs CVS version.
 *
 * The mountpoint entries are now shared among several hostfs.dos instances
 * using a 'BDhf' cookie entry (atari/hostfs/metados/main.c).
 *
 * Revision 1.6  2003/03/24 08:58:53  joy
 * aranymfs.xfs renamed to hostfs.xfs
 *
 * Revision 1.5  2003/03/20 21:27:22  standa
 * The .xfs mapping to the U:\G mountpouints (single letter) implemented.
 *
 * Revision 1.4  2003/03/05 09:30:45  standa
 * mountPath declaration fixed.
 *
 * Revision 1.3  2003/03/03 20:39:44  standa
 * Parameter passing fixed.
 *
 * Revision 1.2  2002/12/11 08:05:54  standa
 * The /tmp/calam host fs mount point changed to /tmp one.
 *
 * Revision 1.1  2002/12/10 20:47:21  standa
 * The HostFS (the host OS filesystem access via NatFeats) implementation.
 *
 *
 **/
