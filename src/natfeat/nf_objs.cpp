/*
	Native features

	ARAnyM (C) 2005 Patrice Mandin

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*--- Includes ---*/

#include "nf_objs.h"
#include "nf_basicset.h"

#include "xhdi.h"
#include "audio.h"
#include "hostfs.h"
#include "ethernet.h"
#include "debugprintf.h"
#ifdef NFVDI_SUPPORT
# include "nfvdi.h"
# include "nfvdi_soft.h"
#else
# include "fvdidrv.h"
#endif
#ifdef NFCDROM_SUPPORT
# include "nfcdrom.h"
# ifdef NFCDROM_LINUX_SUPPORT
#  include "nfcdrom_linux.h"
# endif
# ifdef NFCDROM_SDL_SUPPORT
#  include "nfcdrom_sdl.h"
# endif
#endif
#ifdef NFPCI_SUPPORT
# include "nfpci.h"
# ifdef NFPCI_LINUX_SUPPORT
#  include "nfpci_linux.h"
# endif
#endif
#ifdef NFOSMESA_SUPPORT
# include "nfosmesa.h"
#endif
#ifdef NFJPEG_SUPPORT
# include "nfjpeg.h"
#endif
/* add your NatFeat class definition here */

/*--- Defines ---*/

#define MAX_NATFEATS 32

/*--- Variables ---*/

NF_Base *nf_objects[MAX_NATFEATS];	/* The natfeats we can use */
unsigned int nf_objs_cnt;			/* Number of natfeats we can use */

/*--- Functions prototypes ---*/

static void NFAdd(NF_Base *new_nf);

/*--- Functions ---*/

void NFCreate(void)
{
	nf_objs_cnt=0;
	memset(nf_objects, 0, sizeof(nf_objects));

	/* NF basic set */
	NFAdd(new NF_Name);
	NFAdd(new NF_Version);
	NFAdd(new NF_Shutdown);
	NFAdd(new NF_StdErr);

	/* additional NF */
	NFAdd(new DebugPrintf);
	NFAdd(new XHDIDriver);
	NFAdd(new AUDIODriver);

#ifdef NFVDI_SUPPORT
	NFAdd(new SoftVdiDriver);
#else
	NFAdd(new FVDIDriver);
#endif

#ifdef HOSTFS_SUPPORT
	NFAdd(new HostFs);
#endif

#ifdef ETHERNET_SUPPORT
	NFAdd(new ETHERNETDriver);
#endif

#ifdef NFCDROM_SUPPORT
# ifdef NFCDROM_LINUX_SUPPORT
	NFAdd(new CdromDriverLinux);
# endif
# ifdef NFCDROM_SDL_SUPPORT
	NFAdd(new CdromDriverSdl);
# endif
#endif

#ifdef NFPCI_SUPPORT
# ifdef NFPCI_LINUX_SUPPORT
	NFAdd(new PciDriverLinux);
# endif
#endif

#ifdef NFOSMESA_SUPPORT
	NFAdd(new OSMesaDriver);
#endif

#ifdef NFJPEG_SUPPORT
	NFAdd(new JpegDriver);
#endif

	/* add your NatFeat object declaration here */
}

static void NFAdd(NF_Base *new_nf)
{
	/* Add a natfeat to our array */
	if (nf_objs_cnt == MAX_NATFEATS) {
		fprintf(stderr, "No more available slots to add a Natfeat\n");
	}

	nf_objects[nf_objs_cnt++] = new_nf;
}

void NFDestroy(void)
{
	for(unsigned int i=0; i<nf_objs_cnt; i++) {
		if (nf_objects[i]) {
			delete nf_objects[i];
		}
	}
}

void NFReset(void)
{
	for(unsigned int i=0; i<nf_objs_cnt; i++) {
		if (nf_objects[i]) {
			nf_objects[i]->reset();
		}
	}
}
