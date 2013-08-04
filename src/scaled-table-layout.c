/*
 * scaled-table-layout: Layouts children in a dynamical table grid
 *                      (rows and columns are inserted and deleted
 *                      automatically depending on the number of
 *                      child actors) and scaled to fit the allocation
 *                      of the actor holding all child actors.
 * 
 * Copyright 2012-2013 Stephan Haller <nomad@froevel.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "scaled-table-layout.h"

#include <glib/gi18n-lib.h>
#include <clutter/clutter.h>
#include <math.h>

/* Define this class in GObject system */
G_DEFINE_TYPE(XfdashboardScaledTableLayout,
				xfdashboard_scaled_table_layout,
				CLUTTER_TYPE_LAYOUT_MANAGER)

/* Private structure - access only by public API if needed */
#define XFDASHBOARD_SCALED_TABLE_LAYOUT_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), XFDASHBOARD_TYPE_SCALED_TABLE_LAYOUT, XfdashboardScaledTableLayoutPrivate))

struct _XfdashboardScaledTableLayoutPrivate
{
	/* Properties related */
	gfloat		rowSpacing;
	gfloat		columnSpacing;
	gboolean	relativeScale;

	/* Instance related */
	gint		rows;
	gint		columns;
};

/* Properties */
enum
{
	PROP_0,

	PROP_ROW_SPACING,
	PROP_COLUMN_SPACING,
	PROP_RELATIVE_SCALE,

	PROP_LAST
};

GParamSpec* XfdashboardScaledTableLayoutProperties[PROP_LAST]={ 0, };

/* IMPLEMENTATION: Private variables and methods */

/* Updates the minimum number of rows and columns needed for layout */
void _xfdashboard_scaled_table_layout_update_rows_and_columns(XfdashboardScaledTableLayout *self,
																ClutterContainer *inContainer)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(CLUTTER_IS_CONTAINER(inContainer));
	g_return_if_fail(CLUTTER_IS_ACTOR(inContainer));

	XfdashboardScaledTableLayoutPrivate		*priv=self->priv;
	ClutterActor							*child;
	ClutterActorIter						iter;
	ClutterRequestMode						requestMode;
	gint									numberChildren;

	/* Get number of visible child actors */
	numberChildren=0;
	clutter_actor_iter_init(&iter, CLUTTER_ACTOR(inContainer));
	while(clutter_actor_iter_next(&iter, &child))
	{
		if(CLUTTER_ACTOR_IS_VISIBLE(child)) numberChildren++;
	}

	/* Get request mode to determine if more rows than colums are needed
	 * or the opposite
	 */
	requestMode=clutter_actor_get_request_mode(CLUTTER_ACTOR(inContainer));

	/* Calculate and update number of rows and columns */
	if(requestMode==CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
	{
		priv->rows=ceil(sqrt((double)numberChildren));
		priv->columns=ceil((double)numberChildren / (double)priv->rows);
	}
		else
		{
			priv->columns=ceil(sqrt((double)numberChildren));
			priv->rows=ceil((double)numberChildren / (double)priv->columns);
		}
}

/* IMPLEMENTATION: ClutterLayoutManager */

/* Get preferred width/height */
void _xfdashboard_scaled_table_layout_get_preferred_width(ClutterLayoutManager *self,
															ClutterContainer *inContainer,
															gfloat inForHeight,
															gfloat *outMinWidth,
															gfloat *outNaturalWidth)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(CLUTTER_IS_CONTAINER(inContainer));

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;
	gfloat									maxMinWidth, maxNaturalWidth;

	/* Set up default values */
	maxMinWidth=0.0f;
	maxNaturalWidth=0.0f;

	/* Update number of rows and columns needed for layout */
	_xfdashboard_scaled_table_layout_update_rows_and_columns(XFDASHBOARD_SCALED_TABLE_LAYOUT(self), inContainer);

	/* Calculate width */
	if(priv->columns>0)
	{
		maxMinWidth+=(priv->columns-1)*priv->columnSpacing;
		maxNaturalWidth+=(priv->columns-1)*priv->columnSpacing;
	}

	/* Set return values */
	if(outMinWidth) *outMinWidth=maxMinWidth;
	if(outNaturalWidth) *outNaturalWidth=maxNaturalWidth;
}

void _xfdashboard_scaled_table_layout_get_preferred_height(ClutterLayoutManager *self,
															ClutterContainer *inContainer,
															gfloat inForWidth,
															gfloat *outMinHeight,
															gfloat *outNaturalHeight)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(CLUTTER_IS_CONTAINER(inContainer));

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;
	gfloat									maxMinHeight, maxNaturalHeight;

	/* Set up default values */
	maxMinHeight=0.0f;
	maxNaturalHeight=0.0f;

	/* Update number of rows and columns needed for layout */
	_xfdashboard_scaled_table_layout_update_rows_and_columns(XFDASHBOARD_SCALED_TABLE_LAYOUT(self), inContainer);

	/* Calculate height */
	if(priv->rows>0)
	{
		maxMinHeight+=(priv->rows-1)*priv->rowSpacing;
		maxNaturalHeight+=(priv->rows-1)*priv->rowSpacing;
	}

	/* Set return values */
	if(outMinHeight) *outMinHeight=maxMinHeight;
	if(outNaturalHeight) *outNaturalHeight=maxNaturalHeight;
}

/* Re-layout and allocate children of container we manage */
void _xfdashboard_scaled_table_layout_allocate(ClutterLayoutManager *self,
												ClutterContainer *inContainer,
												const ClutterActorBox *inAllocation,
												ClutterAllocationFlags inFlags)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(CLUTTER_IS_CONTAINER(inContainer));

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;
	gint									row, col;
	ClutterActor							*child;
	ClutterActorIter						iter;
	gfloat									cellWidth, cellHeight;
	gfloat									childWidth, childHeight;
	gfloat									largestWidth, largestHeight;
	gfloat									scaleWidth, scaleHeight;
	gfloat									aspectRatio;
	gfloat									x, y;
	ClutterActorBox							childAllocation;

	/* Get size of container holding children to layout and
	 * determine size of a cell
	 */
	clutter_actor_get_size(CLUTTER_ACTOR(inContainer), &childWidth, &childHeight);

	cellWidth=childWidth-((priv->columns-1)*priv->columnSpacing);
	cellWidth=floor(cellWidth/priv->columns);

	cellHeight=childHeight-((priv->rows-1)*priv->rowSpacing);
	cellHeight=floor(cellHeight/priv->rows);

	/* Iterate through children and find largest one
	 * if relative scale was set
	 */
	if(priv->relativeScale==TRUE)
	{
		gfloat								w, h;

		largestWidth=largestHeight=0.0f;
		clutter_actor_iter_init(&iter, CLUTTER_ACTOR(inContainer));
		while(clutter_actor_iter_next(&iter, &child))
		{
			if(!CLUTTER_ACTOR_IS_VISIBLE(child)) continue;

			clutter_actor_get_preferred_size(child, NULL, NULL, &w, &h);
			if(w>largestWidth) largestWidth=w;
			if(h>largestHeight) largestHeight=h;
		}
	}

	/* Iterate through child actors and set their new allocation */
	row=col=0;
	x=y=0.0f;
	clutter_actor_iter_init(&iter, CLUTTER_ACTOR(inContainer));
	while(clutter_actor_iter_next(&iter, &child))
	{
		if(!CLUTTER_ACTOR_IS_VISIBLE(child)) continue;

		/* Get natural size of actor */
		clutter_actor_get_preferred_size(child, NULL, NULL, &childWidth, &childHeight);

		/* If either width or height is 0 then it is visually hidden and we
		 * skip expensive calculation. This also has the nice effect that
		 * do not perform invalid divisions by zero ;)
		 */
		if(childWidth>0.0f && childHeight>0.0f)
		{
			/* Get scale factor needed to apply to width and height.
			 * If no relative scaling should be performed the scale is always 1.0
			 * otherwise it is the scale factor for this actor to the largest one.
			 */
			if(priv->relativeScale==TRUE)
			{
				/* Get scale factors */
				scaleWidth=childWidth/largestWidth;
				scaleHeight=childHeight/largestHeight;
			}
				else scaleWidth=scaleHeight=1.0f;

			/* Get aspect ratio factor */
			aspectRatio=childHeight/childWidth;

			/* Calculate new size of child */
			childWidth=cellWidth*scaleWidth;
			childHeight=childWidth*aspectRatio;
			if(childHeight>cellHeight)
			{
				childHeight=cellHeight*scaleHeight;
				childWidth=cellHeight/aspectRatio;
			}
		}
			else
			{
				/* Visually hidden so do not allocate any space */
				childWidth=0.0f;
				childHeight=0.0f;
			}

		/* Set new allocation of child */
		childAllocation.x1=ceil(x+((cellWidth-childWidth)/2.0f));
		childAllocation.y1=ceil(y+((cellHeight-childHeight)/2.0f));
		childAllocation.x2=ceil(childAllocation.x1+childWidth);
		childAllocation.y2=ceil(childAllocation.y1+childHeight);
		clutter_actor_allocate(child, &childAllocation, inFlags);

		/* Set up for next child */
		col=(col+1) % priv->columns;
		if(col==0) row++;
		x=col*(cellWidth+priv->columnSpacing);
		y=row*(cellHeight+priv->rowSpacing);
	}
}

/* IMPLEMENTATION: GObject */

/* Set/get properties */
void _xfdashboard_scaled_table_layout_set_property(GObject *inObject,
														guint inPropID,
														const GValue *inValue,
														GParamSpec *inSpec)
{
	XfdashboardScaledTableLayout			*self=XFDASHBOARD_SCALED_TABLE_LAYOUT(inObject);
	
	switch(inPropID)
	{
		case PROP_ROW_SPACING:
			xfdashboard_scaled_table_layout_set_row_spacing(self, g_value_get_float(inValue));
			break;

		case PROP_COLUMN_SPACING:
			xfdashboard_scaled_table_layout_set_column_spacing(self, g_value_get_float(inValue));
			break;

		case PROP_RELATIVE_SCALE:
			xfdashboard_scaled_table_layout_set_relative_scale(self, g_value_get_boolean(inValue));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(inObject, inPropID, inSpec);
			break;
	}
}

void _xfdashboard_scaled_table_layout_get_property(GObject *inObject,
														guint inPropID,
														GValue *outValue,
														GParamSpec *inSpec)
{
	XfdashboardScaledTableLayout	*self=XFDASHBOARD_SCALED_TABLE_LAYOUT(inObject);

	switch(inPropID)
	{
		case PROP_ROW_SPACING:
			g_value_set_float(outValue, self->priv->rowSpacing);
			break;

		case PROP_COLUMN_SPACING:
			g_value_set_float(outValue, self->priv->columnSpacing);
			break;

		case PROP_RELATIVE_SCALE:
			g_value_set_boolean(outValue, self->priv->relativeScale);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(inObject, inPropID, inSpec);
			break;
	}
}

/* Class initialization
 * Override functions in parent classes and define properties
 * and signals
 */
void xfdashboard_scaled_table_layout_class_init(XfdashboardScaledTableLayoutClass *klass)
{
	ClutterLayoutManagerClass	*layoutClass=CLUTTER_LAYOUT_MANAGER_CLASS(klass);
	GObjectClass				*gobjectClass=G_OBJECT_CLASS(klass);

	/* Override functions */
	layoutClass->get_preferred_width=_xfdashboard_scaled_table_layout_get_preferred_width;
	layoutClass->get_preferred_height=_xfdashboard_scaled_table_layout_get_preferred_height;
	layoutClass->allocate=_xfdashboard_scaled_table_layout_allocate;

	gobjectClass->set_property=_xfdashboard_scaled_table_layout_set_property;
	gobjectClass->get_property=_xfdashboard_scaled_table_layout_get_property;

	/* Set up private structure */
	g_type_class_add_private(klass, sizeof(XfdashboardScaledTableLayoutPrivate));

	/* Define properties */
	XfdashboardScaledTableLayoutProperties[PROP_ROW_SPACING]=
		g_param_spec_float("row-spacing",
								_("Row spacing"),
								_("The spacing between rows in table"),
								0.0f,
								G_MAXFLOAT,
								0.0f,
								G_PARAM_READWRITE);

	XfdashboardScaledTableLayoutProperties[PROP_COLUMN_SPACING]=
		g_param_spec_float("column-spacing",
								_("Column spacing"),
								_("The spacing between columns in table"),
								0.0f,
								G_MAXFLOAT,
								0.0f,
								G_PARAM_READWRITE);

	XfdashboardScaledTableLayoutProperties[PROP_RELATIVE_SCALE]=
		g_param_spec_boolean("relative-scale",
								_("Relative scale"),
								_("Whether all children should be scaled relatively to largest child"),
								FALSE,
								G_PARAM_READWRITE);

	g_object_class_install_properties(gobjectClass, PROP_LAST, XfdashboardScaledTableLayoutProperties);
}

/* Object initialization
 * Create private structure and set up default values
 */
void xfdashboard_scaled_table_layout_init(XfdashboardScaledTableLayout *self)
{
	XfdashboardScaledTableLayoutPrivate	*priv;

	priv=self->priv=XFDASHBOARD_SCALED_TABLE_LAYOUT_GET_PRIVATE(self);

	/* Set default values */
	priv->rowSpacing=0.0f;
	priv->columnSpacing=0.0f;
	priv->relativeScale=FALSE;

	priv->rows=0;
	priv->columns=0;
}

/* Implementation: Public API */

ClutterLayoutManager* xfdashboard_scaled_table_layout_new()
{
	return(g_object_new(XFDASHBOARD_TYPE_SCALED_TABLE_LAYOUT, NULL));
}

/* Get/set relative scaling of all children to largest one */
gboolean xfdashboard_scaled_table_layout_get_relative_scale(XfdashboardScaledTableLayout *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self), FALSE);

	return(self->priv->relativeScale);
}

void xfdashboard_scaled_table_layout_set_relative_scale(XfdashboardScaledTableLayout *self, gboolean inScaling)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;

	/* Set new value if changed */
	if(priv->relativeScale!=inScaling)
	{
		/* Set new value and notify about property change */
		priv->relativeScale=inScaling;
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardScaledTableLayoutProperties[PROP_RELATIVE_SCALE]);

		/* Notify for upcoming layout changes */
		clutter_layout_manager_layout_changed(CLUTTER_LAYOUT_MANAGER(self));
	}
}

/* Set relative row and column spacing to same value at once */
void xfdashboard_scaled_table_layout_set_spacing(XfdashboardScaledTableLayout *self, gfloat inSpacing)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(inSpacing>=0.0f);

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;

	/* Set new values if changed */
	if(priv->rowSpacing!=inSpacing || priv->columnSpacing!=inSpacing)
	{
		/* Set new values and notify about properties changes */
		priv->rowSpacing=inSpacing;
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardScaledTableLayoutProperties[PROP_ROW_SPACING]);

		priv->columnSpacing=inSpacing;
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardScaledTableLayoutProperties[PROP_COLUMN_SPACING]);

		/* Notify for upcoming layout changes */
		clutter_layout_manager_layout_changed(CLUTTER_LAYOUT_MANAGER(self));
	}
}

/* Get/set row spacing */
gfloat xfdashboard_scaled_table_layout_get_row_spacing(XfdashboardScaledTableLayout *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self), 0.0f);

	return(self->priv->rowSpacing);
}

void xfdashboard_scaled_table_layout_set_row_spacing(XfdashboardScaledTableLayout *self, gfloat inSpacing)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(inSpacing>=0.0f);

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;

	/* Set new value if changed */
	if(priv->rowSpacing!=inSpacing)
	{
		/* Set new value and notify about property change */
		priv->rowSpacing=inSpacing;
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardScaledTableLayoutProperties[PROP_ROW_SPACING]);

		/* Notify for upcoming layout changes */
		clutter_layout_manager_layout_changed(CLUTTER_LAYOUT_MANAGER(self));
	}
}

/* Get/set columns spacing */
gfloat xfdashboard_scaled_table_layout_get_column_spacing(XfdashboardScaledTableLayout *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self), 0.0f);

	return(self->priv->columnSpacing);
}

void xfdashboard_scaled_table_layout_set_column_spacing(XfdashboardScaledTableLayout *self, gfloat inSpacing)
{
	g_return_if_fail(XFDASHBOARD_IS_SCALED_TABLE_LAYOUT(self));
	g_return_if_fail(inSpacing>=0.0f);

	XfdashboardScaledTableLayoutPrivate		*priv=XFDASHBOARD_SCALED_TABLE_LAYOUT(self)->priv;

	/* Set new value if changed */
	if(priv->columnSpacing!=inSpacing)
	{
		/* Set new value and notify about property change */
		priv->columnSpacing=inSpacing;
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardScaledTableLayoutProperties[PROP_COLUMN_SPACING]);

		/* Notify for upcoming layout changes */
		clutter_layout_manager_layout_changed(CLUTTER_LAYOUT_MANAGER(self));
	}
}
