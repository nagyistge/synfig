/* === S Y N F I G ========================================================= */
/*!	\file synfig/rendering/module/software/optimizer/optimizercolorcorrectsw.cpp
**	\brief OptimizerColorCorrectSW
**
**	$Id$
**
**	\legal
**	......... ... 2016 Ivan Mahonin
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

#include "optimizercolorcorrectsw.h"

#include <synfig/rendering/software/surfacesw.h>

#include "../task/taskcolorcorrectsw.h"


#endif

using namespace synfig;
using namespace rendering;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

void
OptimizerColorCorrectSW::run(const RunParams& params) const
{
	TaskColorCorrect::Handle color_correct = TaskColorCorrect::Handle::cast_dynamic(params.ref_task);
	if ( color_correct
	  && color_correct->target_surface
	  && color_correct.type_equal<TaskColorCorrect>() )
	{
		TaskColorCorrectSW::Handle color_correct_sw;
		init_and_assign_all<SurfaceSW>(color_correct_sw, color_correct);

		// TODO: Are we really need to check 'is_temporary' flag?
		if ( color_correct_sw->sub_task()->target_surface->is_temporary )
		{
			color_correct_sw->sub_task()->target_surface = color_correct_sw->target_surface;
			color_correct_sw->sub_task()->move_target_rect(
				color_correct_sw->get_target_offset() );
		}
		else
		{
			color_correct_sw->sub_task()->set_target_origin( VectorInt::zero() );
			color_correct_sw->sub_task()->target_surface->set_size(
				color_correct_sw->sub_task()->get_target_rect().maxx,
				color_correct_sw->sub_task()->get_target_rect().maxy );
		}
		assert( color_correct_sw->sub_task()->check() );

		apply(params, color_correct_sw);
	}
}

/* === E N T R Y P O I N T ================================================= */
