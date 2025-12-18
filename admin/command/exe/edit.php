<?php
require_once '../../../config_base.php';
require_once '../../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];
$command_id = intval($_GET['command_id']);

if (!isset($_GET['command_id'])) {
    header('Location: ../command/exe/list');
    exit;
}

$stmt = $conn->prepare("SELECT * FROM device_command WHERE command_id = ? AND company_code = ?");
$stmt->bind_param("is", $command_id, $company_code);
$stmt->execute();
$result = $stmt->get_result();
$command_exe = $result->fetch_assoc();

if (!$command_exe) {
    echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Data tidak ditemukan',
                text: 'Command yang dimaksud tidak tersedia.',
                icon: 'error',
                buttons: false,
                timer: 1200
            }).then(() => {
                window.location.href = '../exe/list';
            });
        });
    </script>";
    exit;
}


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $conn->begin_transaction();

    try {
        $command_name = trim($_POST['command_name']);
        $custom_class = trim($_POST['custom_class']) ?: null;
        $description = trim($_POST['description']) ?: null;
        $is_main = isset($_POST['is_main']) ? 1 : 0;
        $original_name = $command_exe['command_name'];

        if (strcasecmp($original_name, $command_name) !== 0) {
            $check = $conn->prepare("SELECT * FROM device_command WHERE company_code = ? AND command_name COLLATE utf8mb4_general_ci = ? AND command_id != ? LIMIT 1");
            $check->bind_param("isi", $company_code, $command_name, $command_id);
            $check->execute();
            $exist = $check->get_result()->num_rows > 0;

            if ($exist) {
                echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', () => {
                swal({
                    title: 'Gagal!',
                    text: 'Nama command sudah digunakan pada data ini.',
                    icon: 'error',
                    button: false,
                    timer: 1200,
                }).then(() => {
                    window.location = 'edit?command_id=$command_id';
                });
            });
        </script>";
                exit;
            }
        }

        $update = $conn->prepare("UPDATE device_command SET command_name = ?, custom_class = ?, description = ?, updated_by = ?, is_main = ? WHERE command_id = ? AND company_code = ? AND command_type = 'exe' ");
        $update->bind_param("sssiiii", $command_name, $custom_class, $description, $user_id, $is_main, $command_id, $company_code);
        $update->execute();

        $pair = $conn->prepare(" SELECT pair_command_id FROM device_command WHERE command_id = ? ");
        $pair->bind_param("i", $command_id);
        $pair->execute();
        $resPair = $pair->get_result()->fetch_assoc();

        if ($resPair && $resPair['pair_command_id']) {
            $res_id = intval($resPair['pair_command_id']);

            $updateRes = $conn->prepare(" UPDATE device_command SET command_name = ?, custom_class = ?, description = ?, is_main = ? WHERE command_id = ? AND command_type = 'res' ");
            $updateRes->bind_param("sssii", $command_name, $custom_class, $description, $is_main, $res_id);
            $updateRes->execute();
        }
        $conn->commit();

        echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
    document.addEventListener('DOMContentLoaded', function () {
        swal({
            title: 'Berhasil!',
            text: 'Command EXE & RES berhasil diperbarui.',
            icon: 'success',
            buttons: false,
            timer: 1200
        }).then(() => {
            window.location.href = './admin/command/exe/list';
        });
    });
    </script>";
    } catch (Exception $e) {
        $conn->rollback();
        echo "<script src='../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
    document.addEventListener('DOMContentLoaded', function () {
        swal({
            title: 'Gagal!',
            text: 'Terjadi kesalahan saat update data.',
            icon: 'error',
            button: 'OK'
        });
    });
    </script>
    ";
        exit;
    }
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Edit Command Exe</title>
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
        <?php include '../../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include '../../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-center justify-content-between pt-2 pb-4">
                        <h3 class="fw-bold mb-3">Edit Command Exe</h3>
                    </div>
                    <div class="card card-round">
                        <div class="card-header">
                            <h4 class="card-title mb-0">Edit Command Exe Data</h4>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="command_name">Command Name</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="command_name" name="command_name" required
                                        value="<?= htmlspecialchars($command_exe['command_name']); ?>">
                                </div>
                                <div class="form-group">
                                    <label for="custom_class">Custom Class</label>
                                    <span style="color: gray;">(Opsional)</span>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="custom_class" name="custom_class" placeholder="Enter custom class"
                                        value="<?= htmlspecialchars($command_exe['custom_class']); ?>">
                                </div>
                                <div class="form-group">
                                    <label for="description">Description</label>
                                    <span style="color: gray;">(Opsional)</span>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="description" name="description" placeholder="Enter description"
                                        value="<?= htmlspecialchars($command_exe['description']); ?>">
                                </div>
                                <div class="form-group">
                                    <label class="checkbox-label" for="is_main">Is Main</label>
                                    <input type="checkbox" class="larger-checkbox" id="is_main" name="is_main" value="1"
                                        <?= $command_exe['is_main'] ? 'checked' : '' ?>>
                                </div>
                            </div>
                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Save Changes</button>
                                <button type="button" class="btn btn-secondary"
                                    onclick="window.location.href='admin/command/exe/list'">Cancel</button>
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