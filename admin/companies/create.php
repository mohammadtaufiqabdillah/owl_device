<?php
include '../../connection.php';

if (empty($_SESSION['user_id'])) {
    echo "<script>
        window.location.href = '/owl_device/login';
    </script>";
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
            title: 'Tidak Bisa Membuat PT Baru!',
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
    $company_name = trim($_POST['nameCompanies']);
    $company_code = trim($_POST['codeCompanies']);
    $company_address = trim($_POST['addressCompanies']) ?: null;
    $company_telephone_number = trim($_POST['phoneCompanies']) ?: null;
    $company_email = trim($_POST['emailCompanies']) ?: null;
    $company_token = bin2hex(random_bytes(8));

    $check = $conn->prepare("SELECT * FROM device_company WHERE company_code = ? OR company_name = ?");
    $check->bind_param("is", $company_code, $company_name);
    $check->execute();
    $checkResult = $check->get_result();

    if ($checkResult->num_rows > 0) {
        $row = $checkResult->fetch_assoc();
        if ($row["company_code"] === $company_code) {
            $msg = "Kode PT sudah digunakan!";
        } else {
            $msg = "Nama perusahaan sudah digunakan!";
        }
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Tidak Bisa Membuat PT!',
                    text: '$msg',
                    icon: 'warning',
                    buttons: false,
                    timer: 1200
                }).then(() => window.location.replace('./create'));
            });
        </script>";
        exit;
    }

    $stmt = $conn->prepare("INSERT INTO device_company (company_code, company_name, company_address, company_email, company_telephone_number, company_token, updated_by) VALUES (?, ?, ?, ?, ?, ?, ?)");
    $stmt->bind_param("isssssi", $company_code, $company_name, $company_address, $company_email, $company_telephone_number, $company_token, $userId);

    if ($stmt->execute()) {
        $update = $conn->prepare("UPDATE device_user SET company_code = ? WHERE user_id = ?");
        $update->bind_param("ii", $company_code, $userId);
        $update->execute();

        $_SESSION['company_code'] = $company_code;

        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'PT Berhasil Dibuat!',
                text: 'Token PT kamu: $company_token',
                icon: 'success',
                button: 'OK'
            }).then(() => window.location = './admin/companies/list');
        });
        </script>";
    } else {
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal('Gagal!', 'Terjadi kesalahan saat membuat PT.', 'error');
        });
        </script>";
        exit;
    }
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="/owl_device/">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Create Company</title>
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
        <?php include $_SERVER['DOCUMENT_ROOT'] . "/owl_device/includes/sidebar.php"; ?>
        <div class="main-panel">
            <?php include $_SERVER['DOCUMENT_ROOT'] . "/owl_device/includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <h3 class="fw-bold mb-3">Create Company</h3>
                    <div class="card card-round">
                        <div class="card-header">
                            <div class="card-title">Add Company Data</div>
                        </div>
                        <form method="POST">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="codeCompanies">ID Company</label>
                                    <input type="number" class="form-control" id="codeCompanies" name="codeCompanies"
                                        placeholder="Enter Company ID" required>
                                </div>
                                <div class="form-group">
                                    <label for="nameCompanies">Name Company</label>
                                    <input type="text" class="form-control" id="nameCompanies" name="nameCompanies"
                                        placeholder="Enter Company Name" required>
                                </div>
                                <div class="form-group">
                                    <label for="addressCompanies">Address Company<span class="gray-italic-text">
                                            (opsional)</span></label>
                                    <input type="text" class="form-control" id="addressCompanies"
                                        name="addressCompanies" placeholder="Enter Company Address">
                                </div>
                                <div class="form-group">
                                    <label for="phoneCompanies">Phone Company<span class="gray-italic-text">
                                            (opsional)</span></label>
                                    <input type="text" class="form-control" id="phoneCompanies" name="phoneCompanies"
                                        placeholder="Enter Company Phone">
                                </div>
                                <div class="form-group">
                                    <label for="emailCompanies">Email Company<span class="gray-italic-text">
                                            (opsional)</span></label>
                                    <input type="email" class="form-control" id="emailCompanies" name="emailCompanies"
                                        placeholder="Enter Company Email">
                                </div>
                            </div>
                            <div class="card-footer d-flex justify-content-end gap-3">
                                <button type="submit" class="btn btn-primary">Create</button>
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