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
	gchar				*equivalent_id;
	GHashTable			*metadata;
} FuDevicePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (FuDevice, fu_device, FWUPD_TYPE_RESULT)
#define GET_PRIVATE(o) (fu_device_get_instance_private (o))

/**
 * fu_device_get_equivalent_id:
 **/
const gchar *
fu_device_get_equivalent_id (FuDevice *device)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_val_if_fail (FU_IS_DEVICE (device), NULL);
	return priv->equivalent_id;
}

/**
 * fu_device_set_equivalent_id:
 **/
void
fu_device_set_equivalent_id (FuDevice *device, const gchar *equivalent_id)
{
	FuDevicePrivate *priv = GET_PRIVATE (device);
	g_return_if_fail (FU_IS_DEVICE (device));
	g_free (priv->equivalent_id);
	priv->equivalent_id = g_strdup (equivalent_id);
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
 * fu_device_class_init:
 **/
static void
fu_device_class_init (FuDeviceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = fu_device_finalize;
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
