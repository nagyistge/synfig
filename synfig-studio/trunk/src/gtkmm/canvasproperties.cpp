/* === S I N F G =========================================================== */
/*!	\file canvasproperties.cpp
**	\brief Template File
**
**	$Id: canvasproperties.cpp,v 1.1.1.1 2005/01/07 03:34:35 darco Exp $
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

#include "canvasproperties.h"
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <sinfgapp/canvasinterface.h>
#include "metadatatreestore.h"
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include "app.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace studio;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

CanvasProperties::CanvasProperties(Gtk::Window& parent,handle<sinfgapp::CanvasInterface> canvas_interface):
	Gtk::Dialog(_("Canvas Properties"),parent,false,true),
	canvas_interface_(canvas_interface)
{
	widget_rend_desc.show();

	widget_rend_desc.signal_changed().connect(sigc::mem_fun(*this,&studio::CanvasProperties::on_rend_desc_changed));

	Gtk::Frame *info_frame=manage(new Gtk::Frame(_("Canvas Info")));
	
	Gtk::Table *info_table=manage(new Gtk::Table(2,2,false));
	info_frame->add(*info_table);

	// The root canvas doesn't have an ID, so don't
	// display it if this is a root canvas.
	if(!canvas_interface_->get_canvas()->is_root())
	{
		info_table->attach(*manage(new Gtk::Label(_("ID"))), 0, 1, 0, 1, Gtk::SHRINK|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);	
		info_table->attach(entry_id, 1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);
	}
	info_table->attach(*manage(new Gtk::Label(_("Name"))), 0, 1, 1, 2, Gtk::SHRINK|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);	
	info_table->attach(*manage(new Gtk::Label(_("Description"))), 0, 1, 2, 3, Gtk::SHRINK|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);	
	info_table->attach(entry_name, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);	
	info_table->attach(entry_description, 1, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);	

	info_frame->show_all();
	
	get_vbox()->pack_start(*info_frame);
	
	Gtk::Notebook* notebook(manage(new Gtk::Notebook()));
	notebook->show();
	notebook->append_page(widget_rend_desc,_("RendDesc"));
	//notebook->append_page(create_meta_data_view(),_("MetaData"));
	
	get_vbox()->pack_start(*notebook);

	canvas_interface_->signal_rend_desc_changed().connect(sigc::mem_fun(*this,&studio::CanvasProperties::refresh));

	Gtk::Button *ok_button(manage(new class Gtk::Button(Gtk::StockID("gtk-ok"))));
	ok_button->show();
	add_action_widget(*ok_button,2);
	ok_button->signal_clicked().connect(sigc::mem_fun(*this, &studio::CanvasProperties::on_ok_pressed));

	Gtk::Button *apply_button(manage(new class Gtk::Button(Gtk::StockID("gtk-apply"))));
	apply_button->show();
	add_action_widget(*apply_button,1);
	apply_button->signal_clicked().connect(sigc::mem_fun(*this, &studio::CanvasProperties::on_apply_pressed));

	Gtk::Button *cancel_button(manage(new class Gtk::Button(Gtk::StockID("gtk-close"))));
	cancel_button->show();
	add_action_widget(*cancel_button,0);
	cancel_button->signal_clicked().connect(sigc::mem_fun(*this, &studio::CanvasProperties::on_cancel_pressed));

	//set_default_response(1);
	
	refresh();

	update_title();
}

Gtk::Widget&
CanvasProperties::create_meta_data_view()
{
	MetaDataTreeStore::Model model;
	meta_data_tree_view=(manage(new class Gtk::TreeView()));
		
	meta_data_tree_view->append_column(_("Key"),model.key);
	meta_data_tree_view->append_column_editable(_("Data"),model.data);
	meta_data_tree_view->set_model(MetaDataTreeStore::create(canvas_interface_));
	meta_data_tree_view->set_rules_hint();
	meta_data_tree_view->show();
	
	Gtk::ScrolledWindow *scrolledwindow = manage(new class Gtk::ScrolledWindow());
	scrolledwindow->set_flags(Gtk::CAN_FOCUS);
	scrolledwindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolledwindow->add(*meta_data_tree_view);
	scrolledwindow->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	scrolledwindow->show();

	
	
	Gtk::Table *table=manage(new Gtk::Table());
	table->attach(*scrolledwindow, 0, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
	
	Gtk::Button* button_add(manage(new Gtk::Button(Gtk::StockID("gtk-add"))));
	button_add->show();
	button_add->signal_clicked().connect(sigc::mem_fun(*this,&CanvasProperties::on_button_meta_data_add));
	table->attach(*button_add, 0, 1, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);

	Gtk::Button* button_delete(manage(new Gtk::Button(Gtk::StockID("gtk-delete"))));
	button_delete->show();
	button_delete->signal_clicked().connect(sigc::mem_fun(*this,&CanvasProperties::on_button_meta_data_delete));	
	table->attach(*button_delete, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);
	
	table->show();
	return *table;
}

void
CanvasProperties::on_button_meta_data_add()
{
	sinfg::String key;
	if(App::dialog_entry("New MetaData Entry", "Please enter the name of the key",key) && !key.empty())
	{
		canvas_interface_->set_meta_data(key," ");
	}

}

void
CanvasProperties::on_button_meta_data_delete()
{
}

void
CanvasProperties::update_title()
{
	set_title(_("Properties")+String(" - ")+canvas_interface_->get_canvas()->get_name());
}

void
CanvasProperties::refresh()
{
	widget_rend_desc.set_rend_desc(canvas_interface_->get_canvas()->rend_desc());
	entry_id.set_text(canvas_interface_->get_canvas()->get_id());
	entry_name.set_text(canvas_interface_->get_canvas()->get_name());
	entry_description.set_text(canvas_interface_->get_canvas()->get_description());
	
	dirty_rend_desc=false;

	update_title();
}

CanvasProperties::~CanvasProperties()
{
}

void
CanvasProperties::on_rend_desc_changed()
{
	dirty_rend_desc=true;
}

void
CanvasProperties::on_apply_pressed()
{
	if(dirty_rend_desc)
		canvas_interface_->set_rend_desc(widget_rend_desc.get_rend_desc());
	if(entry_id.get_text()!=canvas_interface_->get_canvas()->get_id())
		canvas_interface_->set_id(entry_id.get_text());
	if(entry_name.get_text()!=canvas_interface_->get_canvas()->get_name())
		canvas_interface_->set_name(entry_name.get_text());
	if(entry_description.get_text()!=canvas_interface_->get_canvas()->get_description())
		canvas_interface_->set_description(entry_description.get_text());
	
	dirty_rend_desc=false;
}

void
CanvasProperties::on_ok_pressed()
{
	on_apply_pressed();
	hide();
}

void
CanvasProperties::on_cancel_pressed()
{
	refresh();
	hide();
}
