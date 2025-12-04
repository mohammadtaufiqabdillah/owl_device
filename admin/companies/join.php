<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$userId = $_SESSION['user_id'];

$userData = $conn->query("SELECT company_code FROM device_user WHERE user_id = $userId")->fetch_assoc();
$companyCodeUser = $userData['company_code'] ?? null;

if (!empty($companyCodeUser) && $_SERVER['REQUEST_METHOD'] !== 'POST') {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
    document.addEventListener('DOMContentLoaded', function() {
        swal({
            title: 'Tidak Bisa Bergabung PT Baru!',
            text: 'Kamu sudah tergabung di PT dengan kode: $companyCodeUser',
            icon: 'info',
            buttons: false,
            timer: 1200,
        }).then(() => window.location.replace('../companies/list'));
    });
    </script>";
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $company_token = trim($_POST['tokenCompanies']);

    $stmt = $conn->prepare("SELECT company_code, company_name FROM device_company WHERE company_token = ?");
    $stmt->bind_param("s", $company_token);
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows > 0) {
        $pt = $result->fetch_assoc();
        $company_code = $pt['company_code'];
        $company_name = $pt['company_name'];

        $update = $conn->prepare("UPDATE device_user SET company_code = ? WHERE user_id = ?");
        $update->bind_param("ii", $company_code, $userId);
        $update->execute();

        $_SESSION['company_code'] = $company_code;

        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Berhasil Bergabung!',
                text: 'Kamu sekarang menjadi anggota PT $company_name',
                icon: 'success',
                button: 'OK'
            }).then(() => window.location = './admin/companies/list');
        });
        </script>";
    } else {
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal('Token Salah!', 'Token PT tidak ditemukan.', 'error');
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
    <title>Join Company</title>
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
        include $rootPath . "<?= $base ?>includes/sidebar.php"; ?>
        <div class="main-panel">
            <?php $rootPath = $_SERVER['DOCUMENT_ROOT'];
            include $rootPath . "<?= $base ?>includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <h3 class="fw-bold mb-3">Join Company</h3>
                    <div class="card card-round">
                        <div class="card-header">
                            <div class="card-title">Join with Token</div>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="tokenCompanies">Company Token</label>
                                    <input type="text" class="form-control" id="tokenCompanies" name="tokenCompanies"
                                        required placeholder="Enter company token">
                                </div>
                            </div>
                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Join</button>
                                <button type="button" class="btn btn-secondary"
                                    onclick="window.location.href='admin/companies/list'">Cancel</button>
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