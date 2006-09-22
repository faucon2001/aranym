/*
	Atari MIDI emulation, output to file

	ARAnyM (C) 2005-2006 Patrice Mandin

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysdeps.h"
#include "hardware.h"
#include "cpu_emulation.h"
#include "memory.h"
#include "midi.h"
#include "midi_file.h"
#include "parameters.h"

#define DEBUG 0
#include "debug.h"

MidiFile::MidiFile(memptr addr, uint32 size) : MIDI(addr, size)
{
	D(bug("midi_file: interface created at 0x%06x", getHWoffset()));

	output_handle = fopen(bx_options.midi.output,"w");
}

MidiFile::~MidiFile(void)
{
	D(bug("midi_file: interface destroyed at 0x%06x", getHWoffset()));

	if (output_handle) {
		fclose(output_handle);
	}
}

void MidiFile::WriteData(uae_u8 value)
{
	D(bug("midi_file: WriteData(0x%02x)",value));

	if (output_handle) {
		fprintf(output_handle, "%c", value);
	}
}