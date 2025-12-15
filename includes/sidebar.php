<?php
$current_page = $_SERVER['REQUEST_URI'];
?>
<!-- Sidebar -->
<div class="sidebar" data-background-color="dark">
    <div class="sidebar-logo">
        <!-- Logo Header -->
        <div class="logo-header" data-background-color="dark">
            <a href="./admin/device/list" class="logo">
                <img src="./assets/img/OWLLogo.png" alt="OWL Logo"
                    class="navbar-brand brand-image img-circle elevation-3" height="55" style="opacity: .8;">
                <span class="fw-bold text-white ms-2 logo-text">OWL Device</span>
            </a>
            <div class="nav-toggle">
                <button class="btn btn-toggle toggle-sidebar">
                    <i class="gg-menu-right"></i>
                </button>
                <button class="btn btn-toggle sidenav-toggler">
                    <i class="gg-menu-left"></i>
                </button>
            </div>
            <button class="topbar-toggler more">
                <i class="gg-more-vertical-alt"></i>
            </button>
        </div>
        <!-- End Logo Header -->
    </div>
    <div class="sidebar-wrapper scrollbar scrollbar-inner">
        <div class="sidebar-content">
            <ul class="nav nav-secondary">
                <li class="nav-item <?= (strpos($current_page, '/admin/companies/') !== false) ? 'active' : '' ?>">
                    <a href="./admin/companies/list">
                        <i class="fas fa-building"></i>
                        <p>Companies</p>
                    </a>
                </li>
                <li class="nav-item <?= (strpos($current_page, '/admin/categories/') !== false) ? 'active' : '' ?>">
                    <a href="./admin/categories/list">
                        <i class="fas fa-layer-group"></i>
                        <p>Categories</p>
                    </a>
                </li>
                <li class="nav-item <?= (strpos($current_page, '/admin/device/') !== false) ? 'active' : '' ?>">
                    <a href="./admin/device/list">
                        <i class="fas fa-microchip"></i>
                        <p>Devices</p>
                    </a>
                </li>
                <!-- <li class="nav-item <?= (strpos($current_page, '/admin/status_device/') !== false) ? 'active' : '' ?>">
                    <a href="./admin/status_device/list">
                        <i class="fas fa-server"></i>
                        <p>Status Devices</p>
                    </a>
                </li> -->
                <?php
                $is_command_active = (
                    strpos($current_page, '/admin/command/dat/') !== false ||
                    strpos($current_page, '/admin/command/exe/') !== false ||
                    strpos($current_page, '/admin/command/set/') !== false
                );
                ?>
                <li class="nav-item <?= $is_command_active ? 'active submenu' : '' ?>">
                    <a data-bs-toggle="collapse" href="#commandMenu"
                        aria-expanded="<?= $is_command_active ? 'true' : 'false' ?>">
                        <i class="fas fa-terminal"></i>
                        <p>Commands</p>
                        <span class="caret"></span>
                    </a>
                    <div class="collapse <?= $is_command_active ? 'show' : '' ?>" id="commandMenu">
                        <ul class="nav nav-collapse">
                            <li class="<?= (strpos($current_page, '/admin/command/dat/') !== false) ? 'active' : '' ?>">
                                <a href="./admin/command/dat/list">
                                    <span class="sub-item">Command Dat</span>
                                </a>
                            </li>
                            <li class="<?= (strpos($current_page, '/admin/command/exe/') !== false) ? 'active' : '' ?>">
                                <a href="./admin/command/exe/list">
                                    <span class="sub-item">Command Exe</span>
                                </a>
                            </li>
                            <li class="<?= (strpos($current_page, '/admin/command/set/') !== false) ? 'active' : '' ?>">
                                <a href="./admin/command/set/list">
                                    <span class="sub-item">Command Set</span>
                                </a>
                            </li>
                        </ul>
                    </div>
                </li>
                <li
                    class="nav-item <?= (strpos($current_page, '/admin/error_categories/') !== false) ? 'active' : '' ?>">
                    <a href="./admin/error_categories/list">
                        <i class="fas fa-bug"></i>
                        <p>Error Categories</p>
                    </a>
                </li>
                <li class="nav-item <?= (strpos($current_page, '/admin/error_code/') !== false) ? 'active' : '' ?>">
                    <a href="./admin/error_code/list">
                        <i class="fas fa-code"></i>
                        <p>Error Codes</p>
                    </a>
                </li>
            </ul>
        </div>
    </div>
</div>
<!-- End Sidebar -->