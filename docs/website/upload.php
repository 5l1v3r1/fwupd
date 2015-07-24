<?php

/*
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

include 'db.php';

$uploaddir = $_ENV["OPENSHIFT_DATA_DIR"] . '/uploads/';

function lvfs_upload_firmware($db, $auth_token, $uploaddir, $file) {

	$success = True;
	$uri = 'result.php?';

	# check auth key
	if (!lvfs_check_auth($db, $auth_token)) {
		$success = False;
		$uri = $uri . 'authkey=False&';
	}

	# check size < 50Mb
	$size = $file['size'];
	if ($size > 50000000 || $size < 1280) {
		$success = False;
		$uri = $uri . 'sizecheck=False&';
	}

	# check the file is really a cab file
	$data = file_get_contents($file['tmp_name']);
	if (strcmp(substr($data,0,4), "MSCF") != 0) {
		$success = False;
		$uri = $uri . 'filetype=False&';
	}

	# check for metadata
	if (strpos($data, ".metainfo.xml") == FALSE) {
		$success = False;
		$uri = $uri . 'metadata=False&';
	}

	# check the file does not already exist
	$id = sha1($data);
	$result = $db->query('SELECT * FROM firmware WHERE hash = "' . $id . '";');
	if ($result->num_rows > 0) {
		$success = False;
		$uri = $uri . 'exists=False&';
	}

	# only save if we passed all tests
	if ($success == True) {
		$new_filename = $id . '-' . $file['name'];
		$destination = $uploaddir . $new_filename;
		$handle = fopen($destination, "w");
		if ($handle == FALSE) {
			header('HTTP/1.0 403 Forbidden');
			echo 'Write permission for ' . $uploaddir . ' missing';
			return;
		}
		if (fwrite($handle, $data) == FALSE) {
			header('HTTP/1.0 413 Request Entity Too Large');
			echo 'Failed to write ' . $destination;
			return;
		}
		fclose($handle);

		# log to database
		$query = "INSERT INTO firmware (vendor_key, addr, timestamp, filename, hash) " .
			 "VALUES (?, ?, CURRENT_TIMESTAMP, ?, ?);";
		if (!($stmt = $db->prepare($query)))
			die("failed to prepare: " . $db->error);
		$stmt->bind_param("ssss",
				  $auth_token,
				  $_SERVER['REMOTE_ADDR'],
				  $new_filename,
				  $id);
		if (!$stmt->execute())
			die("failed to execute: " . $stmt->error);
		$stmt->close();
	}

	return $uri . 'result=' . $success;
}

# connect to database and upload firmware
$db = lvfs_connect_db();
$location = lvfs_upload_firmware($db, $_POST['auth'], $uploaddir, $_FILES['file']);
lvfs_disconnect_db($db);

header('Location: ' . $location);

?>
