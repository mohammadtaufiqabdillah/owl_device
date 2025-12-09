<?php
require_once '../../../../config_base.php';
require_once '../../../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];
$command_id = isset($_GET['command_id']) ? intval($_GET['command_id']) : 0;
$command_detail_id = isset($_GET['command_detail_id']) ? intval($_GET['command_detail_id']) : 0;

if ($command_id <= 0 || $command_detail_id <= 0) {
    echo "<script>alert('Parameter tidak valid!'); window.location='../../../../admin/command/set/list';</script>";
    exit;
}

$qSet = $conn->prepare("SELECT * FROM device_command WHERE command_id = ? AND company_code = ?");
$qSet->bind_param("ii", $command_id, $company_code);
$qSet->execute();
$qResult = $qSet->get_result();
$set = $qResult->fetch_assoc();

if (!$set) {
    echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', () => {
            swal({
                title: 'Data Tidak Ditemukan!',
                text: 'Command detail tidak tersedia.',
                icon: 'error',
                button: false,
                timer: 1200,
            }).then(() => {
                window.location = '../list';
            });
        });
    </script>";
    exit;
}

$setName = $set['command_name'] ?? '';

$qdSet = $conn->prepare("SELECT * FROM device_command_detail WHERE command_detail_id = ?");
$qdSet->bind_param("i", $command_detail_id);
$qdSet->execute();
$qdResult = $qdSet->get_result();

if ($qdResult->num_rows === 0) {
    echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', () => {
            swal({
                title: 'Data Tidak Ditemukan!',
                text: 'Command detail tidak tersedia.',
                icon: 'error',
                button: false,
                timer: 1200,
            }).then(() => {
                window.location = 'admin/command/set/detail/list?command_id=$command_id';
            });
        });
    </script>";
    exit;
}

$dset = $qdResult->fetch_assoc();

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $data_name = trim($_POST['data_name']);
    $data_type = strtolower(trim($_POST['data_type']));
    $data_len = intval($_POST['data_len']);
    $data_default = trim($_POST['data_default']);

    $check = $conn->prepare("SELECT * FROM device_command_detail WHERE command_id = ? AND data_name = ? AND command_detail_id != ?");
    $check->bind_param("isi", $command_id, $data_name, $command_detail_id);
    $check->execute();
    $exist = $check->get_result()->num_rows > 0;

    if ($exist) {
        echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', () => {
                swal({
                    title: 'Gagal!',
                    text: 'Nama data sudah digunakan pada command ini.',
                    icon: 'error',
                    button: false,
                    timer: 1200,
                }).then(() => {
                    window.location = 'edit?command_id=$command_id&command_detail_id=$command_detail_id';
                });
            });
        </script>";
        exit;
    }

    $stmt = $conn->prepare("UPDATE device_command_detail SET data_name = ?, data_type = ?, data_len = ?, data_default = ?, updated_by = ? WHERE command_detail_id = ?");
    $stmt->bind_param("ssisii", $data_name, $data_type, $data_len, $data_default, $user_id, $command_detail_id);

    if ($stmt->execute()) {
        echo "
        <script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Berhasil!',
                    text: 'Data detail command berhasil diperbarui.',
                    icon: 'success',
                    button: false,
                    timer: 1200
                }).then(() => window.location = './admin/command/set/detail/list?command_id=$command_id');
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
                window.location = './admin/command/set/detail/edit?command_id=$command_id&command_detail_id=$command_detail_id';
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
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Edit Command Set Detail</title>
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
        <?php include '../../../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include '../../../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="pt-2 pb-4">
                        <h3 class="fw-bold mb-3">
                            edit :: set :: <?= htmlspecialchars($setName) ?> ::
                            <?= htmlspecialchars($dset['data_name']) ?>
                        </h3>
                    </div>

                    <div class="card card-round">
                        <div class="card-header">
                            <h4 class="card-title mb-0">Edit Command Set Detail</h4>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="data_name">Data Name</label>
                                    <input type="text" class="form-control" id="data_name" name="data_name"
                                        value="<?= htmlspecialchars($dset['data_name']) ?>" required>
                                </div>

                                <div class="form-group">
                                    <label for="data_type">Data Type</label>
                                    <select name="data_type" id="data_type" class="form-control" required>
                                        <option value="">-- Select Data Type --</option>
                                        <option value="string" <?= $dset['data_type'] === 'string' ? 'selected' : '' ?>>
                                            String</option>
                                        <option value="object" <?= $dset['data_type'] === 'object' ? 'selected' : '' ?>>
                                            Object</option>
                                        <option value="bool" <?= $dset['data_type'] === 'bool' ? 'selected' : '' ?>>Boolean
                                        </option>
                                        <?php
                                        $types = ['uint8_t', 'uint16_t', 'uint32_t', 'uint64_t', 'int8_t', 'int16_t', 'int32_t', 'int64_t', 'float'];
                                        foreach ($types as $t) {
                                            $selected = ($t == $dset['data_type']) ? 'selected' : '';
                                            echo "<option value='$t' $selected>$t</option>";
                                        }
                                        ?>
                                    </select>
                                </div>

                                <div class="form-group">
                                    <label for="data_len">Data Length (bytes)</label>
                                    <input type="number" class="form-control" id="data_len" name="data_len"
                                        value="<?= htmlspecialchars($dset['data_len']) ?>" required>
                                </div>

                                <div class="form-group">
                                    <label for="data_default">Default Value</label>
                                    <input type="text" class="form-control" id="data_default" name="data_default"
                                        value="<?= htmlspecialchars($dset['data_default']) ?>" required>
                                </div>
                            </div>

                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Submit</button>
                                <button type="button" class="btn btn-secondary"
                                    onclick="window.location.href='admin/command/set/detail/list?command_id=<?= $command_id ?>'">
                                    Cancel
                                </button>
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
        function updateDataLenState() {
            const dataType = document.getElementById('data_type').value;
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
                dataLenInput.readOnly = false;
                dataLenInput.placeholder = "Enter custom length for string";
            } else if (fixedLengths[dataType]) {
                dataLenInput.value = fixedLengths[dataType];
                dataLenInput.readOnly = true;
            } else {
                dataLenInput.value = '';
                dataLenInput.readOnly = true;
            }
        }

        document.getElementById('data_type').addEventListener('change', updateDataLenState);
        document.addEventListener('DOMContentLoaded', updateDataLenState);
    </script>
</body>

</html>