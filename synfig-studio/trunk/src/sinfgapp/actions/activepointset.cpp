/* === S I N F G =========================================================== */
/*!	\file activepointset.cpp
**	\brief Template File
**
**	$Id: activepointset.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#include "activepointset.h"
#include <sinfgapp/canvasinterface.h>

#endif

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace sinfgapp;
using namespace Action;

/* === M A C R O S ========================================================= */

ACTION_INIT(Action::ActivepointSet);
ACTION_SET_NAME(Action::ActivepointSet,"activepoint_set");
ACTION_SET_LOCAL_NAME(Action::ActivepointSet,"Set Activepoint");
ACTION_SET_TASK(Action::ActivepointSet,"set");
ACTION_SET_CATEGORY(Action::ActivepointSet,Action::CATEGORY_ACTIVEPOINT);
ACTION_SET_PRIORITY(Action::ActivepointSet,0);
ACTION_SET_VERSION(Action::ActivepointSet,"0.0");
ACTION_SET_CVS_ID(Action::ActivepointSet,"$Id: activepointset.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $");

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Action::ActivepointSet::ActivepointSet()
{
	set_dirty(true);
}

Action::ParamVocab
Action::ActivepointSet::get_param_vocab()
{
	ParamVocab ret(Action::CanvasSpecific::get_param_vocab());
	
	ret.push_back(ParamDesc("value_desc",Param::TYPE_VALUEDESC)
		.set_local_name(_("ValueDesc"))
	);

	ret.push_back(ParamDesc("activepoint",Param::TYPE_ACTIVEPOINT)
		.set_local_name(_("Activepoint"))
		.set_desc(_("Activepoint to be changed"))
		.set_supports_multiple()
	);

	return ret;
}

bool
Action::ActivepointSet::is_canidate(const ParamList &x)
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
Action::ActivepointSet::set_param(const sinfg::String& name, const Action::Param &param)
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
	if(name=="activepoint" && param.get_type()==Param::TYPE_ACTIVEPOINT)
	{
		//NOTE: there is no duplication checking at the moment
		activepoints.push_back(param.get_activepoint());
		
		return true;
	}

	return Action::CanvasSpecific::set_param(name,param);
}

bool
Action::ActivepointSet::is_ready()const
{
	if(!value_node || activepoints.empty())
		return false;
	return Action::CanvasSpecific::is_ready();
}

void
Action::ActivepointSet::perform()
{	
	typedef ValueNode_DynamicList::ListEntry::ActivepointList AList;
	AList::iterator iter;

#if 1	
	vector<AList::iterator>	iters;
	vector<Activepoint>::iterator i = activepoints.begin(), end = activepoints.end();	
	
	for(; i != end; ++i)
	{
		try { iters.push_back(value_node->list[index].find(*i)); }
		catch(sinfg::Exception::NotFound)
		{
			throw Error(_("Unable to find activepoint"));
		}
	}
	
	//check to see which valuenodes are going to override because of the time...
	ValueNode_DynamicList::ListEntry::findresult timeiter;
	
	for(i = activepoints.begin(); i != end; ++i)
	{
		timeiter = value_node->list[index].find_time(i->get_time());
		
		bool candelete = timeiter.second;
	
		//we only want to track overwrites (not activepoints that are also being modified)
		if(candelete)
		{
			for(vector<AList::iterator>::iterator ii = iters.begin(); ii != iters.end(); ++ii)
			{
				if(timeiter.first == *ii)
				{
					candelete = false;
					break;
				}
			}
		}
		
		//if we can still delete it after checking, record it, and then remove them all later
		if(candelete)
		{
			Activepoint a = *timeiter.first;
			overwritten_activepoints.push_back(a);
		}
	}
	
	//overwrite all the valuenodes we're supposed to set
	{
		i = activepoints.begin();
		for(vector<AList::iterator>::iterator ii = iters.begin(); ii != iters.end() && i != end; ++ii, ++i)
		{
			old_activepoints.push_back(**ii);
			**ii = *i; //set the point to the corresponding point in the normal waypoint list
		}
	}
	
	//remove all the points we're supposed to be overwritting
	{
		vector<Activepoint>::iterator 	oi = overwritten_activepoints.begin(),
										oend = overwritten_activepoints.end();
		for(; oi != oend; ++oi)
		{
			value_node->list[index].erase(*oi);
		}
	}

#else	
	try { iter=value_node->list[index].find(activepoint); }
	catch(sinfg::Exception::NotFound)
	{
		throw Error(_("Unable to find activepoint"));
	}	

	//find the value at the old time before we replace it
	ValueNode_DynamicList::ListEntry::findresult timeiter;
	timeiter = value_node->list[index].find_time(activepoint.get_time());
	
	//we only want to track overwrites (not inplace modifications)
	if(timeiter.second && activepoint.get_uid() == timeiter.first->get_uid())
	{
		timeiter.second = false;
	}
		
	old_activepoint=*iter;
	*iter=activepoint;
	
	if(timeiter.second)
	{
		sinfg::info("Erasing the found activepoint");
		time_overwrite = true;
		overwritten_ap = *timeiter.first;
		
		value_node->list[index].erase(overwritten_ap);
	}
	
#endif
	
	value_node->list[index].timing_info.sort();
	
	// Signal that a valuenode has been changed
	value_node->changed();
}

void
Action::ActivepointSet::undo()
{
	ValueNode_DynamicList::ListEntry::ActivepointList::iterator iter;

#if 1	
	vector<Activepoint>::iterator i = old_activepoints.begin(), end = old_activepoints.end();	
	
	for(; i != end; ++i)
	{
		try { iter = value_node->list[index].find(*i); }
		catch(sinfg::Exception::NotFound)
		{
			throw Error(_("Unable to find activepoint"));
		}
		
		//overwrite with old one
		*iter = *i;
	}
		
	//add back in all the points that we removed before...
	{
		vector<Activepoint>::iterator 	oi = overwritten_activepoints.begin(),
										oend = overwritten_activepoints.end();
		for(; oi != oend; ++oi)
		{
			value_node->list[index].add(*oi);
		}
	}

#else	
	try { iter=value_node->list[index].find(activepoint); }
	catch(sinfg::Exception::NotFound)
	{
		throw Error(_("Unable to find activepoint"));
	}	

	*iter=old_activepoint;
		
	if(time_overwrite)
	{
		value_node->list[index].add(overwritten_ap);
	}
#endif
	
	value_node->list[index].timing_info.sort();
	
	// Signal that a valuenode has been changed
	value_node->changed();
}
