<?php
require_once '../../config_base.php';
require_once '../../connection.php';

if (!isset($_GET['file'])) {
    http_response_code(400);
    exit('File not specified');
}

$filename = basename($_GET['file']);
$filepath = __DIR__ . '/../../generated/' . $filename;

if (!file_exists($filepath)) {
    http_response_code(404);
    exit('File not found');
}

header('Content-Type: application/zip');
header('Content-Disposition: attachment; filename="' . $filename . '"');
header('Content-Length: ' . filesize($filepath));
readfile($filepath);
exit;
