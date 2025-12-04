<?php
include "../../connection.php";

if (!isset($_SESSION['user_id'])) {
    header("Location: ../../login");
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'];
$device_type_id = $_GET['device_type_id'] ?? 0;

if (isset($_GET['action']) && isset($_GET['error_code_id'])) {
    $action = $_GET['action'];
    $error_code_id = intval($_GET['error_code_id']);

    if ($action === 'add') {
        $query = "INSERT IGNORE INTO device_vs_error (device_type_id, error_code_id, company_code)
                  VALUES ('$device_type_id', '$error_code_id', '$company_code')";
    } elseif ($action === 'remove') {
        $query = "DELETE FROM device_vs_error 
                  WHERE device_type_id = '$device_type_id' 
                    AND error_code_id = '$error_code_id' 
                    AND company_code = '$company_code'";
    }

    mysqli_query($conn, $query);

    if (mysqli_error($conn)) {
        echo "SQL Error: " . mysqli_error($conn);
    } else {
        echo "ok";
    }
    exit;
}

$qDevice = mysqli_query($conn, "SELECT device_name FROM device_type WHERE device_type_id='$device_type_id'");
$d = mysqli_fetch_assoc($qDevice);
$dName = $d ? $d['device_name'] : "Unknown Device";

$queryAll = "
    SELECT 
        e.error_code_id,
        e.code_error,
        CASE e.error_source 
            WHEN 1 THEN 'Hardware'
            WHEN 2 THEN 'Software'
            WHEN 3 THEN 'Hardware/Software'
            WHEN 4 THEN 'Network'
            ELSE 'Unknown'
        END AS source_name,
        c.description AS category_name
    FROM device_error_code e
    JOIN device_error_category c 
      ON e.error_category = c.error_category_id 
     AND e.company_code = c.company_code
    WHERE e.company_code = '$company_code'
      AND e.is_deleted = 0
    ORDER BY e.code_error ASC
";

$allErrors = mysqli_query($conn, $queryAll);

$queryLinked = "
    SELECT error_code_id
    FROM device_vs_error
    WHERE device_type_id = '$device_type_id'
      AND company_code = '$company_code'
";
$linkResult = mysqli_query($conn, $queryLinked);
$linkedIds = [];
while ($r = mysqli_fetch_assoc($linkResult)) {
    $linkedIds[] = $r['error_code_id'];
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <?php include_once dirname(__FILE__) . '/config_base.php'; ?>
    <base href="<?= $base ?>">
    <meta charset="utf-8">
    <title>Mapping Errors</title>
    <meta content='width=device-width, initial-scale=1.0, shrink-to-fit=no' name='viewport' />
    <link rel="icon" href="assets/img/OWLlogo.png" type="image/x-icon" />

    <!-- Fonts and icons -->
    <script src="assets/js/plugin/webfont/webfont.min.js"></script>
    <script>
        WebFont.load({
            google: { "families": ["Public Sans:300,400,500,600,700"] },
            custom: { "families": ["Font Awesome 5 Solid", "Font Awesome 5 Regular", "Font Awesome 5 Brands", "simple-line-icons"], urls: ['assets/css/fonts.min.css'] },
            active: function () { sessionStorage.fonts = true; }
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
                    <h3 class="fw-bold mb-3">Manage Errors for <?= htmlspecialchars($dName) ?></h3>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <button class="btn btn-secondary mb-3"
                                        onclick="window.location.href='admin/device/detail?device_type_id=<?= $device_type_id ?>'">
                                        Back
                                    </button>
                                </div>
                                <div class="card-body p-0">
                                    <table class="table table-bordered table-striped text-center">
                                        <thead>
                                            <tr>
                                                <th>Code</th>
                                                <th>Source</th>
                                                <th>Category</th>
                                                <th>Action</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <?php while ($err = mysqli_fetch_assoc($allErrors)): ?>
                                                <?php $isLinked = in_array($err['error_code_id'], $linkedIds); ?>
                                                <tr class="<?= $isLinked ? 'table-success' : '' ?>">
                                                    <td><?= htmlspecialchars($err['code_error']) ?></td>
                                                    <td><?= htmlspecialchars($err['source_name']) ?></td>
                                                    <td><?= htmlspecialchars($err['category_name']) ?></td>
                                                    <td>
                                                        <?php if ($isLinked): ?>
                                                            <button class="btn btn-danger btn-sm"
                                                                onclick="updateError('remove', <?= $err['error_code_id'] ?>)">Remove</button>
                                                        <?php else: ?>
                                                            <button class="btn btn-success btn-sm"
                                                                onclick="updateError('add', <?= $err['error_code_id'] ?>)">Add</button>
                                                        <?php endif; ?>
                                                    </td>
                                                </tr>
                                            <?php endwhile; ?>
                                        </tbody>
                                    </table>
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

    <script>
        function updateError(action, id) {
            $.get("admin/device/create_error", {
                device_type_id: <?= $device_type_id ?>,
                error_code_id: id,
                company_code: "<?= $company_code ?>",
                action: action
            }, function (res) {
                if (res.trim() === "ok") {
                    swal({
                        title: "Success!",
                        text: "Error successfully " + (action === 'add' ? "added." : "removed."),
                        icon: "success",
                        timer: 800,
                        buttons: false
                    }).then(() => location.reload());
                } else {
                    swal("Error", res, "error");
                }
            }).fail(function (xhr, status, error) {
                swal("Error", "AJAX failed: " + error, "error");
            });
        }
    </script>
</body>

</html>