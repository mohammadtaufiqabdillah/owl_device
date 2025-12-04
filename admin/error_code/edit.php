<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];

if (!isset($_GET['error_code_id'])) {
    header('Location: ../error_code/list');
    exit;
}

$error_code_id = intval($_GET['error_code_id']);

$stmt = $conn->prepare("
    SELECT ec.*, 
           CASE ec.error_source 
                WHEN 1 THEN 'Hardware'
                WHEN 2 THEN 'Software'
                WHEN 3 THEN 'Hardware/Software'
                WHEN 4 THEN 'Network'
                ELSE 'Unknown'
           END AS source_name,
           dc.description AS category_name
    FROM device_error_code ec
    JOIN device_error_category dc ON ec.error_category = dc.error_category_id
    WHERE ec.company_code = ? AND ec.error_code_id = ?
");
$stmt->bind_param("ii", $company_code, $error_code_id);
$stmt->execute();
$result = $stmt->get_result();
$error_code = $result->fetch_assoc();

if (!$error_code) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Data tidak ditemukan',
                text: 'Error Code yang dimaksud tidak tersedia.',
                icon: 'error',
                buttons: false,
                timer: 1200
            }).then(() => {
                window.location.href = '../error_code/list';
            });
        });
    </script>";
    exit;
}


if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $description = trim($_POST['description']);

    $update = $conn->prepare("UPDATE device_error_code SET description = ?, updated_by = ? WHERE error_code_id = ? AND company_code = ?");
    $update->bind_param("siii", $description, $user_id, $error_code_id, $company_code);

    if ($update->execute()) {
        echo "
            <script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
            <script>
                document.addEventListener('DOMContentLoaded', function() {
                    swal({
                        title: 'Berhasil!',
                        text: 'Error Kategori berhasil diperbarui.',
                        icon: 'success',
                        buttons: false,
                        timer: 1200
                    }).then(() => {
                        window.location.href = './admin/error_code/list';
                    });
                });
            </script>";
    } else {
        echo "
            <script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
            <script>
                document.addEventListener('DOMContentLoaded', function() {
                    swal({
                        title: 'Gagal!',
                        text: 'Terjadi kesalahan saat memperbarui data.',
                        icon: 'error',
                        buttons: {
                            confirm: { className: 'btn btn-danger' }
                        }
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
    <title>Edit Error Code</title>
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
        include $rootPath . "<?= $base ?>includes/sidebar.php";?>
        <div class="main-panel">
            <?php $rootPath = $_SERVER['DOCUMENT_ROOT'];
            include $rootPath . "<?= $base ?>includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-center justify-content-between pt-2 pb-4">
                        <h3 class="fw-bold mb-3">Edit Error Code</h3>
                    </div>
                    <div class="card card-round">
                        <div class="card-header">
                            <h4 class="card-title mb-0">Edit Error Code Data</h4>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="nameErrorSource">Error Source</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="nameErrorSource" name="nameErrorSource"
                                        value="<?= htmlspecialchars($error_code['source_name']); ?>" readonly>
                                </div>

                                <div class="form-group">
                                    <label for="nameErrorCategory">Error Category</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="nameErrorCategory" name="nameErrorCategory"
                                        value="<?= htmlspecialchars($error_code['error_category']); ?>" readonly>
                                </div>

                                <div class="form-group">
                                    <label for="nameErrorCategory">Code Error</label>
                                    <input type="text" class="form-control form-control-border border-width-2"
                                        id="nameErrorCode" name="nameErrorCode"
                                        value="<?= htmlspecialchars($error_code['code_error']); ?>" readonly>
                                </div>

                                <div class="form-group">
                                    <label for="description">Description Error Code</label>
                                    <textarea class="form-control form-control-border border-width-2" id="description"
                                        name="description" required><?= htmlspecialchars($error_code['description']); ?>
                                    </textarea>
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

    <!-- Sweet Alert -->
    <script src="assets/js/plugin/sweetalert/sweetalert.min.js"></script>

    <!-- Kaiadmin JS -->
    <script src="assets/js/kaiadmin.min.js"></script>
</body>

</html>