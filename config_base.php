<?php

$docRoot = rtrim(realpath($_SERVER['DOCUMENT_ROOT']), DIRECTORY_SEPARATOR);
$projectPath = realpath(__DIR__);

$relative = false;
if ($docRoot && $projectPath !== false && strpos($projectPath, $docRoot) === 0) {
    $relative = substr($projectPath, strlen($docRoot));
    $relative = str_replace('\\', '/', $relative);
    $relative = '/' . trim($relative, '/') . '/';
    if ($relative === '//')
        $relative = '/';
}
if ($relative === false) {
    $script = $_SERVER['SCRIPT_NAME'] ?? ($_SERVER['PHP_SELF'] ?? $_SERVER['REQUEST_URI'] ?? '/');
    $base = rtrim(dirname($script), '/');
    if ($base === '' || $base === '\\' || $base === '.') {
        $relative = '/';
    } else {
        $relative = '/' . trim($base, '/') . '/';
    }
}
$relative = str_replace('\\', '/', $relative);
$relative = preg_replace('#^/?[A-Za-z]:#', '', $relative);
$relative = '/' . trim($relative, '/') . '/';
$relative = preg_replace('#/{2,}#', '/', $relative);

if ($relative === '//')
    $relative = '/';
$proto = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') || (isset($_SERVER['SERVER_PORT']) && $_SERVER['SERVER_PORT'] == 443) ? 'https' : 'http';
$host = $_SERVER['HTTP_HOST'] ?? '';

if ($host) {
    $base_host = $proto . '://' . $host . $relative;
    if (!defined('BASE_URL'))
        define('BASE_URL', $base_host);
} else {
    if (!defined('BASE_URL'))
        define('BASE_URL', $relative);
}

if (!function_exists('base_url')) {
    function base_url($path = '')
    {
        if ($path === '')
            return BASE_URL;
        return rtrim(BASE_URL, '/') . '/' . ltrim($path, '/');
    }
}
?>