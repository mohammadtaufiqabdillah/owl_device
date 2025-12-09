<?php
include __DIR__ . '/../config_base.php';
include __DIR__ . '/../connection.php';

$user_id = $_SESSION['user_id'] ?? null;
$user_name = $_SESSION['user_name'] ?? "User";
$user_email = $_SESSION['user_email'] ?? "";
$profile_pic = $_SESSION['user_profile_picture'] ?? "assets/img/profile.jpg";

$projectRootDir = realpath(__DIR__ . '/..') . DIRECTORY_SEPARATOR;

$relativePath = ltrim($profile_pic, '/');

$fullPath = $projectRootDir . str_replace('/', DIRECTORY_SEPARATOR, $relativePath);

if (function_exists('base_url')) {
    $asset = function ($p) {
        return base_url($p);
    };
} else {
    $asset = function ($p) {
        $root = defined('BASE_URL') ? rtrim(BASE_URL, '/') : '/';
        return $root . '/' . ltrim($p, '/');
    };
}

if ($fullPath && file_exists($fullPath)) {
    $profile_url = $asset($relativePath);
} else {
    $profile_url = $asset('assets/img/profile.jpg');
}
?>
<div class="main-header">
    <nav class="navbar navbar-header navbar-header-transparent navbar-expand-lg border-bottom">
        <div class="container-fluid">
            <ul class="navbar-nav ms-auto align-items-center">
                <li class="nav-item topbar-user dropdown hidden-caret">

                    <a class="dropdown-toggle profile-pic" data-bs-toggle="dropdown">
                        <div class="avatar-sm">
                            <img src="<?= htmlspecialchars($profile_url) ?>" class="avatar-img rounded-circle">
                        </div>
                        <span class="profile-username">
                            <span class="op-7">Hi,</span>
                            <span class="fw-bold"><?= htmlspecialchars($user_name) ?></span>
                        </span>
                    </a>

                    <ul class="dropdown-menu dropdown-user animated fadeIn">

                        <li>
                            <div class="user-box">
                                <div class="avatar-lg">
                                    <img src="<?= htmlspecialchars($profile_url) ?>" class="avatar-img rounded">
                                </div>
                                <div class="u-text">
                                    <h4><?= htmlspecialchars($user_name) ?></h4>
                                    <p class="text-muted"><?= htmlspecialchars($user_email) ?></p>
                                </div>
                            </div>
                        </li>

                        <li>
                            <div class="dropdown-divider"></div>
                        </li>

                        <li>
                            <a class="dropdown-item" href="admin/account_setting">
                                Account Setting
                            </a>
                        </li>

                        <li>
                            <div class="dropdown-divider"></div>
                        </li>

                        <li>
                            <a class="dropdown-item" href="logout">
                                Logout
                            </a>
                        </li>

                    </ul>

                </li>
            </ul>
        </div>
    </nav>
</div>