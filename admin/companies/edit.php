<?php
require_once '../../config_base.php';
require_once '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$currentKodePT = $_SESSION['company_code'] ?? null;

if (!isset($_GET['company_id'])) {
    header('Location: ../companies/list');
    exit;
}

$company_id = intval($_GET['company_id']);

$stmt = $conn->prepare("SELECT * FROM device_company WHERE company_id = ? AND company_code = ?");
$stmt->bind_param("ii", $company_id, $currentKodePT);
$stmt->execute();
$result = $stmt->get_result();
$company = $result->fetch_assoc();

if (!$company) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Data tidak ditemukan',
                text: 'Perusahaan yang dimaksud tidak tersedia.',
                icon: 'error',
                buttons: false,
                timer : 1200
            }).then(() => {
                window.location.href = '../companies/list';
            });
        });
    </script>";
    exit;
}

$oldKodePT = $company['company_code'];

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $company_name = trim($_POST['nameCompanies'] ?? '');
    $newKodePT = intval(trim($_POST['idCompanies'] ?? $oldKodePT));
    $company_address = trim($_POST['addressCompanies'] ?: null);
    $company_telephone_number = trim($_POST['phoneCompanies'] ?: null);
    $company_email = trim($_POST['emailCompanies'] ?: null);

    $check = $conn->prepare("SELECT * FROM device_company WHERE (company_code = ? OR company_name = ?) AND company_id != ?");
    $check->bind_param("isi", $newKodePT, $company_name, $company_id);
    $check->execute();
    $dupResult = $check->get_result();

    if ($dupResult->num_rows > 0) {
        $row = $dupResult->fetch_assoc();
        if ($row['company_code'] === $newKodePT) {
            $msg = "Kode PT sudah digunakan!";
        } else {
            $msg = "Nama perusahaan sudah digunakan!";
        }
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Duplikasi Ditemukan!', 
                text: '$msg', 
                icon: 'warning',
                buttons: false,
                timer: 1200
            }).then(() => {
                window.location.href = './edit?company_id=$company_id';
            });
        });
        </script>";
        exit;
    }

    $conn->begin_transaction();

    try {
        $update = $conn->prepare("UPDATE device_company SET company_code = ?, updated_by = ?, company_name = ?, company_address = ?, company_email = ?, company_telephone_number = ? WHERE company_id = ?");
        $update->bind_param("iissssi", $newKodePT, $user_id, $company_name, $company_address, $company_email, $company_telephone_number, $company_id);

        if (!$update->execute()) {
            throw new Exception("Gagal update device_company: " . $update->error);
        }

        $conn->commit();

        $_SESSION['company_code'] = $newKodePT;

        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Berhasil!',
                text: 'Data perusahaan berhasil diperbarui.',
                icon: 'success',
                buttons: false,
                timer: 1200
            }).then(() => {
                window.location.href = '../companies/list';
            });
        });
        </script>";
        exit;

    } catch (Exception $e) {
        $conn->rollback();
        die("DEBUG ERROR: " . $e->getMessage());
    }
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Edit Company</title>
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
        <?php include '../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include '../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <h3 class="fw-bold mb-3">Edit Company</h3>
                    <div class="card card-round">
                        <div class="card-header">
                            <div class="card-title">Edit Company Data</div>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="idCompanies">ID Company</label>
                                    <input type="number" class="form-control" id="idCompanies" name="idCompanies"
                                        placeholder="Enter Company ID"
                                        value="<?php echo htmlspecialchars($company['company_code']); ?>">
                                </div>
                                <div class="form-group">
                                    <label for="nameCompanies">Name Company</label>
                                    <input type="text" class="form-control" id="nameCompanies" name="nameCompanies"
                                        placeholder="Enter Company Name"
                                        value="<?php echo htmlspecialchars($company['company_name']); ?>">
                                </div>
                                <div class="form-group">
                                    <label for="addressCompanies">Address Company<span class="gray-italic-text">
                                            (opsional)</span></label>
                                    <input type="text" class="form-control" id="addressCompanies"
                                        name="addressCompanies" placeholder="Enter Company Address"
                                        value="<?php echo htmlspecialchars($company['company_address']); ?>">
                                </div>
                                <div class="form-group">
                                    <label for="phoneCompanies">Phone Company<span class="gray-italic-text">
                                            (opsional)</span></label>
                                    <input type="text" class="form-control" id="phoneCompanies" name="phoneCompanies"
                                        placeholder="Enter Company Phone"
                                        value="<?php echo htmlspecialchars($company['company_telephone_number']); ?>">
                                </div>
                                <div class="form-group">
                                    <label for="emailCompanies">Email Company<span class="gray-italic-text">
                                            (opsional)</span></label>
                                    <input type="email" class="form-control" id="emailCompanies" name="emailCompanies"
                                        placeholder="Enter Company Email"
                                        value="<?php echo htmlspecialchars($company['company_email']); ?>">
                                </div>
                            </div>
                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Save Changes</button>
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