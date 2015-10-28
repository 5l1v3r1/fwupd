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

#include "config.h"

#include <glib-object.h>
#include <gio/gio.h>

#include "fu-device.h"

static void fu_device_finalize			 (GObject *object);

/**
 * FuDevicePrivate:
 *
 * Private #FuDevice data
 **/
typedef struct {
	gchar				*id;
	guint64				 flags;
	GHashTable			*metadata;
} FuDevicePrivate;

enum {
	SIGNAL_LAST
};

enum {
	PROP_0,
	PROP_ID,
	PROP_LAST
};

G_DEFINE_TYPE_WITH_PRIVATE (FuDevice, fu_device, G_TYPE_OBJECT)
#define GET_PRIVATE(o) (fu_device_get_instance_private (o))

/**
 * fu_device_get_id:
 **/
const gchar *
fu_device_get_id (FuDevice *device)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_val_if_fail (FU_IS_DEVICE (device), NULL);
	return priv->id;
}

/**
 * fu_device_set_id:
 **/
void
fu_device_set_id (FuDevice *device, const gchar *id)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_if_fail (FU_IS_DEVICE (device));
	g_free (priv->id);
	priv->id = g_strdup (id);
}

/**
 * fu_device_get_flags:
 **/
guint64
fu_device_get_flags (FuDevice *device)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_val_if_fail (FU_IS_DEVICE (device), 0);
	return priv->flags;
}

/**
 * fu_device_set_flags:
 **/
void
fu_device_set_flags (FuDevice *device, guint64 flags)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_if_fail (FU_IS_DEVICE (device));
	priv->flags = flags;
}

/**
 * fu_device_add_flag:
 **/
void
fu_device_add_flag (FuDevice *device, FwupdDeviceFlags flag)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_if_fail (FU_IS_DEVICE (device));
	priv->flags |= flag;
}

/**
 * fu_device_get_guid:
 **/
const gchar *
fu_device_get_guid (FuDevice *device)
{
	return fu_device_get_metadata (device, FU_DEVICE_KEY_GUID);
}

/**
 * fu_device_set_guid:
 **/
void
fu_device_set_guid (FuDevice *device, const gchar *guid)
{
	fu_device_set_metadata (device, FU_DEVICE_KEY_GUID, guid);
}

/**
 * fu_device_get_display_name:
 **/
const gchar *
fu_device_get_display_name (FuDevice *device)
{
	const gchar *tmp;
	tmp = fu_device_get_metadata (device, FU_DEVICE_KEY_DISPLAY_NAME);
	if (tmp == NULL)
		tmp = fu_device_get_id (device);
	return tmp;
}

/**
 * fu_device_set_display_name:
 **/
void
fu_device_set_display_name (FuDevice *device, const gchar *display_name)
{
	fu_device_set_metadata (device, FU_DEVICE_KEY_DISPLAY_NAME, display_name);
}

/**
 * fu_device_get_metadata:
 **/
const gchar *
fu_device_get_metadata (FuDevice *device, const gchar *key)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_val_if_fail (FU_IS_DEVICE (device), NULL);
	g_return_val_if_fail (key != NULL, NULL);
	return g_hash_table_lookup (priv->metadata, key);
}

/**
 * fu_device_get_id:
 **/
void
fu_device_set_metadata (FuDevice *device, const gchar *key, const gchar *value)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_if_fail (FU_IS_DEVICE (device));
	g_return_if_fail (key != NULL);
	g_return_if_fail (value != NULL);
	g_hash_table_insert (priv->metadata, g_strdup (key), g_strdup (value));
}

/**
 * fu_device_to_variant:
 **/
GVariant *
fu_device_to_variant (FuDevice *device)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	GList *l;
	GVariantBuilder builder;
	const gchar *key;
	const gchar *value;
	g_autoptr(GList) keys = NULL;

	/* create an array with all the metadata in */
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
	keys = g_hash_table_get_keys (priv->metadata);
	for (l = keys; l != NULL; l = l->next) {
		key = l->data;
		value = g_hash_table_lookup (priv->metadata, key);
		if (g_strcmp0 (value, "TRUE") == 0) {
			g_variant_builder_add (&builder, "{sv}",
					       key, g_variant_new_boolean (TRUE));
		} else {
			g_variant_builder_add (&builder, "{sv}",
					       key, g_variant_new_string (value));
		}
	}
	g_variant_builder_add (&builder, "{sv}",
			       FU_DEVICE_KEY_FLAGS,
			       g_variant_new_uint64 (priv->flags));
	return g_variant_new ("{sa{sv}}", priv->id, &builder);
}

/**
 * fu_device_get_metadata_as_variant:
 **/
GVariant *
fu_device_get_metadata_as_variant (FuDevice *device)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	GList *l;
	GVariantBuilder builder;
	const gchar *key;
	const gchar *value;
	g_autoptr(GList) keys = NULL;

	/* create an array with all the metadata in */
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
	keys = g_hash_table_get_keys (priv->metadata);
	keys = g_list_sort (keys, (GCompareFunc) g_strcmp0);
	for (l = keys; l != NULL; l = l->next) {
		key = l->data;
		value = g_hash_table_lookup (priv->metadata, key);
		if (g_strcmp0 (value, "TRUE") == 0) {
			g_variant_builder_add (&builder, "{sv}",
					       key, g_variant_new_boolean (TRUE));
		} else {
			g_variant_builder_add (&builder, "{sv}",
					       key, g_variant_new_string (value));
		}
	}
	g_variant_builder_add (&builder, "{sv}",
			       FU_DEVICE_KEY_FLAGS,
			       g_variant_new_uint64 (priv->flags));
	return g_variant_new ("(a{sv})", &builder);
}

/**
 * fu_device_set_metadata_from_iter:
 **/
void
fu_device_set_metadata_from_iter (FuDevice *device, GVariantIter *iter)
{
	GVariant *variant;
	const gchar *key;
	const gchar *type;

	while (g_variant_iter_next (iter, "{&sv}", &key, &variant)) {
		if (g_strcmp0 (key, FU_DEVICE_KEY_FLAGS) == 0) {
			fu_device_set_flags (device, g_variant_get_uint64 (variant));
			continue;
		}
		type = g_variant_get_type_string (variant);
		if (g_strcmp0 (type, "s") == 0) {
			fu_device_set_metadata (device, key,
						g_variant_get_string (variant, NULL));
		} else if (g_strcmp0 (type, "b") == 0) {
			fu_device_set_metadata (device, key, "TRUE");
		} else if (g_strcmp0 (type, "t") == 0) {
			g_autofree gchar *tmp = NULL;
			tmp = g_strdup_printf ("%" G_GUINT64_FORMAT,
					       g_variant_get_uint64 (variant));
			fu_device_set_metadata (device, key, tmp);
		} else {
			g_warning ("type=%s", type);
			fu_device_set_metadata (device, key, "???");
		}
		g_variant_unref (variant);
	}
}

/**
 * fu_device_get_property:
 **/
static void
fu_device_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	FuDevice *device = FU_DEVICE (object);
	FuDevicePrivate *priv = GET_PRIVATE (device);

	switch (prop_id) {
	case PROP_ID:
		g_value_set_string (value, priv->id);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/**
 * fu_device_set_property:
 **/
static void
fu_device_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	FuDevice *device = FU_DEVICE (object);
	FuDevicePrivate *priv = GET_PRIVATE (device);

	switch (prop_id) {
	case PROP_ID:
		g_free (priv->id);
		priv->id = g_strdup (g_value_get_string (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/**
 * fu_device_class_init:
 **/
static void
fu_device_class_init (FuDeviceClass *klass)
{
	GParamSpec *pspec;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = fu_device_finalize;
	object_class->get_property = fu_device_get_property;
	object_class->set_property = fu_device_set_property;

	/**
	 * FuDevice:id:
	 */
	pspec = g_param_spec_string ("id", NULL, NULL,
				     NULL,
				     G_PARAM_READWRITE);
	g_object_class_install_property (object_class, PROP_ID, pspec);
}

/**
 * fu_device_init:
 **/
static void
fu_device_init (FuDevice *device)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	priv->metadata = g_hash_table_new_full (g_str_hash, g_str_equal,
						g_free, g_free);
}

/**
 * fu_device_finalize:
 **/
static void
fu_device_finalize (GObject *object)
{
	FuDevice *device = FU_DEVICE (object);
	FuDevicePrivate *priv = GET_PRIVATE (device);

	g_free (priv->id);
	g_hash_table_unref (priv->metadata);

	G_OBJECT_CLASS (fu_device_parent_class)->finalize (object);
}

/**
 * fu_device_new:
 **/
FuDevice *
fu_device_new (void)
{
	FuDevice *device;
	device = g_object_new (FU_TYPE_DEVICE, NULL);
	return FU_DEVICE (device);
}
