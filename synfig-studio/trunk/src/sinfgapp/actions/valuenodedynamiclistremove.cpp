/* === S I N F G =========================================================== */
/*!	\file valuenodedynamiclistremove.cpp
**	\brief Template File
**
**	$Id: valuenodedynamiclistremove.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#include "valuenodedynamiclistremove.h"
#include <sinfgapp/canvasinterface.h>

#endif

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace sinfgapp;
using namespace Action;

/* === M A C R O S ========================================================= */

ACTION_INIT(Action::ValueNodeDynamicListRemove);
ACTION_SET_NAME(Action::ValueNodeDynamicListRemove,"value_node_dynamic_list_remove");
ACTION_SET_LOCAL_NAME(Action::ValueNodeDynamicListRemove,"Remove Item");
ACTION_SET_TASK(Action::ValueNodeDynamicListRemove,"remove");
ACTION_SET_CATEGORY(Action::ValueNodeDynamicListRemove,Action::CATEGORY_VALUEDESC|Action::CATEGORY_VALUENODE|Action::CATEGORY_HIDDEN);
ACTION_SET_PRIORITY(Action::ValueNodeDynamicListRemove,-19);
ACTION_SET_VERSION(Action::ValueNodeDynamicListRemove,"0.0");
ACTION_SET_CVS_ID(Action::ValueNodeDynamicListRemove,"$Id: valuenodedynamiclistremove.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $");

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Action::ValueNodeDynamicListRemove::ValueNodeDynamicListRemove()
{
	index=0;
	set_dirty(true);
}

Action::ParamVocab
Action::ValueNodeDynamicListRemove::get_param_vocab()
{
	ParamVocab ret(Action::CanvasSpecific::get_param_vocab());
	
	ret.push_back(ParamDesc("value_desc",Param::TYPE_VALUEDESC)
		.set_local_name(_("ValueDesc"))
	);

	return ret;
}

bool
Action::ValueNodeDynamicListRemove::is_canidate(const ParamList &x)
{
	if(canidate_check(get_param_vocab(),x))
	{
		ValueDesc value_desc(x.find("value_desc")->second.get_value_desc());
		if(!value_desc.parent_is_value_node() || !ValueNode_DynamicList::Handle::cast_dynamic(value_desc.get_parent_value_node()))
			return false;

		return true;
	}
	return false;
}

bool
Action::ValueNodeDynamicListRemove::set_param(const sinfg::String& name, const Action::Param &param)
{
	if(name=="value_desc" && param.get_type()==Param::TYPE_VALUEDESC)
	{
		ValueDesc value_desc(param.get_value_desc());
		
		if(!value_desc.parent_is_value_node())
			return false;
		
		value_node=ValueNode_DynamicList::Handle::cast_dynamic(value_desc.get_parent_value_node());
		
		if(!value_node)
			return false;
		
		index=value_desc.get_index();
		
		return true;
	}
	
	return Action::CanvasSpecific::set_param(name,param);
}

bool
Action::ValueNodeDynamicListRemove::is_ready()const
{
	if(!value_node)
		return false;
	return Action::CanvasSpecific::is_ready();
}

void
Action::ValueNodeDynamicListRemove::perform()
{	
	if(index>=value_node->link_count())
		index=value_node->link_count()-1;

	list_entry=value_node->list[index];
	value_node->erase((value_node->list.begin()+index)->value_node);
		
	// Signal that a layer has been inserted
	value_node->changed();
/*_if(get_canvas_interface())
	{
		get_canvas_interface()->signal_value_node_changed()(value_node);
	}
	else sinfg::warning("CanvasInterface not set on action");*/
}

void
Action::ValueNodeDynamicListRemove::undo()
{
	value_node->add(list_entry,index);
	
	// Signal that a layer has been inserted
	value_node->changed();
/*_if(get_canvas_interface())
	{
		get_canvas_interface()->signal_value_node_changed()(value_node);
	}
	else sinfg::warning("CanvasInterface not set on action");*/
}
