/* === S I N F G =========================================================== */
/*!	\file layeradd.cpp
**	\brief Template File
**
**	$Id: layeradd.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "layeradd.h"
#include <sinfgapp/canvasinterface.h>

#endif

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace sinfgapp;
using namespace Action;

/* === M A C R O S ========================================================= */

ACTION_INIT(Action::LayerAdd);
ACTION_SET_NAME(Action::LayerAdd,"layer_add");
ACTION_SET_LOCAL_NAME(Action::LayerAdd,"Add Layer");
ACTION_SET_TASK(Action::LayerAdd,"add");
ACTION_SET_CATEGORY(Action::LayerAdd,Action::CATEGORY_LAYER);
ACTION_SET_PRIORITY(Action::LayerAdd,0);
ACTION_SET_VERSION(Action::LayerAdd,"0.0");
ACTION_SET_CVS_ID(Action::LayerAdd,"$Id: layeradd.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $");

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Action::LayerAdd::LayerAdd()
{
}

Action::ParamVocab
Action::LayerAdd::get_param_vocab()
{
	ParamVocab ret(Action::CanvasSpecific::get_param_vocab());
	
	ret.push_back(ParamDesc("new",Param::TYPE_LAYER)
		.set_local_name(_("New Layer"))
		.set_desc(_("Layer to be added"))
	);
	
	return ret;
}

bool
Action::LayerAdd::is_canidate(const ParamList &x)
{
	return canidate_check(get_param_vocab(),x);
}

bool
Action::LayerAdd::set_param(const sinfg::String& name, const Action::Param &param)
{
	if(name=="new" && param.get_type()==Param::TYPE_LAYER)
	{
		layer=param.get_layer();
		
		return true;
	}

	return Action::CanvasSpecific::set_param(name,param);
}

bool
Action::LayerAdd::is_ready()const
{
	if(!layer)
		return false;
	return Action::CanvasSpecific::is_ready();
}

void
Action::LayerAdd::perform()
{
	// Set the layer's canvas
	layer->set_canvas(get_canvas());
		
	// Push the layer onto the front of the canvas
	get_canvas()->push_front(layer);	

	// Mark ourselves as dirty if necessary
	//set_dirty(layer->active());
	
	// Signal that a layer has been inserted
	if(get_canvas_interface())
	{
		get_canvas_interface()->signal_layer_inserted()(layer,0);
	}
	else sinfg::warning("CanvasInterface not set on action");
}

void
Action::LayerAdd::undo()
{
	// Find the iterator for the layer
	Canvas::iterator iter=find(get_canvas()->begin(),get_canvas()->end(),layer);
		
	// If we couldn't find the layer in the canvas, then bail
	if(*iter!=layer)
		throw Error(_("This layer doesn't exist anymore."));
	
	// Remove the layer from the canvas
	get_canvas()->erase(iter);

	// Mark ourselves as dirty if necessary
	//set_dirty(layer->active());
	
	// Signal that a layer has been inserted
	if(get_canvas_interface())
	{
		get_canvas_interface()->signal_layer_removed()(layer);
	}
	else sinfg::warning("CanvasInterface not set on action");
}
