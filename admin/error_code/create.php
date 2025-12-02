<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];
$success = $error = '';

if (!$company_code) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Tidak Bisa Membuat Error Kategori!',
                text: 'Kamu belum tergabung atau belum membuat perusahaan (PT).',
                icon: 'warning',
                button: false,
                timer: 1200,
            }).then(() => window.location.replace('../companies/list'));
        });
    </script>";
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $error_source = intval($_POST['error_source']);
    $error_category = intval($_POST['error_category']);
    $description = trim($_POST['descriptionErrorCode']);

    $queryKategori = $conn->prepare("SELECT error_category_code FROM device_error_category WHERE error_category_id = ? AND company_code = ?");
    $queryKategori->bind_param("ii", $error_category, $company_code);
    $queryKategori->execute();
    $resKategori = $queryKategori->get_result()->fetch_assoc();
    $error_category_code = $resKategori['error_category_code'] ?? 0;

    $queryLast = $conn->prepare("SELECT MAX(RIGHT(code_error, 2)) AS lastNumber FROM device_error_code WHERE company_code = ? AND error_source = ? AND error_category = ?");
    $queryLast->bind_param("iii", $company_code, $error_source, $error_category);
    $queryLast->execute();
    $lastRow = $queryLast->get_result()->fetch_assoc();
    $nextNumber = str_pad(($lastRow['lastNumber'] ?? 0) + 1, 2, '0', STR_PAD_LEFT);
    $code_error = sprintf("%02d%02d%s", $error_source, $error_category_code, $nextNumber);

    $query = $conn->prepare("SELECT MAX(error_code) AS lastKode FROM device_error_code WHERE company_code = ?");
    $query->bind_param("i", $company_code);
    $query->execute();
    $result = $query->get_result()->fetch_assoc();
    $nextKode = ($result['lastKode'] ?? 0) + 1;

    $stmt = $conn->prepare("INSERT INTO device_error_code (company_code, updated_by, error_source, error_category, error_number, code_error, error_code, description) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    $stmt->bind_param("iiiiiiis", $company_code, $user_id, $error_source, $error_category, $nextNumber, $code_error, $lastKode, $description);


    if ($stmt->execute()) {
        $success = "Error Code berhasil dibuat!";
    } else {
        $error = "Terjadi kesalahan: " . $stmt->error;
    }
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <base href="/owl_device/">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Create Error Code</title>
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
            <?php include $rootPath . "/owl_device/includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left flex-column pt-2 pb-4">
                        <h3 class="fw-bold mb-3">Create Error Code</h3>
                    </div>

                    <div class="card card-round">
                        <div class="card-header">
                            <div class="card-title">Add Error Code Data</div>
                        </div>
                        <form method="POST" id="formErrorCode">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="nameErrorSource">Error Source</label>
                                    <select name="error_source" id="nameErrorSource" class="form-control" required>
                                        <option value="">-- Select Source --</option>
                                        <option value="1">Hardware</option>
                                        <option value="2">Software</option>
                                        <option value="3">Hardware/Software</option>
                                        <option value="4">Network</option>
                                    </select>
                                </div>

                                <div class="form-group">
                                    <label for="nameErrorCategory">Error Category</label>
                                    <select name="error_category" id="nameErrorCategory" class="form-control" required>
                                        <option value="">-- Select Category --</option>
                                        <?php
                                        $result = $conn->prepare("SELECT error_category_id, description, error_category_code FROM device_error_category WHERE company_code = ?");
                                        $result->bind_param("i", $company_code);
                                        $result->execute();
                                        $error_categories = $result->get_result();
                                        while ($cat = $error_categories->fetch_assoc()) {
                                            echo "<option value='{$cat['error_category_id']}' data-kode='{$cat['error_category_code']}'>{$cat['description']}</option>";
                                        }
                                        ?>
                                    </select>
                                </div>

                                <div class="form-group">
                                    <label for="nameErrorCode">Code Error</label>
                                    <input type="text" id="nameErrorCode" name="nameErrorCode"
                                        class="form-control bg-light" readonly placeholder="XXXXXX">
                                </div>

                                <div class="form-group">
                                    <label for="descriptionErrorCode">Description Error Code</label>
                                    <textarea class="form-control" id="descriptionErrorCode" name="descriptionErrorCode"
                                        rows="2" placeholder="Enter error description" required></textarea>
                                </div>
                            </div>

                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Submit</button>
                                <button type="button" class="btn btn-secondary"
                                    onclick="window.location.href='admin/error_code/list'">Cancel</button>
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

    <!-- Datatables -->
    <script src="assets/js/plugin/datatables/datatables.min.js"></script>

    <!-- Sweet Alert -->
    <script src="assets/js/plugin/sweetalert/sweetalert.min.js"></script>

    <!-- Kaiadmin JS -->
    <script src="assets/js/kaiadmin.min.js"></script>

    <script>
        $('#nameErrorSource, #nameErrorCategory').on('change', function () {
            const source = $('#nameErrorSource').val();
            const kodeKategori = $('#nameErrorCategory option:selected').data('kode');
            let code = 'XXXXXX';

            if (source) {
                code = source.toString().padStart(2, '0') + 'XXXX';
            }

            if (source && kodeKategori !== undefined) {
                code = source.toString().padStart(2, '0') + kodeKategori.toString().padStart(2, '0') + 'XX';
            }

            $('#nameErrorCode').val(code);
        });

        <?php if ($success): ?>
            document.addEventListener('DOMContentLoaded', () => {
                swal({
                    title: 'Berhasil!',
                    text: '<?php echo $success; ?>',
                    icon: 'success',
                    button: false,
                    timer: 1200
                }).then(() => window.location = 'admin/error_code/list');
            });
        <?php elseif ($error): ?>
            document.addEventListener('DOMContentLoaded', () => {
                swal('Gagal!', '<?php echo $error; ?>', 'error');
            });
        <?php endif; ?>
    </script>

</body>

</html>