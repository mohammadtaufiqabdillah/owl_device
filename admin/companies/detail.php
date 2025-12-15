<?php
require_once '../../config_base.php';
require_once '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_id = isset($_GET['company_id']) ? intval($_GET['company_id']) : 0;

if ($company_id <= 0) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            swal({
                title: 'ID PT Tidak Valid',
                text: 'ID PT yang diberikan tidak valid.',
                icon: 'error',
                buttons: false,
                timer : 1200
            }).then(() => {
                window.location.href = '../companies/list';
            });
        </script>";
    exit;
}

$stmt = $conn->prepare("SELECT c.*, u.user_name AS creator_name, u.user_email AS creator_email FROM device_company c LEFT JOIN device_user u ON u.user_id = c.created_by WHERE c.company_id = ? LIMIT 1");
$stmt->bind_param("i", $company_id);
$stmt->execute();
$result = $stmt->get_result();

if (!$result || $result->num_rows === 0) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            swal({
                title: 'Data PT Error',
                text: 'Terjadi kesalahan saat mengambil data PT.',
                icon: 'error'
                buttons: false,
                timer : 1200
            }).then(() => {
                window.location.href = '../companies/list';
            });
        </script>";
    exit;
}

$company = $result->fetch_assoc();
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Company Detail</title>
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
        <?php include __DIR__ . '/../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include __DIR__ . '/../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">Company Detail</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <h4>Company Information</h4>
                                </div>
                                <div class="card-body p-4">
                                    <table class="table table-borderless">
                                        <tr>
                                            <td><b>ID</b></td>
                                            <td><?= htmlspecialchars($company['company_code']) ?></td>
                                        </tr>
                                        <tr>
                                            <td><b>Company Name</b></td>
                                            <td><?= htmlspecialchars($company['company_name']) ?></td>
                                        </tr>
                                        <tr>
                                            <td><b>Address</b></td>
                                            <td><?= htmlspecialchars($company['company_address']) ?></td>
                                        </tr>
                                        <tr>
                                            <td><b>Phone</b></td>
                                            <td><?= htmlspecialchars($company['company_telephone_number']) ?></td>
                                        </tr>
                                        <tr>
                                            <td><b>Email</b></td>
                                            <td><?= htmlspecialchars($company['company_email']) ?></td>
                                        </tr>
                                        <tr>
                                            <td><b>Code Token</b></td>
                                            <td><?= htmlspecialchars($company['company_token']) ?></td>
                                        </tr>
                                        <tr>
                                            <td><b>Created By</b></td>
                                            <td>
                                                <?php
                                                if (!empty($company['creator_name'])) {
                                                    echo htmlspecialchars($company['creator_name']) . " (" . htmlspecialchars($company['creator_email']) . ")";
                                                } else {
                                                    echo "N/A";
                                                } ?>
                                            </td>
                                        </tr>
                                    </table>
                                    <div class="d-flex justify-content-end mt-4">
                                        <button type="button" class="btn btn-secondary"
                                            onclick="window.location.href='admin/companies/list'">Back</button>
                                    </div>
                                </div>
                            </div>
                        </div>
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