/* === S Y N F I G ========================================================= */
/*!	\file valuenode_log.cpp
**	\brief Implementation of the "Natural Logarithm" valuenode conversion.
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007, 2008 Chris Moore
**  Copyright (c) 2008 Carlos López
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

#include "valuenode_log.h"
#include "valuenode_const.h"
#include "general.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

ValueNode_Logarithm::ValueNode_Logarithm(const ValueBase &x):
	LinkableValueNode(x.get_type())
{
	Real value(x.get(Real()));
	Real infinity(999999.0);
	Real epsilon(0.000001);

	value = exp(value);
	
	set_link("link",     ValueNode_Const::create(Real(value)));
	set_link("epsilon",  ValueNode_Const::create(Real(epsilon)));
	set_link("infinite", ValueNode_Const::create(Real(infinity)));
}

ValueNode_Logarithm*
ValueNode_Logarithm::create(const ValueBase &x)
{
	return new ValueNode_Logarithm(x);
}

LinkableValueNode*
ValueNode_Logarithm::create_new()const
{
	return new ValueNode_Logarithm(get_type());
}

ValueNode_Logarithm::~ValueNode_Logarithm()
{
	unlink_all();
}

bool
ValueNode_Logarithm::set_link_vfunc(int i,ValueNode::Handle value)
{
	assert(i>=0 && i<link_count());

	switch(i)
	{
	case 0: CHECK_TYPE_AND_SET_VALUE(link_,     ValueBase::TYPE_REAL);
	case 1: CHECK_TYPE_AND_SET_VALUE(epsilon_,  ValueBase::TYPE_REAL);
	case 2: CHECK_TYPE_AND_SET_VALUE(infinite_, ValueBase::TYPE_REAL);
	}
	return false;
}

ValueNode::LooseHandle
ValueNode_Logarithm::get_link_vfunc(int i)const
{
	assert(i>=0 && i<link_count());

	if(i==0) return link_;
	if(i==1) return epsilon_;
	if(i==2) return infinite_;

	return 0;
}

int
ValueNode_Logarithm::link_count()const
{
	return 3;
}

String
ValueNode_Logarithm::link_local_name(int i)const
{
	assert(i>=0 && i<link_count());

	if(i==0) return _("Link");
	if(i==1) return _("Epsilon");
	if(i==2) return _("Infinite");
	return String();
}

String
ValueNode_Logarithm::link_name(int i)const
{
	assert(i>=0 && i<link_count());

	if(i==0) return "link";
	if(i==1) return "epsilon";
	if(i==2) return "infinite";
	return String();
}

int
ValueNode_Logarithm::get_link_index_from_name(const String &name)const
{
	if(name=="link")     return 0;
	if(name=="epsilon")  return 1;
	if(name=="infinite") return 2;

	throw Exception::BadLinkName(name);
}

ValueBase
ValueNode_Logarithm::operator()(Time t)const
{
	Real link     = (*link_)    (t).get(Real());
	Real epsilon  = (*epsilon_) (t).get(Real());
	Real infinite = (*infinite_)(t).get(Real());

	if (epsilon < 0.00000001)
		epsilon = 0.00000001;

	if (link < epsilon)
			return -infinite;
	else
		return log(link);
}

String
ValueNode_Logarithm::get_name()const
{
	return "logarithm";
}

String
ValueNode_Logarithm::get_local_name()const
{
	return _("Logarithm");
}

bool
ValueNode_Logarithm::check_type(ValueBase::Type type)
{
	return type==ValueBase::TYPE_REAL;
}

 	  	 