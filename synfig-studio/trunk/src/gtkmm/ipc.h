/* === S I N F G =========================================================== */
/*!	\file ipc.h
**	\brief Template Header
**
**	$Id: ipc.h,v 1.2 2005/01/12 00:31:11 darco Exp $
**
**	\legal
**	Copyright (c) 2002 Robert B. Quattlebaum Jr.
**
**	This software and associated documentation
**	are CONFIDENTIAL and PROPRIETARY property of
**	the above-mentioned copyright holder.
**
**	You may not copy, print, publish, or in any
**	other way distribute this software without
**	a prior written agreement with
**	the copyright holder.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SINFG_IPC_H
#define __SINFG_IPC_H

/* === H E A D E R S ======================================================= */

#include <sinfg/smartfile.h>
#include <glibmm/main.h>
#include <sinfg/string.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace studio {

class IPC
{
private:

	int fd;
	sinfg::SmartFILE file;

	bool fifo_activity(Glib::IOCondition cond);

public:
	IPC();
	~IPC();

	static sinfg::String fifo_path();
	static sinfg::SmartFILE make_connection();

	static bool process_command(const sinfg::String& cmd);
}; // END of class IPC
	
}; // END of namespace studio

/* === E N D =============================================================== */

#endif
