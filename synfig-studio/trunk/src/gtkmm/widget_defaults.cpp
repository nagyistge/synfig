/* === S I N F G =========================================================== */
/*!	\file widget_defaults.cpp
**	\brief Template File
**
**	$Id: widget_defaults.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#include "widget_defaults.h"
#include "widget_color.h"
#include "widget_gradient.h"
#include "dialog_color.h"
#include "dialog_gradient.h"
#include "app.h"
#include <gtkmm/menu.h>
#include <gtkmm/scale.h>
#include <sinfg/exception.h>
#include <sinfgapp/main.h>
#include "canvasview.h"
#include "widget_distance.h"
#include "widget_enum.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace studio;

/* === M A C R O S ========================================================= */

#define GRADIENT_HEIGHT		16
#define DEFAULT_INCREMENT	(0.25)
#define DEFAULT_WIDTH		(sinfg::Distance(3,sinfg::Distance::SYSTEM_POINTS))

/* === G L O B A L S ======================================================= */

class studio::Widget_Brush : public Gtk::DrawingArea
{
public:
	Widget_Brush()
	{
		signal_expose_event().connect(sigc::mem_fun(*this, &studio::Widget_Brush::redraw));

		set_size_request(24,24);
		add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
		add_events(Gdk::BUTTON1_MOTION_MASK);

		sinfgapp::Main::signal_foreground_color_changed().connect(sigc::mem_fun(*this,&studio::Widget_Brush::queue_draw));
		sinfgapp::Main::signal_background_color_changed().connect(sigc::mem_fun(*this,&studio::Widget_Brush::queue_draw));
		sinfgapp::Main::signal_bline_width_changed().connect(sigc::mem_fun(*this,&studio::Widget_Brush::queue_draw));
		studio::App::signal_instance_selected().connect(sigc::hide(sigc::mem_fun(*this,&studio::Widget_Brush::queue_draw)));
	}
	
	bool
	redraw(GdkEventExpose*bleh)
	{
		Glib::RefPtr<Gdk::GC> gc(Gdk::GC::create(get_window()));
		
		const int h(get_height());
		const int w(get_width());	

		float pixelsize(0);
		if(App::get_selected_canvas_view())
		{
			const RendDesc& rend_desc(App::get_selected_canvas_view()->get_canvas()->rend_desc());
			pixelsize=sinfgapp::Main::get_bline_width().get(Distance::SYSTEM_PIXELS,rend_desc);
		}
		else 
		{
			RendDesc rend_desc;
			pixelsize=sinfgapp::Main::get_bline_width().get(Distance::SYSTEM_PIXELS,rend_desc);
		}
		// Fill in the background color
		render_color_to_window(get_window(),Gdk::Rectangle(0,0,w,h),sinfgapp::Main::get_background_color());

/*
		gc->set_rgb_fg_color(colorconv_sinfg2gdk(sinfgapp::Main::get_background_color()));
		gc->set_line_attributes(1,Gdk::LINE_SOLID,Gdk::CAP_BUTT,Gdk::JOIN_MITER);	
		get_window()->draw_rectangle(
			gc,
			true,	// Fill?
			0,0,	// x,y
			w,h	//w,h
		);
*/
		
		// Draw in the circle
		gc->set_rgb_fg_color(colorconv_sinfg2gdk(sinfgapp::Main::get_foreground_color()));
		gc->set_function(Gdk::COPY);
		gc->set_line_attributes(1,Gdk::LINE_SOLID,Gdk::CAP_BUTT,Gdk::JOIN_MITER);
		get_window()->draw_arc(
			gc,
			true,
			round_to_int(((float)w/2.0f)-pixelsize/2.0f),
			round_to_int(((float)h/2.0f)-pixelsize/2.0f),
			round_to_int(pixelsize+0.6),
			round_to_int(pixelsize+0.6),
			0,
			360*64
		);  
			
		return true;
	}

	bool
	on_event(GdkEvent *event)
	{
//		const int x(static_cast<int>(event->button.x));
		const int y(static_cast<int>(event->button.y));
		
		const int h(get_height());
//		const int w(get_width());	
		
		switch(event->type)
		{
			case GDK_MOTION_NOTIFY:
				break;
			case GDK_BUTTON_RELEASE:
				if(event->button.button==1) // Left click
				{
					Distance dist(sinfgapp::Main::get_bline_width());
					
					if(y<h/2) // increase BLine size
					{
						dist+=DEFAULT_INCREMENT;
					}
					else // Decrease BLine size
					{
						dist-=DEFAULT_INCREMENT;
					}
					sinfgapp::Main::set_bline_width(dist);
					return true;
				}
				if(event->button.button==3)
				{
					// right click on bline width
					sinfgapp::Main::set_bline_width(DEFAULT_WIDTH);
					return true;
				}
				break;
			case GDK_SCROLL:
				{
					Distance dist(sinfgapp::Main::get_bline_width());
					
					if(event->scroll.direction==GDK_SCROLL_UP)
					{
						dist+=DEFAULT_INCREMENT;
					}
					else if(event->scroll.direction==GDK_SCROLL_DOWN)
					{
						dist-=DEFAULT_INCREMENT;
					}
					sinfgapp::Main::set_bline_width(dist);
					return true;
				}
			default:
				break;
		}
		
		return false;
	}

};

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Widget_Defaults::Widget_Defaults()
{
	//set_size_request(48,48+GRADIENT_HEIGHT+16);
	//set_size_request(48,-1);
	
	{
		Gtk::Table* subtable(manage(new Gtk::Table()));
	
		// Foreground Color
		widget_fg_color=manage(new Widget_Color());
		widget_fg_color->show();
		widget_fg_color->set_size_request(16,16);
		widget_fg_color->signal_clicked().connect(sigc::mem_fun(*this,&Widget_Defaults::on_fg_color_clicked));
		subtable->attach(*widget_fg_color, 0, 2, 0, 2, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
		tooltips_.set_tip(*widget_fg_color,_("Foreground Color"));
	
		// Background Color
		widget_bg_color=manage(new Widget_Color());
		widget_bg_color->show();
		widget_bg_color->set_size_request(16,16);
		widget_bg_color->signal_clicked().connect(sigc::mem_fun(*this,&Widget_Defaults::on_bg_color_clicked));
		subtable->attach(*widget_bg_color, 1, 3, 1, 3, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 0, 0);
		tooltips_.set_tip(*widget_bg_color,_("Background Color"));

		Gtk::Image* icon;
		
		// Swap button
		Gtk::Button* button_swap(manage(new Gtk::Button()));
		button_swap->show();
		button_swap->set_relief(Gtk::RELIEF_NONE);
		button_swap->set_border_width(0);
		icon=manage(new Gtk::Image(Gtk::StockID("sinfg-swap_colors"),Gtk::IconSize(1)));
		icon->show();
		button_swap->add(*icon);
		//button_swap->get_child()->set_size_request(16/3,16/3);
		//button_swap->set_size_request(16/3,16/3);
		dynamic_cast<Gtk::Misc*>(button_swap->get_child())->set_padding(0,0);
		button_swap->signal_clicked().connect(sigc::mem_fun(*this,&Widget_Defaults::on_swap_color_clicked));
		subtable->attach(*button_swap, 2, 3, 0, 1, Gtk::SHRINK|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);
		tooltips_.set_tip(*button_swap,_("Swap Background and\nForeground Colors"));

		// Reset button
		Gtk::Button* button_reset(manage(new Gtk::Button("R")));
		button_reset->show();
		button_reset->set_relief(Gtk::RELIEF_NONE);
		button_reset->set_border_width(0);
		dynamic_cast<Gtk::Misc*>(button_reset->get_child())->set_padding(0,0);
		//button_reset->set_size_request(16/3,16/3);
		button_reset->signal_clicked().connect(sigc::mem_fun(*this,&Widget_Defaults::on_reset_color_clicked));
		subtable->attach(*button_reset, 0,1, 2, 3, Gtk::SHRINK|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);
		tooltips_.set_tip(*button_reset,_("Reset Colors to Black and White"));

		
		attach(*subtable, 0, 1, 0, 2, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 1, 1);
		subtable->set_size_request(36,36);
		subtable->set_homogeneous(true);
		subtable->show();
	}
	widget_brush=manage(new Widget_Brush());
	widget_brush->show();
	widget_brush->set_size_request(36,36);
	attach(*widget_brush,1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::EXPAND|Gtk::FILL, 1, 1);
	tooltips_.set_tip(*widget_brush,_("Brush Preview"));

	widget_bline_width=manage(new Widget_Distance());
	widget_bline_width->show();
	widget_bline_width->set_digits(2);
	widget_bline_width->set_size_request(24,-1);
	widget_bline_width->signal_value_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::on_bline_width_changed));
	attach(*widget_bline_width,1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 0, 0);
	tooltips_.set_tip(*widget_bline_width,_("Brush Size"));


	widget_blend_method=manage(new Widget_Enum());
	widget_blend_method->show();
	widget_blend_method->signal_activate().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::on_blend_method_changed));
	widget_blend_method->set_param_desc(ParamDesc(Color::BLEND_COMPOSITE,"blend_method"));
	attach(*widget_blend_method,0, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 1, 1);
	tooltips_.set_tip(*widget_blend_method,_("Default Blend Method"));

	widget_interpolation=manage(new Widget_Enum());
	widget_interpolation->show();
	widget_interpolation->signal_activate().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::on_interpolation_changed));
	widget_interpolation->set_param_desc(
		ParamDesc("interpolation")
			.set_hint("enum")
			.add_enum_value(INTERPOLATION_TCB,"auto",_("TBC"))
			.add_enum_value(INTERPOLATION_CONSTANT,"constant",_("Constant"))
			.add_enum_value(INTERPOLATION_HALT,"ease",_("Ease in/out"))
			.add_enum_value(INTERPOLATION_LINEAR,"linear",_("Linear"))
	);
	attach(*widget_interpolation,0, 2, 5, 6, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 1, 1);
	tooltips_.set_tip(*widget_interpolation,_("Default Interpolation"));

	widget_opacity=manage(new Gtk::HScale(0.0f,1.01f,0.01f));
	widget_opacity->show();
	widget_opacity->set_digits(2);
	widget_opacity->set_value_pos(Gtk::POS_LEFT);
	widget_opacity->signal_value_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::on_opacity_changed));
	attach(*widget_opacity,0, 2, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 1, 1);
	tooltips_.set_tip(*widget_opacity,_("Default Opacity"));

	widget_gradient=manage(new Widget_Gradient());
	widget_gradient->show();
	widget_gradient->set_size_request(-1,GRADIENT_HEIGHT);
	widget_gradient->signal_clicked().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::on_gradient_clicked));
	attach(*widget_gradient,0, 2, 4, 5, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK|Gtk::FILL, 1, 1);
	tooltips_.set_tip(*widget_gradient,_("Default Gradient"));


	// Signals
	sinfgapp::Main::signal_opacity_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::opacity_refresh));
	sinfgapp::Main::signal_bline_width_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::bline_width_refresh));
	sinfgapp::Main::signal_foreground_color_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::fg_color_refresh));
	sinfgapp::Main::signal_background_color_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::bg_color_refresh));
	sinfgapp::Main::signal_gradient_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::gradient_refresh));
	sinfgapp::Main::signal_blend_method_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::blend_method_refresh));
	sinfgapp::Main::signal_interpolation_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::interpolation_refresh));

	fg_color_refresh();
	bg_color_refresh();
	gradient_refresh();
	bline_width_refresh();
	blend_method_refresh();
	opacity_refresh();
	interpolation_refresh();
/*
	set_size_request(48,48+GRADIENT_HEIGHT);
	signal_expose_event().connect(sigc::mem_fun(*this, &studio::Widget_Defaults::redraw));
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	add_events(Gdk::BUTTON1_MOTION_MASK);

	sinfgapp::Main::signal_foreground_color_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::queue_draw));
	sinfgapp::Main::signal_background_color_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::queue_draw));
	sinfgapp::Main::signal_gradient_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::queue_draw));
	sinfgapp::Main::signal_bline_width_changed().connect(sigc::mem_fun(*this,&studio::Widget_Defaults::queue_draw));

	if(App::dialog_gradient)
	{
		App::dialog_gradient->set_gradient(sinfgapp::Main::get_gradient());
		App::dialog_gradient->reset();
		App::dialog_gradient->signal_edited().connect(sigc::mem_fun(sinfgapp::Main::set_gradient));
	}
	
	if(App::dialog_color)
	{
		App::dialog_color->set_color(sinfgapp::Main::get_foreground_color());
		App::dialog_color->reset();
		App::dialog_color->signal_edited().connect(sigc::mem_fun(sinfgapp::Main::set_foreground_color));
	}
*/
}
	
Widget_Defaults::~Widget_Defaults()
{
}

void
Widget_Defaults::fg_color_refresh()
{
	widget_fg_color->set_value(sinfgapp::Main::get_foreground_color());
}

void
Widget_Defaults::bg_color_refresh()
{
	widget_bg_color->set_value(sinfgapp::Main::get_background_color());
}

void
Widget_Defaults::gradient_refresh()
{
	widget_gradient->set_value(sinfgapp::Main::get_gradient());
}

void
Widget_Defaults::bline_width_refresh()
{
	widget_bline_width->set_value(sinfgapp::Main::get_bline_width());
}

void
Widget_Defaults::blend_method_refresh()
{
	widget_blend_method->set_value(sinfgapp::Main::get_blend_method());
}

void
Widget_Defaults::interpolation_refresh()
{
	widget_interpolation->set_value(sinfgapp::Main::get_interpolation());
}

void
Widget_Defaults::opacity_refresh()
{
	widget_opacity->set_value(sinfgapp::Main::get_opacity());
}

void
Widget_Defaults::on_opacity_changed()
{
	sinfgapp::Main::set_opacity(widget_opacity->get_value());
}

void
Widget_Defaults::on_blend_method_changed()
{
	sinfgapp::Main::set_blend_method(Color::BlendMethod(widget_blend_method->get_value()));
}

void
Widget_Defaults::on_interpolation_changed()
{
	sinfgapp::Main::set_interpolation(Waypoint::Interpolation(widget_interpolation->get_value()));
}

void
Widget_Defaults::on_bline_width_changed()
{
	sinfgapp::Main::set_bline_width(widget_bline_width->get_value());
}

void
Widget_Defaults::on_fg_color_clicked()
{
	// Left click on foreground
	App::dialog_color->set_color(sinfgapp::Main::get_foreground_color());
	App::dialog_color->reset();
	App::dialog_color->signal_edited().connect(sigc::ptr_fun(sinfgapp::Main::set_foreground_color));
	App::dialog_color->present();
}

void
Widget_Defaults::on_bg_color_clicked()
{
	// Left click on background
	App::dialog_color->set_color(sinfgapp::Main::get_background_color());
	App::dialog_color->reset();
	App::dialog_color->signal_edited().connect(sigc::ptr_fun(sinfgapp::Main::set_background_color));
	App::dialog_color->present();
}

void
Widget_Defaults::on_swap_color_clicked()
{
	sinfgapp::Main::color_swap();
}

void
Widget_Defaults::on_reset_color_clicked()
{
	sinfgapp::Main::set_background_color(Color::white());
	sinfgapp::Main::set_foreground_color(Color::black());
}

void
Widget_Defaults::on_gradient_clicked()
{
	App::dialog_gradient->set_gradient(sinfgapp::Main::get_gradient());
	App::dialog_gradient->reset();
	App::dialog_gradient->signal_edited().connect(sigc::ptr_fun(sinfgapp::Main::set_gradient));
	App::dialog_gradient->present();
}

/*
bool
Widget_Defaults::redraw(GdkEventExpose*bleh)
{
	Glib::RefPtr<Gdk::GC> gc(Gdk::GC::create(get_window()));

	const int h(get_height());
	const int w(get_width());	
	const int size=std::min(h-GRADIENT_HEIGHT,w);
	
	render_color_to_window(get_window(),Gdk::Rectangle(size/4,size/4,size/4*3-1,size/4*3-1),sinfgapp::Main::get_background_color());
	render_color_to_window(get_window(),Gdk::Rectangle(0,0,size/4*3-1,size/4*3-1),sinfgapp::Main::get_foreground_color());
	render_gradient_to_window(get_window(),Gdk::Rectangle(0,h-GRADIENT_HEIGHT,w,GRADIENT_HEIGHT-1),sinfgapp::Main::get_gradient());

	



	Glib::RefPtr<Pango::Layout> layout(Pango::Layout::create(get_pango_context()));
	
	gc->set_rgb_fg_color(Gdk::Color("#FF0000"));
	layout->set_text(sinfgapp::Main::get_bline_width().get_string(2));		
	layout->set_alignment(Pango::ALIGN_CENTER);
	layout->set_width(w/2);
	get_window()->draw_layout(gc, w*3/4, (h-GRADIENT_HEIGHT)-16, layout);

	return true;
}

bool
Widget_Defaults::on_event(GdkEvent *event)
{
	const int x(static_cast<int>(event->button.x));
	const int y(static_cast<int>(event->button.y));

	const int h(get_height());
	const int w(get_width());	
	const int size=std::min(h-GRADIENT_HEIGHT,w);

	switch(event->type)
	{
	case GDK_MOTION_NOTIFY:
		break;
	case GDK_BUTTON_PRESS:
//			if(event->button.button==1 && y>get_height()-CONTROL_HEIGHT)
		break;
	case GDK_BUTTON_RELEASE:
		if(event->button.button==1)
		{
			if(y>size)
			{
				// Left click on gradient
				App::dialog_gradient->set_gradient(sinfgapp::Main::get_gradient());
				App::dialog_gradient->reset();
				App::dialog_gradient->signal_edited().connect(sigc::mem_fun(sinfgapp::Main::set_gradient));
				App::dialog_gradient->present();
				return true;
			}
			if(x>0 && x<=size)
			{
				if(x<size*3/4 && y<size*3/4)
				{
					// Left click on foreground
					App::dialog_color->set_color(sinfgapp::Main::get_foreground_color());
					App::dialog_color->reset();
					App::dialog_color->signal_edited().connect(sigc::mem_fun(sinfgapp::Main::set_foreground_color));
					App::dialog_color->present();
					return true;
				}
				if(x>size*3/4 && y>size/4)
				{
					// Left click on background
					App::dialog_color->set_color(sinfgapp::Main::get_background_color());
					App::dialog_color->reset();
					App::dialog_color->signal_edited().connect(sigc::mem_fun(sinfgapp::Main::set_background_color));
					App::dialog_color->present();
					return true;
				}
			}
			if(x>size) // Left click on BLine Width
			{
				Distance dist(sinfgapp::Main::get_bline_width());
				
				if(y<size/2) // increase BLine size
				{
					dist+=DEFAULT_INCREMENT;
				}
				else // Decrease BLine size
				{
					dist-=DEFAULT_INCREMENT;
				}
				sinfgapp::Main::set_bline_width(dist);
			}
		}
		if(event->button.button==3)
		{
			if(y>size)
			{
				// right click on gradient
				sinfgapp::Main::set_gradient_default_colors();
				return true;
			}
			else
			{
				if(x<size)
				{
					// right click on colors
					sinfgapp::Main::color_swap();
					return true;
				}
				
				if(x>w/2)
				{
					// right click on bline width
					sinfgapp::Main::set_bline_width(DEFAULT_WIDTH);
				}
				
			}
		}
		break;
	case GDK_SCROLL:
		{
			Distance dist(sinfgapp::Main::get_bline_width());
			
			if(event->scroll.direction==GDK_SCROLL_UP)
			{
				dist+=DEFAULT_INCREMENT;
			}
			else if(event->scroll.direction==GDK_SCROLL_DOWN)
			{
				dist-=DEFAULT_INCREMENT;
			}
			sinfgapp::Main::set_bline_width(dist);
		}
	default:
		break;
	}
	
	return false;
}
*/
