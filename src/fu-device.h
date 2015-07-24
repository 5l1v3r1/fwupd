/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2015 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU General Public License Version 2
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __FU_DEVICE_H
#define __FU_DEVICE_H

#include <glib-object.h>
#include <fwupd.h>

G_BEGIN_DECLS

#define FU_TYPE_DEVICE		(fu_device_get_type ())
#define FU_DEVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), FU_TYPE_DEVICE, FuDevice))
#define FU_DEVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), FU_TYPE_DEVICE, FuDeviceClass))
#define FU_IS_DEVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), FU_TYPE_DEVICE))
#define FU_IS_DEVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), FU_TYPE_DEVICE))
#define FU_DEVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), FU_TYPE_DEVICE, FuDeviceClass))
#define FU_DEVICE_ERROR		fu_device_error_quark()

#define FU_DEVICE_KEY_VERSION		"Version"	/* s */
#define FU_DEVICE_KEY_PROVIDER		"Provider"	/* s */
#define FU_DEVICE_KEY_GUID		"Guid"		/* s */
#define FU_DEVICE_KEY_FLAGS		"Flags"		/* internal only */
#define FU_DEVICE_KEY_DISPLAY_NAME	"DisplayName"	/* s */
#define FU_DEVICE_KEY_VERSION_NEW	"VersionNew"	/* internal only */
#define FU_DEVICE_KEY_VERSION_OLD	"VersionOld"	/* internal only */
#define FU_DEVICE_KEY_FILENAME_CAB	"FilenameCab"	/* internal only */
#define FU_DEVICE_KEY_VERSION_LOWEST	"VersionLowest"	/* s */
#define FU_DEVICE_KEY_VENDOR		"Vendor"	/* s */
#define FU_DEVICE_KEY_NAME		"Name"		/* s */
#define FU_DEVICE_KEY_SUMMARY		"Summary"	/* s */
#define FU_DEVICE_KEY_DESCRIPTION	"Description"	/* s */
#define FU_DEVICE_KEY_LICENSE		"License"	/* s */
#define FU_DEVICE_KEY_URL_HOMEPAGE	"UrlHomepage"	/* s */
#define FU_DEVICE_KEY_SIZE		"Size"		/* t */
#define FU_DEVICE_KEY_PENDING_STATE	"PendingState"	/* s */
#define FU_DEVICE_KEY_PENDING_ERROR	"PendingError"	/* s */
#define FU_DEVICE_KEY_TRUSTED		"Trusted"	/* t */
#define FU_DEVICE_KEY_FIRMWARE_HASH	"FirmwareHash"	/* s */
#define FU_DEVICE_KEY_UPDATE_VERSION	"UpdateVersion"	/* s */
#define FU_DEVICE_KEY_UPDATE_HASH	"UpdateHash"	/* s */
#define FU_DEVICE_KEY_UPDATE_URI	"UpdateUri"	/* s */
#define FU_DEVICE_KEY_UPDATE_DESCRIPTION "UpdateDescription" /* s */

typedef struct _FuDevicePrivate	FuDevicePrivate;
typedef struct _FuDevice	FuDevice;
typedef struct _FuDeviceClass	FuDeviceClass;

struct _FuDevice
{
	 GObject		 parent;
	 FuDevicePrivate	*priv;
};

struct _FuDeviceClass
{
	GObjectClass		 parent_class;
};

GType		 fu_device_get_type			(void);
FuDevice	*fu_device_new				(void);

/* accessors */
GVariant	*fu_device_to_variant			(FuDevice	*device);
const gchar	*fu_device_get_id			(FuDevice	*device);
void		 fu_device_set_id			(FuDevice	*device,
							 const gchar	*id);
guint64		 fu_device_get_flags			(FuDevice	*device);
void		 fu_device_set_flags			(FuDevice	*device,
							 guint64	 flags);
void		 fu_device_add_flag			(FuDevice	*device,
							 FwupdDeviceFlags flag);
const gchar	*fu_device_get_guid			(FuDevice	*device);
void		 fu_device_set_guid			(FuDevice	*device,
							 const gchar	*guid);
const gchar	*fu_device_get_display_name		(FuDevice	*device);
void		 fu_device_set_display_name		(FuDevice	*device,
							 const gchar	*display_name);
const gchar	*fu_device_get_metadata			(FuDevice	*device,
							 const gchar	*key);
GVariant	*fu_device_get_metadata_as_variant	(FuDevice	*device);
void		 fu_device_set_metadata			(FuDevice	*device,
							 const gchar	*key,
							 const gchar	*value);
void		 fu_device_set_metadata_from_iter	(FuDevice	*device,
							 GVariantIter	*iter);

G_END_DECLS

#endif /* __FU_DEVICE_H */

