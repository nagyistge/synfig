/* === S I N F G =========================================================== */
/*!	\file template.h
**	\brief Template Header
**
**	$Id: selectionmanager.h,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#ifndef __SINFG_APP_SELECTIONMANAGER_H
#define __SINFG_APP_SELECTIONMANAGER_H

/* === H E A D E R S ======================================================= */

#include <list>
#include <set>
#include <ETL/handle>
#include <sinfg/layer.h>
#include <sinfg/valuenode.h>
#include "value_desc.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace sinfgapp {

class SelectionManager : public etl::shared_object
{
public:
	typedef std::pair<sinfg::Layer::Handle,sinfg::String> LayerParam;
	typedef std::list<LayerParam> LayerParamList;

	typedef std::list<sinfg::Layer::Handle> LayerList;
	typedef std::list<ValueDesc> ChildrenList;
	//typedef std::list<sinfg::ValueNode::Handle> ValueNodeList;

	virtual ~SelectionManager() { }

	//! Returns the number of layers selected.
	virtual int get_selected_layer_count()const=0;

	//! Returns a list of the currently selected layers.
	virtual LayerList get_selected_layers()const=0;
	
	//! Returns the first layer selected or an empty handle if none are selected.
	virtual sinfg::Layer::Handle get_selected_layer()const=0;
	
	//! Sets which layers should be selected
	virtual void set_selected_layers(const LayerList &layer_list)=0;

	//! Sets which layer should be selected. Empty handle if none.
	virtual void set_selected_layer(const sinfg::Layer::Handle &layer)=0;

	//! Clears the layer selection list
	virtual void clear_selected_layers()=0;



	//! Returns the number of childrens selected.
	virtual int get_selected_children_count()const=0;

	//! Returns a list of the currently selected childrens.
	virtual ChildrenList get_selected_children()const=0;
	
	//! Returns the first children selected or an empty handle if none are selected.
	virtual ChildrenList::value_type get_selected_child()const=0;
	
	//! Sets which childrens should be selected
	virtual void set_selected_children(const ChildrenList &children_list)=0;

	//! Sets which children should be selected. Empty handle if none.
	virtual void set_selected_child(const ChildrenList::value_type &children)=0;

	//! Clears the children selection list
	virtual void clear_selected_children()=0;


	//! Returns the number of layer parameters selected.
	virtual int get_selected_layer_parameter_count()const=0;

	//! Returns a list of the currently selected layer parameters.
	virtual LayerParamList get_selected_layer_parameters()const=0;
	
	//! Returns the first layer parameter selected or an empty handle if none are selected.
	virtual LayerParam get_selected_layer_parameter()const=0;
	
	//! Sets which layer parameters should be selected
	virtual void set_selected_layer_parameters(const LayerParamList &layer_param_list)=0;

	//! Sets which layer parameter should be selected. Empty handle if none.
	virtual void set_selected_layer_param(const LayerParam &layer_param)=0;

	//! Clears the layer parameter selection list
	virtual void clear_selected_layer_parameters()=0;
}; // END of class SelectionManager

//! A place holding selection manager that does nothing
class NullSelectionManager : public SelectionManager
{
public:
	int get_selected_layer_count()const { return 0; }
	LayerList get_selected_layers()const { return LayerList(); }
	sinfg::Layer::Handle get_selected_layer()const { return 0; }
	void set_selected_layers(const LayerList &layer_list) { return; }
	void set_selected_layer(const sinfg::Layer::Handle &layer) { return; }
	void clear_selected_layers() { return; }


	int get_selected_children_count()const { return 0; }
	ChildrenList get_selected_children()const { return ChildrenList(); }
	ChildrenList::value_type get_selected_child()const { return ChildrenList::value_type(); }
	void set_selected_children(const ChildrenList &children_list) { return; }
	void set_selected_child(const ChildrenList::value_type &child) { return; }
	void clear_selected_children() { return; }

	int get_selected_layer_parameter_count()const { return 0; }
	LayerParamList get_selected_layer_parameters()const { return LayerParamList(); }
	LayerParam get_selected_layer_parameter()const { return LayerParam(); }
	void set_selected_layer_parameters(const LayerParamList &layer_param_list) { return; }
	void set_selected_layer_param(const LayerParam &layer_param) { return; }
	void clear_selected_layer_parameters() { return; }

}; // END of class NullSelectionManager

}; // END of namespace sinfgapp

/* === E N D =============================================================== */

#endif
