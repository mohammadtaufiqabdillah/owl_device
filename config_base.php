<?php
$base = rtrim(dirname($_SERVER['SCRIPT_NAME']), '/');
if ($base === '')
    $base = '/';
else
    $base .= '/';

$proto = (!empty($_SERVER['HTTPS']) && $_SERVER['HTTPS'] !== 'off') || (isset($_SERVER['SERVER_PORT']) && $_SERVER['SERVER_PORT'] == 443) ? 'https' : 'http';
$host = $_SERVER['HTTP_HOST'] ?? '';
if ($host) {
    $base_host = $proto . '://' . $host . $base;
    if (!defined('BASE_URL'))
        define('BASE_URL', $base_host);
} else {
    if (!defined('BASE_URL'))
        define('BASE_URL', $base);
}

if (!function_exists('base_url')) {
    function base_url($path = '')
    {
        global $base;
        if ($path === '')
            return $base;
        return $base . ltrim($path, '/');
    }
}
?>