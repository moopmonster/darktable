/*
    This file is part of darktable,
    copyright (c)2010 Henrik Andersson.

    darktable is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    darktable is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with darktable.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "label.h"

static void _label_class_init(GtkDarktableLabelClass *klass);
static void _label_init(GtkDarktableLabel *slider);
static void _label_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void _label_get_preferred_width (GtkWidget *widget, gint *minimal_width, gint *natural_width);
static void _label_get_preferred_height (GtkWidget *widget, gint *minimal_height, gint *natural_height);
static gboolean _label_draw(GtkWidget *widget, cairo_t *cr);

static void _label_class_init (GtkDarktableLabelClass *klass)
{
  GtkWidgetClass *widget_class=(GtkWidgetClass *) klass;

  widget_class->get_preferred_width = _label_get_preferred_width;
  widget_class->get_preferred_height = _label_get_preferred_height;
  widget_class->draw = _label_draw;
}

static void _label_init(GtkDarktableLabel *label)
{
}

static void  _label_size_request(GtkWidget *widget,GtkRequisition *requisition)
{
  g_return_if_fail(widget != NULL);
  g_return_if_fail(DTGTK_IS_LABEL(widget));
  g_return_if_fail(requisition != NULL);
  requisition->width = -1;
  requisition->height = 17;
}

static void
_label_get_preferred_width (GtkWidget *widget,
                            gint *minimal_width,
                            gint *natural_width)
{
  GtkRequisition requisition;

  _label_size_request (widget, &requisition);

  *minimal_width = *natural_width = requisition.width;
}

static void
_label_get_preferred_height (GtkWidget *widget,
                             gint *minimal_height,
                             gint *natural_height)
{
  GtkRequisition requisition;

  _label_size_request (widget, &requisition);

  *minimal_height = *natural_height = requisition.height;
}

static gboolean _label_draw(GtkWidget *widget, cairo_t *cr)
{
  g_return_val_if_fail(widget != NULL, FALSE);
  g_return_val_if_fail(DTGTK_IS_LABEL(widget), FALSE);

  GtkStyle *style=gtk_rc_get_style_by_paths(gtk_settings_get_default(), NULL,"GtkButton", GTK_TYPE_BUTTON);
  if(!style) style = gtk_rc_get_style(widget);
  int state = gtk_widget_get_state(widget);

  GtkAllocation allocation;
  gtk_widget_get_allocation(widget, &allocation);
  int x = allocation.x;
  int y = allocation.y;
  int width = allocation.width;
  int height = allocation.height;

  // Formatting the display of text and draw it...
  PangoLayout *layout;
  layout = gtk_widget_create_pango_layout(widget,NULL);
  pango_layout_set_font_description(layout,style->font_desc);
  const gchar *text=gtk_label_get_text(GTK_LABEL(widget));
  pango_layout_set_text(layout,text,-1);
  GdkRectangle t= {x,y,x+width,y+height};
  int pw,ph;
  pango_layout_get_pixel_size(layout,&pw,&ph);


  // Begin cairo drawing
  cairo_set_source_rgba(cr,
                        /* style->fg[state].red/65535.0,
                         style->fg[state].green/65535.0,
                         style->fg[state].blue/65535.0,*/
                        1,1,1,
                        0.10
                       );

  cairo_set_antialias(cr,CAIRO_ANTIALIAS_NONE);

  cairo_set_line_width(cr,1.0);
  cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
  if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_UNDERLINED )
  {
    cairo_move_to(cr,x,y+height-2);
    cairo_line_to(cr,x+width,y+height-2);
    cairo_stroke(cr);
  }
  else if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_BACKFILLED )
  {
    cairo_rectangle(cr,x,y,width,height);
    cairo_fill(cr);
  }
  else if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_TAB )
  {
    int rx=x,rw=pw+2;
    if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_ALIGN_RIGHT ) rx=x+width-pw-8;
    cairo_rectangle(cr,rx,y,rw+4,height-1);
    cairo_fill(cr);

    if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_ALIGN_RIGHT )
    {
      // /|
      cairo_move_to(cr,x+width-rw-6,y);
      cairo_line_to(cr,x+width-rw-6-15,y+height-2);
      cairo_line_to(cr,x+width-rw-6,y+height-2);
      cairo_fill(cr);

      // hline
      cairo_move_to(cr,x,y+height-1);
      cairo_line_to(cr,x+width-rw-6,y+height-1);
      cairo_stroke(cr);
    }
    else
    {
      // |
      cairo_move_to(cr,x+rw+4,y);
      cairo_line_to(cr,x+rw+4+15,y+height-2);
      cairo_line_to(cr,x+rw+4,y+height-2);
      cairo_fill(cr);

      // hline
      cairo_move_to(cr,x+rw+4,y+height-1);
      cairo_line_to(cr,x+width,y+height-1);
      cairo_stroke(cr);
    }
  }
  cairo_set_antialias(cr,CAIRO_ANTIALIAS_DEFAULT);

  // draw text
  int lx=x+4, ly=y+((height/2.0)-(ph/2.0));
  if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_ALIGN_RIGHT ) lx=x+width-pw-6;
  else if( DTGTK_LABEL(widget)->flags&DARKTABLE_LABEL_ALIGN_CENTER ) lx=(width/2.0)-(pw/2.0);
  gtk_paint_layout(style,gdk_cairo_create(gtk_widget_get_window(widget)),state,TRUE,widget,"label",lx,ly,layout);

  return FALSE;
}

// Public functions
GtkWidget* dtgtk_label_new(const gchar *text, _darktable_label_flags_t flags)
{
  GtkDarktableLabel *label;
  label = g_object_new(dtgtk_label_get_type(), NULL);
  gtk_label_set_text(GTK_LABEL(label),text);
  label->flags=flags;
  return (GtkWidget *)label;
}

void dtgtk_label_set_text(GtkDarktableLabel *label,
                          const gchar *text,
                          _darktable_label_flags_t flags)
{
  gtk_label_set_text(GTK_LABEL(label),text);
  label->flags=flags;
  gtk_widget_queue_draw(GTK_WIDGET(label));
}

GType dtgtk_label_get_type()
{
  static GType dtgtk_label_type = 0;
  if (!dtgtk_label_type)
  {
    static const GTypeInfo dtgtk_label_info =
    {
      sizeof(GtkDarktableLabelClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) _label_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkDarktableLabel),
      0,              /* n_preallocs */
      (GInstanceInitFunc) _label_init,
    };
    dtgtk_label_type = g_type_register_static(GTK_TYPE_LABEL, "GtkDarktableLabel", &dtgtk_label_info, 0);
  }
  return dtgtk_label_type;
}
// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;
