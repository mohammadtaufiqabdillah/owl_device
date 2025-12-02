<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];
$device_type_id = isset($_GET['device_type_id']) ? intval($_GET['device_type_id']) : 0;

$stmt = $conn->prepare("
    SELECT 
        dt.*, 
        cat.description AS category_name,
        comp.company_name
    FROM device_type dt
    JOIN device_category cat 
        ON dt.category_id = cat.category_id 
       AND dt.company_code = cat.company_code
    JOIN device_company comp 
        ON dt.company_code = comp.company_code
    WHERE dt.device_type_id = ? 
      AND dt.company_code = ?
");

$stmt->bind_param("ii", $device_type_id, $company_code);
$stmt->execute();
$result = $stmt->get_result();
$device = $result->fetch_assoc();

if (!$device) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Data tidak ditemukan',
                text: 'Device Type tidak ditemukan atau bukan milik perusahaan Anda.',
                icon: 'error',
                buttons: false,
                timer: 1200
            }).then(() => {
                window.location.href = '../device/list';
            });
        });
    </script>";
    exit;
}

$queryDevice = "
    SELECT 
        dt.device_type_id, dt.device_name, dt.device_namely,
        dt.device_firmware, dt.device_firmware_build,
        dt.device_hardware, dt.device_hardware_build,
        dt.device_image, dt.device_otaurl, dt.description,
        cat.category_code, cat.description AS category_name,
        comp.company_name
    FROM device_type AS dt
    JOIN device_category AS cat ON dt.category_id = cat.category_id
    JOIN device_company AS comp ON dt.company_code = comp.company_code
    WHERE dt.device_type_id = '$device_type_id' 
      AND dt.company_code = '$company_code'
";

$deviceResult = mysqli_query($conn, $queryDevice);
$device = mysqli_fetch_assoc($deviceResult);
$dName = $device ? $device['device_name'] : 'Unknown';

$queryCommand = "
    SELECT 
        cmd.command_id,
        cmd.command_code,
        cmd.command_name,
        cmd.command_type,
        cmd.description
    FROM device_vs_command AS dvc
    JOIN device_command AS cmd 
      ON dvc.command_id = cmd.command_id 
     AND dvc.company_code = cmd.company_code
    WHERE dvc.device_type_id = '$device_type_id'
      AND dvc.company_code = '$company_code'
      AND cmd.is_deleted = 0
    ORDER BY cmd.command_type, cmd.command_code ASC
";
$result = mysqli_query($conn, $queryCommand);

$queryError = "
    SELECT 
        err.error_code_id,
        err.code_error,
        CASE err.error_source 
            WHEN 1 THEN 'Hardware'
            WHEN 2 THEN 'Software'
            WHEN 3 THEN 'Hardware/Software'
            WHEN 4 THEN 'Network'
            ELSE 'Unknown'
        END AS source_name,
        err_cat.description AS category_name,
        err.description AS error_description
    FROM device_vs_error AS dve
    JOIN device_error_code AS err 
      ON dve.error_code_id = err.error_code_id 
     AND dve.company_code = err.company_code
    JOIN device_error_category AS err_cat 
      ON err.error_category = err_cat.error_category_id 
     AND err.company_code = err_cat.company_code
    WHERE dve.device_type_id = '$device_type_id'
      AND dve.company_code = '$company_code'
      AND err.is_deleted = 0
    ORDER BY err.code_error ASC
";
$resultError = mysqli_query($conn, $queryError);

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="/owl_device/">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Device Detail</title>
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
        .lebar-kolom1 {
            width: 25%;
        }

        .lebar-kolom2 {
            width: 25%;
        }

        .lebar-kolom3 {
            width: 25%;
        }

        .lebar-kolom4 {
            width: 25%;
        }
    </style>
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
                            <h3 class="fw-bold mb-3">Device Detail - <?= htmlspecialchars($dName) ?></h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <h4>Device Information</h4>
                                </div>
                                <div class="card-body p-4">
                                    <?php if ($device): ?>
                                        <table class="table table-borderless">
                                            <tr>
                                                <td><b>ID:</b></td>
                                                <td><?= htmlspecialchars($device['device_type_id']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Name:</b></td>
                                                <td><?= htmlspecialchars($device['device_name']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Category ID:</b></td>
                                                <td><?= htmlspecialchars($device['category_code']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Category Name:</b></td>
                                                <td><?= htmlspecialchars($device['category_name']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Namely:</b></td>
                                                <td><?= htmlspecialchars($device['device_namely']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Firmware:</b></td>
                                                <td><?= htmlspecialchars($device['device_firmware']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Firmware Build:</b></td>
                                                <td><?= htmlspecialchars($device['device_firmware_build']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Hardware:</b></td>
                                                <td><?= htmlspecialchars($device['device_hardware']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Hardware Build:</b></td>
                                                <td><?= htmlspecialchars($device['device_hardware_build']) ?></td>
                                            </tr>
                                            <tr>
                                                <td><b>Image:</b></td>
                                                <td>
                                                    <?php
                                                    $imgVal = $device['device_image'] ?? '';
                                                    $imgSrc = '';
                                                    if ($imgVal) {

                                                        if (stripos($imgVal, 'http://') === 0 || stripos($imgVal, 'https://') === 0) {
                                                            $imgSrc = $imgVal;
                                                        } elseif (strpos($imgVal, 'assets/upload/device/') === 0) {
                                                            $imgSrc = $imgVal;
                                                        } else {
                                                            $imgSrc = 'assets/upload/device/' . $imgVal;
                                                        }
                                                    }
                                                    ?>
                                                    <?php if ($imgSrc && file_exists($_SERVER['DOCUMENT_ROOT'] . '/' . ltrim($imgSrc, '/'))): ?>
                                                        <img src="<?= htmlspecialchars($imgSrc) ?>" alt="Device Image"
                                                            style="max-width:180px; height:auto; border-radius:8px; border:1px solid #ddd;">
                                                        <br>
                                                        <a href="<?= htmlspecialchars($imgSrc) ?>" download
                                                            class="btn btn-sm btn-primary mt-2">Download Image</a>
                                                    <?php elseif ($imgSrc): ?>
                                                        <!-- file path exists in DB but physical file may be missing; still show link -->
                                                        <img src="<?= htmlspecialchars($imgSrc) ?>" alt="Device Image"
                                                            style="max-width:180px; height:auto; border-radius:8px; border:1px solid #ddd;">
                                                        <br>
                                                    <?php else: ?>
                                                        <span class="text-muted">No image uploaded</span>
                                                    <?php endif; ?>
                                                </td>
                                            </tr>
                                            <tr>
                                                <td><b>OTA:</b></td>
                                                <td>
                                                    <?php
                                                    $otaVal = $device['device_otaurl'] ?? '';
                                                    $otaSrc = '';
                                                    if ($otaVal) {
                                                        if (stripos($otaVal, 'http://') === 0 || stripos($otaVal, 'https://') === 0) {
                                                            $otaSrc = $otaVal;
                                                        } elseif (strpos($otaVal, 'assets/upload/ota/') === 0) {
                                                            $otaSrc = $otaVal;
                                                        } else {
                                                            $otaSrc = 'assets/upload/ota/' . $otaVal;
                                                        }
                                                    }
                                                    ?>
                                                    <?php if ($otaSrc && file_exists($_SERVER['DOCUMENT_ROOT'] . '/' . ltrim($otaSrc, '/'))): ?>
                                                        <a href="<?= htmlspecialchars($otaSrc) ?>"
                                                            download><?= htmlspecialchars(basename($otaSrc)) ?></a>
                                                    <?php elseif ($otaSrc): ?>
                                                        <a href="<?= htmlspecialchars($otaSrc) ?>"
                                                            target="_blank"><?= htmlspecialchars(basename($otaSrc)) ?></a>
                                                    <?php else: ?>
                                                        <span class="text-muted">No OTA file uploaded</span>
                                                    <?php endif; ?>
                                                </td>
                                            </tr>

                                            <tr>
                                                <td><b>Description:</b></td>
                                                <td><?= htmlspecialchars($device['description']) ?></td>
                                            </tr>
                                        </table>
                                    <?php else: ?>
                                        <p class="text-danger">Device not found.</p>
                                    <?php endif; ?>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">Mapped Commands</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/device/create_command?device_type_id=<?= $device_type_id ?>">
                                        <button type="button" class="btn btn-primary btn-block px-4 py-2.8">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Add/Remove Command
                                        </button>
                                    </a>
                                    <button type="button" class="btn btn-secondary"
                                        onclick="window.location.href='admin/device/list'">Back</button>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableCommandDatDetail"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">Type</th>
                                                    <th class="text-center lebar-kolom2">Command ID</th>
                                                    <th class="text-center lebar-kolom3">Command Name</th>
                                                    <th class="text-center lebar-kolom4">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php if (mysqli_num_rows($result) > 0): ?>
                                                    <?php while ($row = mysqli_fetch_assoc($result)): ?>
                                                        <tr>
                                                            <td class="text-center"><?= strtoupper($row['command_type']) ?></td>
                                                            <td class="text-center">
                                                                <?= htmlspecialchars($row['command_code']) ?>
                                                            </td>
                                                            <td class="text-center">
                                                                <?= htmlspecialchars($row['command_name']) ?>
                                                            </td>
                                                            <td class="text-center  ">
                                                                <a href="admin/command/<?= strtolower($row['command_type']) ?>/detail/list?command_id=<?= $row['command_id'] ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
                                                                </a>
                                                            </td>
                                                        </tr>
                                                    <?php endwhile; ?>
                                                <?php endif; ?>
                                            </tbody>
                                        </table>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">Mapped Errors</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/device/create_error?device_type_id=<?= $device_type_id ?>">
                                        <button type="button" class="btn btn-primary btn-block px-4 py-2.8">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Add/Remove Error
                                        </button>
                                    </a>
                                    <button type="button" class="btn btn-secondary"
                                        onclick="window.location.href='admin/device/list'">Back</button>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableCommandDatDetail"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">Code</th>
                                                    <th class="text-center lebar-kolom2">Source</th>
                                                    <th class="text-center lebar-kolom3">Category</th>
                                                    <th class="text-center lebar-kolom4">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php if (mysqli_num_rows($resultError) > 0): ?>
                                                    <?php while ($row = mysqli_fetch_assoc($resultError)): ?>
                                                        <tr>
                                                            <td class="text-center"><?= htmlspecialchars($row['code_error']) ?>
                                                            </td>
                                                            <td class="text-center">
                                                                <?= htmlspecialchars($row['source_name']) ?>
                                                            </td>
                                                            <td class="text-center">
                                                                <?= htmlspecialchars($row['category_name']) ?>
                                                            </td>
                                                            <td class="text-center  ">
                                                                <a href="admin/error_code/edit?error_code_id=<?= $row['error_code_id'] ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
                                                                </a>
                                                            </td>
                                                        </tr>
                                                    <?php endwhile; ?>
                                                <?php endif; ?>
                                            </tbody>
                                        </table>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>

                </div> <!-- page-inner -->
            </div> <!-- container -->
        </div> <!-- main-panel -->
    </div> <!-- wrapper -->

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