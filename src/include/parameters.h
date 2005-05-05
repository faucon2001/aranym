/*
 * parameters.h - parameter init/load/save code - header file
 *
 * Copyright (c) 2001-2005 ARAnyM developer team (see AUTHORS)
 *
 * Authors:
 *  MJ		Milan Jurik
 *  Joy		Petr Stehlik
 * 
 * This file is part of the ARAnyM project which builds a new and powerful
 * TOS/FreeMiNT compatible virtual machine running on almost any hardware.
 *
 * ARAnyM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ARAnyM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ARAnyM; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* MJ 2001 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "sysdeps.h"
#include "version.h"
#include "cfgopts.h"
#include <SDL_keyboard.h>

# include <cassert>
# include <cstdio>

#ifndef ARANYMHOME
# define ARANYMHOME		".aranym"
#endif
#define ARANYMCONFIG	"config"
#define ARANYMNVRAM		"nvram"
#define ARANYMKEYMAP	"keymap"
#ifndef DIRSEPARATOR
# define DIRSEPARATOR	"/"
#endif

#define BX_MAX_ATA_CHANNEL 1

enum geo_type {
	geoCylinders,
	geoHeads,
	geoSpt,
	geoByteswap
};

extern int get_geometry(char *, geo_type geo);

// External filesystem type
typedef struct {
	bool halfSensitive;
	char rootPath[512];
	char configPath[520];
} bx_aranymfs_options_t;

// Floppy device
typedef struct {
  char path[512];
} bx_floppy_options_t;

typedef enum {
      IDE_NONE, IDE_DISK, IDE_CDROM
} device_type_t;

// Generic ATA device
typedef struct {
  bool present;		// is converted to device_type_t below
  bool isCDROM;		// is converted to device_type_t below
  device_type_t type;
  char path[512];
  unsigned int cylinders;
  unsigned int heads;
  unsigned int spt;
  bool byteswap;
  bool readonly;
  bool status;		// inserted = true
  char model[41];
} bx_atadevice_options_t;

// SCSI device
typedef struct {
  bool present;
  bool byteswap;
  bool readonly;
  char path[512];
  char partID[3+1];	// 3 chars + EOS ( = '\0')
} bx_scsidevice_options_t;

// TOS options
typedef struct {
  bool redirect_CON;
  long cookie_mch;
} bx_tos_options_t;

// Video output options
typedef struct {
  bool fullscreen;		// boot in fullscreen
  int8 boot_color_depth;		// boot color depth
  uint8 refresh;
  int8 monitor;				// VGA or TV
} bx_video_options_t;

// Startup options
typedef struct {
  bool grabMouseAllowed;
  bool debugger;
} bx_startup_options_t;

// JIT compiler options
typedef struct {
  bool jit;
  bool jitfpu;
  bool tunealign;
  bool tunenop;
  uint32 jitcachesize;
  uint32 jitlazyflush;
  char jitblacklist[512];
} bx_jit_options_t;

// OpenGL options
typedef struct {
  bool enabled;
  uint32 width;
  uint32 height;
  uint32 bpp;
  bool filtered;
} bx_opengl_options_t;

// Ethernet options
typedef struct {
  char type[16];
  char tunnel[16];
  char ip_host[16];
  char ip_atari[16];
  char netmask[16];
} bx_ethernet_options_t;

// Lilo options
typedef struct {
	char kernel[512];	/* /path/to/vmlinux[.gz] */
	char args[512];		/* command line arguments for kernel */
	char ramdisk[512];	/* /path/to/ramdisk[.gz] */
} bx_lilo_options_t;

// Midi options
typedef struct {
	bool enabled;		/* MIDI output to file enabled ? */
	char output[512];	/* /path/to/output.txt */
} bx_midi_options_t;

// NfCdrom options
typedef struct {
	int32 physdevtohostdev;
} bx_nfcdrom_options_t;

// Misc CPU emulation options
#ifdef ENABLE_EPSLIMITER
typedef struct {
	bool	eps_enabled;	/* Exception per second limiter enabled ? */
	int32	eps_max;		/* Maximum tolerated eps before shutdown */
} bx_cpu_options_t;
#endif

// Autozoom options
typedef struct {
  bool enabled;		// Autozoom enabled
  bool integercoefs;	// Autozoom with integer coefficients
  bool fixedsize;	// Keep host screen size constant ?
  uint32 width;		// Wanted host screen size
  uint32 height;
} bx_autozoom_options_t;

// NfOSMesa options
typedef struct {
	uint32 channel_size;    /* If using libOSMesa[16|32], size of channel */
	char libgl[256];		/* Pathname to libGL */
	char libosmesa[256];	/* Pathname to libOSMesa */
} bx_nfosmesa_options_t;

// Parallel port options
typedef struct {
	char type[256];
	char file[256];
	char parport[256];
} bx_parallel_options_t;

// Keyboard and mouse
typedef struct {
  bool wheel_eiffel;		// eiffel compatible scancodes for mouse wheel
} bx_ikbd_options_t;

// Hotkeys
typedef struct {
	SDL_keysym	setup;
	SDL_keysym	quit;
	SDL_keysym	reboot;
	SDL_keysym	debug;
	SDL_keysym	ungrab;
	SDL_keysym	fullscreen;
	SDL_keysym	screenshot;
} bx_hotkeys_t;

#ifdef GDBSTUB
typedef struct {
  memptr text_base;
  memptr data_base;
  memptr bss_base;
} bx_gdbstub_t;
#endif

#define DISKS	8

// Options 
typedef struct {
  bx_floppy_options_t	floppy;
  bx_atadevice_options_t atadevice[BX_MAX_ATA_CHANNEL][2];
  bx_scsidevice_options_t	disks[DISKS];
  bx_aranymfs_options_t	aranymfs[ 'Z'-'A'+1 ];
  bx_video_options_t	video;
  bx_tos_options_t	tos;
  bx_startup_options_t	startup;
  bx_jit_options_t	jit;
  bx_opengl_options_t	opengl;
  bx_ethernet_options_t ethernet;
  bx_lilo_options_t		lilo;
  bx_midi_options_t		midi;
  bx_ikbd_options_t		ikbd;
  bx_nfcdrom_options_t	nfcdroms[ 'Z'-'A'+1 ];
#ifdef ENABLE_EPSLIMITER
  bx_cpu_options_t  cpu;
#endif
  bx_autozoom_options_t	autozoom;
  bx_nfosmesa_options_t	osmesa;
  bx_parallel_options_t parallel;
#ifdef GDBSTUB
  bx_gdbstub_t		gdbstub;
#endif
  char			tos_path[512];
  char			emutos_path[512];
  uint32		fastram;
  bool			autoMouseGrab;
  bx_hotkeys_t		hotkeys;
  bool			newHardDriveSupport;
} bx_options_t;

extern bx_options_t bx_options;


extern uint32 FastRAMSize;	// Size of Fast-RAM

extern char *program_name;
extern char rom_path[512];
extern char emutos_path[512];
extern bool startupGUI;
extern bool boot_emutos;
extern bool boot_lilo;

void usage(int);
extern bool check_cfg();
extern bool decode_switches(FILE *, int, char **);
extern char *getConfFilename(const char *file, char *buffer, unsigned int bufsize);
extern char *getDataFilename(const char *file, char *buffer, unsigned int bufsize);
char *addFilename(char *buffer, const char *file, unsigned int bufsize);

// following functions implemented in parameters_[unix|linux|cygwin].cpp
char *getHomeFolder(char *buffer, unsigned int bufsize);
char *getConfFolder(char *buffer, unsigned int bufsize);
char *getDataFolder(char *buffer, unsigned int bufsize);

extern const char *getConfigFile();
extern bool loadSettings(const char *);
extern bool saveSettings(const char *);

#endif

/*
vim:ts=4:sw=4:
*/
