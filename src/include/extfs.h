/*
 * extfs.h - HostFS routines
 *
 * Copyright (c) 2001-2003 STanda of ARAnyM development team (see AUTHORS)
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
 * along with Atari800; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _EXTFS_H
#define _EXTFS_H

#include "sysdeps.h"
#include "cpu_emulation.h"

// FIXME: it is stupid to place these here,
//        but I didn't want to touch many files in CVS ;(
//	      Feel free to move it anywhere
#ifndef MIN
#define MIN(_a,_b) ((_a)<(_b)?(_a):(_b))
#endif
#ifndef MAX
#define MAX(_a,_b) ((_a)>(_b)?(_a):(_b))
#endif


#define MAXPATHNAMELEN 2048

#ifdef EXTFS_SUPPORT
class ExtFs {
  private:

	struct LogicalDev {
		int16	dummy;
	};			// Dummy structure... I don't know the meaning in COOK_FS

	struct XfsFsFile {
		XfsFsFile *parent;
		uint32	  refCount;
		uint32	  childCount;
		char	  *name;
	};

	struct XfsCookie {
		uint32    xfs;
		uint16    dev;
		uint16    aux;
		XfsFsFile *index;
	};

	struct ExtDta {			// used by Fsetdta, Fgetdta
		DIR	*ds_dirh;		// opendir resulting handle to perform dirscan
		uint16	ds_attrib;	// search attribs wanted
		uint8	ds_index;	// index in the fs_pathName array (seems like a hack, but I don't know better)
		int8	ds_name[14];

		// And now GEMDOS specified fields
		uint8	d_attrib;
		uint16	d_time;
		uint16	d_date;
		uint32	d_length;
		int8	d_fname[14];
	};				// See myDTA in Julian's COOK_FS

	struct ExtFile {
		XfsCookie fc;
		int16	  index;
		int16	  flags;
		int16	  links;
		int32	  hostfd;
		int32	  offset;
		int32	  devinfo;
		uint32	  next;
	} ;				// See MYFILE in Julian's COOK_FS

	struct XfsDir {
		XfsCookie fc;			// cookie for this directory
		uint16    index;		// index of the current entry
		uint16    flags;		// flags (e. g. tos or not)
		DIR       *dir;			// used DIR
		int16	  pathIndex;	// index of the pathName in the internal pool FIXME?
		XfsDir	  *next;		// linked together so we can close them to process term
	};

	typedef struct XfsDir ExtDir;


	struct ExtDrive	{
		bool halfSensitive;
		char *rootPath;
		char *currPath;		// Only Dsetpath uses this in the .DOS driver -> can be removed
	};

	ExtDrive drives[ 'Z'-'A'+1 ];

	bool isPathValid(const char *fileName);
	void xfs_debugCookie( XfsCookie *fc );


  public:
	ExtFs();
	~ExtFs();

	/**
	 * Installs the drive.
	 **/
	void init();
	void install( const char driveSign, const char* rootPath, bool halfSensitive );

	uint32 getDrvBits();

	/**
	 * MetaDos DOS driver dispatch functions.
	 **/
	void dispatch( uint32 fncode, M68kRegisters *r );

	void fetchDTA( ExtDta *dta, memptr dtap );
	void flushDTA( ExtDta *dta, memptr dtap );
	void fetchFILE( ExtFile *extFile, memptr filep );
	void flushFILE( ExtFile *extFile, memptr filep );
	void fetchEDIR( ExtDir *extDir, memptr dirp );
	void flushEDIR( ExtDir *extDir, memptr dirp );

	/**
	 * Some crossplatform mem & str functions to use in GEMDOS replacement.
	 **/
	void a2fmemcpy( char *dest, memptr source, size_t count );
	void a2fstrcpy( char *dest, memptr source );
	void f2amemcpy( memptr dest, char *source, size_t count );
	void f2astrcpy( memptr dest, char *source );

	/**
	 * Unix to ARAnyM structure conversion routines.
	 **/
	uint32 unix2toserrno( int unixerrno,int defaulttoserrno );
	uint16 statmode2xattrmode( mode_t m );
	uint16 statmode2attr( mode_t m );
	uint16 time2dos( time_t t );
	uint16 date2dos( time_t t );
	void   datetime2tm( uint32 dtm, struct tm* ttm );
	int    st2flags( uint16 flags );
	int16  flags2st( int flags );

	/**
	 * Path conversions.
	 *
	 * Note: This is the most sophisticated thing in this object.
	 **/
	ExtDrive* getDrive( const char* pathName );
	void transformFileName( char* dest, const char* source );
	bool getHostFileName( char* result, ExtDrive* drv, char* pathName, const char* name );
	void convertPathA2F( char* fpathName, char* pathName, char* basePath = NULL );

	int16  getFreeDirIndex( char **pathNames );
	void   freeDirIndex( uint8 index, char **pathNames );
	bool   filterFiles( ExtDta *dta, char *fpathName, char *mask, struct dirent *dirEntry );
	int32  findFirst( ExtDta *dta, char *fpathName );

	// GEMDOS functions
	int32 Dfree_(char *fpathName, memptr diskinfop );
	int32 DfreeExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					 memptr diskinfop, int16 drive );
	int32 DcreateExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					   const char *pn);
	int32 DdeleteExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					   const char *pn);
	int32 DsetpathExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
						const char *pn);
	int32 FcreateExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					   const char *pn, int16 attr);
	int32 FopenExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					 const char *pn, int16 mode);
	int32 Fopen_( const char* fpathName, int flags, int mode, ExtFile *fp );
	int32 FcloseExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					  int16 handle);
	int32 FreadExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					 int16 handle, uint32 count, memptr buffer);
	int32 FwriteExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					  int16 handle, uint32 count, memptr buffer);
	int32 FdeleteExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					   const char *pn);
	int32 FseekExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					 int32 offset, int16 handle, int16 seekmode);

	int32 FattribExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					   const char* pn, int16 wflag, int16 attr);

	int32 FsfirstExtFs( LogicalDev *ldp, char *pathname, ExtDta *dta,
						const char *pn, int16 attribs );
	int32 FsnextExtFs ( LogicalDev *ldp, char *pathName, ExtDta *dta );
	int32 FrenameExtFs(LogicalDev *ldp, char *pathName, ExtFile *fp,
					   int16 reserved, char *oldpath, char *newPathName);
	int32 Fdatime_( char *fpathName, ExtFile *fp, uint32 datetimep, int16 wflag);
	int32 FdatimeExtFs( LogicalDev *ldp, char *pathName, ExtFile *fp,
						uint32 datetimep, int16 handle, int16 wflag);
	int32 FcntlExtFs( LogicalDev *ldp, char *pathName, ExtFile *fp,
					  int16 handle, memptr arg, int16 cmd );
	int32 DpathconfExtFs( LogicalDev *ldp, char *pathName, ExtFile *fp,
						  const char* pn, int16 which );
	int32 Dpathconf_( char *fpathName, int16 which, ExtDrive *drv );

	int32 DopendirExtFs( LogicalDev *ldp, char *pathName, ExtDir *dirh, const char* pn, int16 flag);
	int32 Dopendir_( char *fpathName, ExtDir *dirh, int16 flag);
	int32 DclosedirExtFs( ExtDir *dirh );

	int32 DreaddirExtFs( LogicalDev *ldp, char *pathName, ExtDir *dirh,
						 int16 len, int32 dirhandle, memptr buff );

	int32 DxreaddirExtFs( LogicalDev *ldp, char *pathName, ExtDir *dirh,
						  int16 len, int32 dirhandle, memptr buff, memptr xattrp, memptr xretp );
	int32 Dxreaddir_( char *fpathName, ExtDir *dirh, int16 len, memptr buff, memptr xattrp, memptr xretp );
	int32 DrewinddirExtFs( ExtDir *dirh );

	int32 FxattrExtFs( LogicalDev *ldp, char *pathName, ExtFile *fp,
					   int16 flag, const char* pn, memptr xattrp );
	int32 Fxattr_( LogicalDev *ldp, char *fpathName, int16 flag, memptr xattrp );   // Taking host pathName instead of Atari one.

	// these variables are another plain hack.
	// They should be in each mounted instance just like basePath and caseSensitive flag (which are not either now)
	uint32 mint_fs_drv;    /* FILESYS */
	uint32 mint_fs_devdrv; /* DEVDRV  */
	uint16 mint_fs_devnum; /* device number */

	void dispatchXFS( uint32 fncode, M68kRegisters *r );

	void fetchXFSC( XfsCookie *fc, memptr filep );
	void flushXFSC( XfsCookie *fc, memptr filep );
	void fetchXFSF( ExtFile *extFile, memptr filep );
	void flushXFSF( ExtFile *extFile, memptr filep );
	void fetchXFSD( XfsDir *dirh, memptr dirp );
	void flushXFSD( XfsDir *dirh, memptr dirp );

	char *cookie2Pathname( XfsFsFile *fs, const char *name, char *buf );
	void xfs_freefs( XfsFsFile *fs );

	int32 xfs_root( uint16 dev, XfsCookie *fc );
	int32 xfs_dupcookie( XfsCookie *newCook, XfsCookie *oldCook );
	int32 xfs_release( XfsCookie *fc );
	int32 xfs_getxattr( XfsCookie *fc, memptr xattrp );
	int32 xfs_getdev( XfsCookie *fc, memptr devspecial );
	int32 xfs_lookup( XfsCookie *dir, memptr name, XfsCookie *fc );
	int32 xfs_getname( XfsCookie *relto, XfsCookie *dir, memptr pathName, int16 size );
	int32 xfs_creat( XfsCookie *dir, memptr name, uint16 mode, int16 flags, XfsCookie *fc );
	int32 xfs_rename( XfsCookie *olddir, memptr oldname, XfsCookie *newdir, memptr newname );
	int32 xfs_remove( XfsCookie *dir, memptr name );
	int32 xfs_pathconf( XfsCookie *fc, int16 which );
	int32 xfs_opendir( XfsDir *dirh, uint16 flags );
	int32 xfs_readdir( XfsDir *dirh, memptr buff, int16 len, XfsCookie *fc );
	int32 xfs_mkdir( XfsCookie *dir, memptr name, uint16 mode );
	int32 xfs_rmdir( XfsCookie *dir, memptr name );
	int32 xfs_readlink( XfsCookie *dir, memptr buf, int16 len );
	int32 xfs_dfree( XfsCookie *dir, memptr buf );

	int32 xfs_dev_open(ExtFile *fp);
	int32 xfs_dev_datime( ExtFile *fp, memptr datetimep, int16 wflag);

};

#endif /* EXTFS_SUPPORT */

#endif /* _EXTFS_H */
