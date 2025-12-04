<?php
include '../../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];

if (!$company_code) {
    echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Tidak Bisa Membuat Command Set!',
                text: 'Kamu belum tergabung atau belum membuat perusahaan (PT).',
                icon: 'warning',
                button: false,
                timer: 1200,
            }).then(() => window.location.replace('../../companies/list'));
        });
    </script>";
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $command_name = trim($_POST['nameCommandSet']);
    $custom_class = trim($_POST['customClassCommand']) ?: null;
    $description = trim($_POST['commandDescription']) ?: null;
    $is_main = isset($_POST['commandIsMain']) ? 1 : 0;

    $command_type = 'set';
    $check = $conn->prepare("SELECT 1 FROM device_command WHERE company_code = ? AND command_name = ? AND command_type = ?");
    $check->bind_param("iss", $company_code, $command_name, $command_type);
    $check->execute();
    $exist = $check->get_result()->num_rows > 0;
    if ($exist) {
        echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Gagal!',
                    text: 'Nama command sudah ada.',
                    icon: 'error',
                    button: false,
                    timer: 1200,
                }).then(() => {
                window.location = './create';
            });
        });
        </script>";
        exit;
    }

    $query = $conn->prepare("SELECT MAX(command_code) AS lastKode FROM device_command WHERE company_code = ? AND command_type = ?");
    $query->bind_param("is", $company_code, $command_type);
    $query->execute();
    $result = $query->get_result()->fetch_assoc();
    $nextKode = (isset($result['lastKode']) && $result['lastKode'] !== null) ? $result['lastKode'] + 1 : 1;

    $stmt = $conn->prepare(" INSERT INTO device_command (company_code, command_code, updated_by, command_name, is_main, custom_class, description, command_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    $stmt->bind_param(
        "iiisisss",
        $company_code,
        $nextKode,
        $user_id,
        $command_name,
        $is_main,
        $custom_class,
        $description,
        $command_type
    );

    if ($stmt->execute()) {
        echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Berhasil!',
                    text: 'Error Command Set berhasil ditambahkan.',
                    icon: 'success',
                    button: false,
                    timer: 1200,
                }).then(() => window.location = './admin/command/set/list');
            });
        </script>";
    } else {
        echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Gagal!',
                    text: 'Terjadi kesalahan saat menyimpan data.',
                    icon: 'error',
                    button: false,
                    timer: 1200,
                }).then(() => {
                    window.location = './create';
                });
            });
        </script>";
        exit;
    }
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <?php include_once dirname(__FILE__) . '/config_base.php'; ?>
    <base href="<?= $base ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Create Command Set</title>
    <meta content='width=device-width, initial-scale=1.0, shrink-to-fit=no' name='viewport' />
    <link rel="icon" href="assets/img/OWLlogo.png" type="image/x-icon" />

    <!-- Fonts and icons -->
    <script src="assets/js/plugin/webfont/webfont.min.js"></script>
    <script>
        WebFont.load({
            google: { "families": ["Public Sans:300,400,500,600,700"] },
            custom: { "families": ["Font Awesome 5 Solid", "Font Awesome 5 Regular", "Font Awesome 5 Brands", "simple-line-icons"], urls: ['assets/css/fonts.min.css'] },
            active: function () {
                sessionStorage.fonts = true;
            }
        });
    </script>

    <!-- CSS Files -->
    <link rel="stylesheet" href="assets/css/bootstrap.min.css">
    <link rel="stylesheet" href="assets/css/plugins.min.css">
    <link rel="stylesheet" href="assets/css/kaiadmin.min.css">

    <style>
        .larger-checkbox {
            width: 40px;
            height: 30px;
            cursor: pointer;
        }

        .checkbox-label {
            display: block;
            margin-bottom: 6px;
            font-weight: 500;
        }
    </style>
</head>

<body>
    <div class="wrapper">
        <?php
        $rootPath = $_SERVER['DOCUMENT_ROOT'];
        include $rootPath . "<?= $base ?>includes/sidebar.php";
        ?>
        <div class="main-panel">
            <?php include $rootPath . "<?= $base ?>includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-center justify-content-between pt-2 pb-4">
                        <h3 class="fw-bold mb-3">Create Command Set</h3>
                    </div>
                    <div class="card card-round">
                        <div class="card-header">
                            <h4 class="card-title mb-0">Add Command Set Data</h4>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="nameCommandSet">Command Name</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="nameCommandSet" name="nameCommandSet" placeholder="Enter command name"
                                        required>
                                </div>
                                <div class="form-group">
                                    <label for="customClassCommand">Custom Class
                                        <span style="color: gray;">(Opsional)</span>
                                    </label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="customClassCommand" name="customClassCommand"
                                        placeholder="Enter custom class">
                                </div>
                                <div class="form-group">
                                    <label for="commandDescription">Description<span
                                            style="color: gray;">(Opsional)</span>
                                    </label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="commandDescription" name="commandDescription"
                                        placeholder="Enter description">
                                </div>
                                <div class="form-group">
                                    <label class="checkbox-label" for="commandIsMain">Is Main</label>
                                    <input type="checkbox" class="larger-checkbox" id="commandIsMain"
                                        name="commandIsMain">
                                </div>
                            </div>
                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Submit</button>
                                <button type="button" class="btn btn-secondary"
                                    onclick="window.location.href='admin/command/set/list'">Cancel</button>
                            </div>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!--   Core JS Files   -->
    <script src="assets/js/core/jquery-3.7.1.min.js"></script>
    <script src="assets/js/core/popper.min.js"></script>
    <script src="assets/js/core/bootstrap.min.js"></script>

    <!-- jQuery Scrollbar -->
    <script src="assets/js/plugin/jquery-scrollbar/jquery.scrollbar.min.js"></script>

    <!-- Sweet Alert -->
    <script src="assets/js/plugin/sweetalert/sweetalert.min.js"></script>

    <!-- Kaiadmin JS -->
    <script src="assets/js/kaiadmin.min.js"></script>
</body>

</html>