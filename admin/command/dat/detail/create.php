<?php
include '../../../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$command_id = $_GET['command_id'];

if (!$command_id) {
    echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Tidak Bisa Membuat Command Detail Dat!',
                text: 'Command ID tidak ditemukan.',
                icon: 'warning',
                button: false,
                timer: 1200,
            }).then(() => {
                window.location = '../../dat/detail/list';
            });
        });
    </script>";
    exit;
}

$datName = '';
$qDat = $conn->prepare("SELECT command_name FROM device_command WHERE command_id = ?");
$qDat->bind_param("i", $command_id);
$qDat->execute();
$qResult = $qDat->get_result();
if ($qResult->num_rows > 0) {
    $dat = $qResult->fetch_assoc();
    $datName = $dat['command_name'];
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $data_name = trim($_POST['data_name']);
    $data_type = trim($_POST['data_type']);
    $data_len = intval($_POST['data_len']);
    $data_default = trim($_POST['data_default']);

    $check = $conn->prepare("SELECT 1 FROM device_command_detail WHERE command_id = ? AND data_name = ?");
    $check->bind_param("is", $command_id, $data_name);
    $check->execute();
    $exist = $check->get_result()->num_rows > 0;
    if ($exist) {
        echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Gagal!',
                    text: 'Nama command sudah ada.',
                    icon: 'error',
                    button: false,
                    timer: 1200,
                 }).then(() => {
                window.location = './create?command_id=$command_id';
            });
        });
        </script>";
        exit;
    }
    if ($data_name && $data_type && $data_len > 0) {
        $stmt = $conn->prepare(" INSERT INTO device_command_detail (command_id, updated_by, data_name, data_type, data_len, data_default) VALUES (?, ?, ?, ?, ?, ?) ");
        $stmt->bind_param("iissis", $command_id, $user_id, $data_name, $data_type, $data_len, $data_default);

        if ($stmt->execute()) {
            echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
            <script>
                document.addEventListener('DOMContentLoaded', function() {
                    swal({
                        title: 'Berhasil!',
                        text: 'Data detail command berhasil ditambahkan.',
                        icon: 'success',
                        button: false,
                        timer: 1200,
                    }).then(() => window.location = './admin/command/dat/detail/list?command_id=$command_id');
                });
            </script>";
        } else {
            echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
            <script>
                document.addEventListener('DOMContentLoaded', function() {
                    swal({
                        title: 'Gagal!',
                        text: 'Terjadi kesalahan saat menyimpan data.',
                        icon: 'error',
                        button: false,
                        timer: 1200,
                    }).then(() => {
                        window.location = './admin/command/dat/detail/create?command_id=$command_id';
                    });
                });
            </script>";
            exit;
        }
    }
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <base href="/owl_device/">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Create Command Dat Detail</title>
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

</head>

<body>
    <div class="wrapper">
        <?php $rootPath = $_SERVER['DOCUMENT_ROOT'];
        include $rootPath . "/owl_device/includes/sidebar.php"; ?>
        <div class="main-panel">
            <?php $rootPath = $_SERVER['DOCUMENT_ROOT'];
            include $rootPath . "/owl_device/includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">add :: dat :: <?= htmlspecialchars($datName) ?></h3>
                        </div>
                    </div>
                    <div class="card card-round">
                        <div class="card-header">
                            <h4 class="card-title mb-0">Add Command Detail Data</h4>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="data_name">Data Name</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="data_name" name="data_name" placeholder="Enter command name..." required>
                                </div>

                                <div class="form-group">
                                    <label for="data_type">Data Type</label>
                                    <select name="data_type" id="data_type" class="form-control" required>
                                        <option value="">-- Select Data Type --</option>
                                        <option value="string">String</option>
                                        <option value="object">Object</option>
                                        <option value="bool">Boolean</option>
                                        <option value="uint8_t">uint8_t</option>
                                        <option value="uint16_t">uint16_t</option>
                                        <option value="uint32_t">uint32_t</option>
                                        <option value="uint64_t">uint64_t</option>
                                        <option value="int8_t">int8_t</option>
                                        <option value="int16_t">int16_t</option>
                                        <option value="int32_t">int32_t</option>
                                        <option value="int64_t">int64_t</option>
                                        <option value="float">float</option>
                                    </select>
                                </div>

                                <div class="form-group">
                                    <label for="data_len">Data Length (bytes)</label>
                                    <input type="number" class="form-control form-control-border border-width-2"
                                        id="data_len" name="data_len" required>
                                </div>

                                <div class="form-group">
                                    <label for="data_default">Default Value</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="data_default" name="data_default" placeholder="Enter command default value..." required>
                                </div>
                            </div>

                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Submit</button>
                                <button type="button" class="btn btn-secondary"
                                    onclick="window.location.href='admin/command/dat/detail/list?command_id=<?= $command_id ?>'">Cancel</button>
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

    <script>
        document.getElementById('data_type').addEventListener('change', function () {
            const dataType = this.value;
            const dataLenInput = document.getElementById('data_len');
            const fixedLengths = {
                'bool': 1,
                'uint8_t': 1,
                'uint16_t': 2,
                'uint32_t': 4,
                'uint64_t': 8,
                'int8_t': 1,
                'int16_t': 2,
                'int32_t': 4,
                'int64_t': 8,
                'float': 4
            };

            if (dataType === 'string' || dataType === 'object') {
                dataLenInput.value = '';
                dataLenInput.readOnly = false;
                dataLenInput.placeholder = "Enter custom length...";
            } else if (fixedLengths[dataType]) {
                dataLenInput.value = fixedLengths[dataType]
                dataLenInput.readOnly = true;
            } else {
                dataLenInput.value = '';
                dataLenInput.readOnly = true;
            }
        });
    </script>
</body>

</html>