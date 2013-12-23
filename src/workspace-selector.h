/*
 * workspace-selector: Workspace selector box
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

#ifndef __XFDASHBOARD_WORKSPACE_SELECTOR__
#define __XFDASHBOARD_WORKSPACE_SELECTOR__

#include "background.h"

G_BEGIN_DECLS

#define XFDASHBOARD_TYPE_WORKSPACE_SELECTOR				(xfdashboard_workspace_selector_get_type())
#define XFDASHBOARD_WORKSPACE_SELECTOR(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), XFDASHBOARD_TYPE_WORKSPACE_SELECTOR, XfdashboardWorkspaceSelector))
#define XFDASHBOARD_IS_WORKSPACE_SELECTOR(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), XFDASHBOARD_TYPE_WORKSPACE_SELECTOR))
#define XFDASHBOARD_WORKSPACE_SELECTOR_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), XFDASHBOARD_TYPE_WORKSPACE_SELECTOR, XfdashboardWorkspaceSelectorClass))
#define XFDASHBOARD_IS_WORKSPACE_SELECTOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), XFDASHBOARD_TYPE_WORKSPACE_SELECTOR))
#define XFDASHBOARD_WORKSPACE_SELECTOR_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), XFDASHBOARD_TYPE_WORKSPACE_SELECTOR, XfdashboardWorkspaceSelectorClass))

typedef struct _XfdashboardWorkspaceSelector			XfdashboardWorkspaceSelector;
typedef struct _XfdashboardWorkspaceSelectorClass		XfdashboardWorkspaceSelectorClass;
typedef struct _XfdashboardWorkspaceSelectorPrivate		XfdashboardWorkspaceSelectorPrivate;

struct _XfdashboardWorkspaceSelector
{
	/* Parent instance */
	XfdashboardBackground				parent_instance;

	/* Private structure */
	XfdashboardWorkspaceSelectorPrivate	*priv;
};

struct _XfdashboardWorkspaceSelectorClass
{
	/*< private >*/
	/* Parent class */
	XfdashboardBackgroundClass			parent_class;

	/*< public >*/
	/* Virtual functions */
};

/* Public API */
GType xfdashboard_workspace_selector_get_type(void) G_GNUC_CONST;

ClutterActor* xfdashboard_workspace_selector_new(void);
ClutterActor* xfdashboard_workspace_selector_new_with_orientation(ClutterOrientation inOrientation);

gfloat xfdashboard_workspace_selector_get_spacing(XfdashboardWorkspaceSelector *self);
void xfdashboard_workspace_selector_set_spacing(XfdashboardWorkspaceSelector *self, const gfloat inSpacing);

ClutterOrientation xfdashboard_workspace_selector_get_orientation(XfdashboardWorkspaceSelector *self);
void xfdashboard_workspace_selector_set_orientation(XfdashboardWorkspaceSelector *self, ClutterOrientation inOrientation);

G_END_DECLS

#endif	/* __XFDASHBOARD_WORKSPACE_SELECTOR__ */
