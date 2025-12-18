<?php
require_once '../../../config_base.php';
require_once '../../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../../login');
    exit;
}

$company_code = $_SESSION['company_code'];

$stmtList = $conn->prepare(" SELECT * FROM device_command WHERE company_code = ? AND command_type = 'res' AND is_deleted = 0 ORDER BY command_code ASC ");
$stmtList->bind_param("i", $company_code);
$stmtList->execute();
$result = $stmtList->get_result();
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Commands Res</title>
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
            width: 4%;
        }

        .lebar-kolom2 {
            width: 20%;
        }

        .lebar-kolom3 {
            width: 20%;
        }

        .lebar-kolom4 {
            width: 30%;
        }

        .lebar-kolom5 {
            width: 1%;
        }

        .lebar-kolom6 {
            width: 25%;
        }
    </style>
</head>

<body>
    <div class="wrapper">
        <?php include __DIR__ . '/../../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include __DIR__ . '/../../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">List Command Res</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableCommandRes"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">Id</th>
                                                    <th class="text-center lebar-kolom2">Command Name</th>
                                                    <th class="text-center lebar-kolom3">Custom Class</th>
                                                    <th class="text-center lebar-kolom4">Description</th>
                                                    <th class="text-center lebar-kolom5">Is Main</th>
                                                    <th class="text-center lebar-kolom6">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php
                                                if ($result->num_rows > 0) {
                                                    while ($row = mysqli_fetch_assoc($result)) {
                                                        ?>
                                                        <tr>
                                                            <td class="text-center"><?php echo $row['command_code']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['command_name']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['custom_class']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['description']; ?></td>
                                                            <td class="text-center">
                                                                <?php echo $row['is_main'] ? '<span class="badge bg-success">Yes</span>' : '<span class="badge bg-secondary">No</span>'; ?>
                                                            </td>
                                                            <td class="text-center">
                                                                <a href="admin/command/res/detail/list?command_id=<?php echo $row['command_id']; ?>"
                                                                    class="btn btn-warning btn-sm">
                                                                    <i class="fas fa-info-circle"></i>
                                                            </td>
                                                        </tr>
                                                        <?php
                                                    }
                                                }
                                                ?>
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
        $(document).ready(function () {
            var table = $('#tableCommandRes').DataTable({
                pageLength: 10,
                lengthMenu: [
                    [10, 25, 50, -1],
                    ['10', '25', '50', 'All']
                ],
                language: {
                    search: "Search: _INPUT_",
                    searchPlaceholder: "Search...",
                    info: "Showing _START_ to _END_ of _TOTAL_ entries",
                    lengthMenu: "Show _MENU_ entries",
                    zeroRecords: "No matching records found",
                },
                order: [[0, 'asc']],
                responsive: true,
            });
        });
    </script>
</body>

</html>