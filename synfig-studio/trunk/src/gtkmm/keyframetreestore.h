/* === S I N F G =========================================================== */
/*!	\file keyframetreestore.h
**	\brief Template Header
**
**	$Id: keyframetreestore.h,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
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

#ifndef __SINFG_STUDIO_KEYFRAMETREESTORE_H
#define __SINFG_STUDIO_KEYFRAMETREESTORE_H

/* === H E A D E R S ======================================================= */

#include <gtkmm/liststore.h>
#include <sinfgapp/canvasinterface.h>
#include <gdkmm/pixbuf.h>
#include <sinfg/keyframe.h>
#include <map>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

//class TreeRowReferenceHack;
//#define TreeRowReferenceHack Gtk::TreeRowReference

namespace studio {

class KeyframeTreeStore_Class;
	
class KeyframeTreeStore :
	public Glib::Object,
	public Gtk::TreeModel,
	public Gtk::TreeDragSource,
	public Gtk::TreeDragDest
{
	/*
 -- ** -- P U B L I C   T Y P E S ---------------------------------------------
	*/

public:

	class Model : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Gtk::TreeModelColumn<sinfg::Time> time;
		Gtk::TreeModelColumn<Glib::ustring> description;
		Gtk::TreeModelColumn<sinfg::Keyframe> keyframe;
		Gtk::TreeModelColumn<sinfg::Time> time_delta;
	
		Model()
		{
			add(time);
			add(description);
			add(keyframe);
			add(time_delta);
		}
	};

	/*
 -- ** -- P U B L I C  D A T A ------------------------------------------------
	*/

public:
	
	const Model model;

	/*
 -- ** -- P R I V A T E   D A T A ---------------------------------------------
	*/

private:

	etl::loose_handle<sinfgapp::CanvasInterface> canvas_interface_;

	//! Unique stamp for this TreeModel.
	int stamp_;
	
	static KeyframeTreeStore_Class keyframe_tree_store_class_;

	//std::map<sinfg::Keyframe,TreeRowReferenceHack> path_table_;

	sinfg::KeyframeList old_keyframe_list;

	/*
 -- ** -- P R I V A T E   M E T H O D S ---------------------------------------
	*/

private:

	void add_keyframe(sinfg::Keyframe);

	void remove_keyframe(sinfg::Keyframe);

	void change_keyframe(sinfg::Keyframe);

	static int sorter(const Gtk::TreeModel::iterator &,const Gtk::TreeModel::iterator &);

	bool iterator_sane(const GtkTreeIter* iter)const;

	bool iterator_sane(const Gtk::TreeModel::iterator& iter)const;

	void dump_iterator(const GtkTreeIter* iter, const Glib::ustring &name)const;

	void dump_iterator(const Gtk::TreeModel::iterator& iter, const Glib::ustring &name)const;

	//! Resets the iterator stamp for this model.
	/*!	This should be called whenever the class is
	**	constructed	or when large numbers of
	**	iterators become invalid. */
	void reset_stamp();
	
	//void reset_path_table();

	/*
 -- ** -- V I R T U A L   F U N C T I O N S -----------------------------------
	*/

protected:
	
	virtual void set_value_impl (const Gtk::TreeModel::iterator& row, int column, const Glib::ValueBase& value);
	virtual Gtk::TreeModelFlags  get_flags_vfunc ();
	virtual int  get_n_columns_vfunc ();
	virtual GType  get_column_type_vfunc (int index);
	virtual bool iter_next_vfunc (const iterator& iter, iterator& iter_next) const;
	virtual bool  get_iter_vfunc (const Gtk::TreeModel::Path& path, iterator& iter_next)const;
	virtual bool  iter_nth_root_child_vfunc (int n, iterator& iter)const;
	virtual Gtk::TreeModel::Path  get_path_vfunc (const iterator& iter)const;
	virtual void  ref_node_vfunc (iterator& iter)const;
	virtual void  unref_node_vfunc (iterator& iter)const;
	virtual void  get_value_vfunc (const Gtk::TreeModel::iterator& iter, int column, Glib::ValueBase& value)const;
	virtual bool	iter_is_valid (const iterator& iter) const;
	virtual int 	iter_n_root_children_vfunc () const;

	//virtual bool  iter_nth_child_vfunc (GtkTreeIter* iter, const GtkTreeIter* parent, int n);
	//virtual bool  iter_children_vfunc (GtkTreeIter* iter, const GtkTreeIter* parent);
	//virtual bool  iter_has_child_vfunc (const GtkTreeIter* iter);
	//virtual int  iter_n_children_vfunc (const GtkTreeIter* iter);
	//virtual bool  iter_parent_vfunc (GtkTreeIter* iter, const GtkTreeIter* child);

	/*
	virtual bool  get_sort_column_id_vfunc (int* sort_column_id, Gtk::SortType* order);
	virtual void  set_sort_column_id_vfunc (int sort_column_id, Gtk::SortType order);
	virtual void  set_sort_func_vfunc (int sort_column_id, GtkTreeIterCompareFunc func, void* data, GtkDestroyNotify destroy);
	virtual void  set_default_sort_func_vfunc (GtkTreeIterCompareFunc func, void* data, GtkDestroyNotify destroy);
	virtual bool  has_default_sort_func_vfunc ();
	*/
	
	/*
 -- ** -- S I G N A L   T E R M I N A L S -------------------------------------
	*/

private:

	/*
 -- ** -- P U B L I C   M E T H O D S -----------------------------------------
	*/

public:
	
	KeyframeTreeStore(etl::loose_handle<sinfgapp::CanvasInterface> canvas_interface_);
	~KeyframeTreeStore();

	etl::loose_handle<sinfgapp::CanvasInterface> canvas_interface() { return canvas_interface_; }
	etl::loose_handle<const sinfgapp::CanvasInterface> canvas_interface()const { return canvas_interface_; }
	
	sinfg::Canvas::Handle get_canvas() { return canvas_interface()->get_canvas(); }
	sinfg::Canvas::Handle get_canvas()const { return canvas_interface()->get_canvas(); }

	Gtk::TreeModel::Row find_row(const sinfg::Keyframe &keyframe);

	/*
 -- ** -- S T A T I C  M E T H O D S ------------------------------------------
	*/

public:

	static Glib::RefPtr<KeyframeTreeStore> create(etl::loose_handle<sinfgapp::CanvasInterface> canvas_interface_);

	static int time_sorter(const Gtk::TreeModel::iterator &rhs,const Gtk::TreeModel::iterator &lhs);

}; // END of class KeyframeTreeStore

//! \internal
class KeyframeTreeStore_Class : public Glib::Class
{
public:
	struct KeyframeTreeStoreClass
	{
		GObjectClass parent_class;
	};
	
	friend class KeyframeTreeStore;
	
	const Glib::Class& init();
	
	static void class_init_function(gpointer g_blass, gpointer class_data);
}; // END of CustomTreeStore_Class

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
