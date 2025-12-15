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

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['hard_delete_id'])) {
    header('Content-Type: application/json');

    $detail_id = intval($_POST['hard_delete_id']);
    if ($detail_id <= 0) {
        echo json_encode([
            'status' => 'error',
            'message' => 'Command Detail ID tidak valid.'
        ]);
        exit;
    }
    $delstmt = $conn->prepare("
        DELETE dcd
        FROM device_command_detail dcd
        INNER JOIN device_command dc ON dc.command_id = dcd.command_id
        WHERE dcd.command_detail_id = ? AND dc.company_code = ?
    ");
    $delstmt->bind_param("ii", $detail_id, $company_code);
    $delstmt->execute();

    if ($delstmt->affected_rows > 0) {
        echo json_encode([
            'status' => 'success',
            'message' => 'Command detail berhasil dihapus.'
        ]);
    } else {
        echo json_encode([
            'status' => 'error',
            'message' => 'Gagal menghapus command detail.'
        ]);
    }
    exit;
}

$stmt = $conn->prepare("SELECT * FROM device_command WHERE command_id = ? AND company_code = ?");
$stmt->bind_param("ii", $command_id, $company_code);
$stmt->execute();
$command_dat = $stmt->get_result()->fetch_assoc();

if (!$command_dat) {
    echo "<script src='../../../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Data tidak ditemukan',
                text: 'Command detail tidak ditemukan atau bukan milik perusahaan Anda.',
                icon: 'error',
                buttons: false,
                timer: 1200
            }).then(() => {
                window.location.href = '../list';
            });
        });
    </script>";
    exit;
}

$stmt2 = $conn->prepare("
    SELECT dcd.*
    FROM device_command_detail dcd
    INNER JOIN device_command dc ON dc.command_id = dcd.command_id
    WHERE dcd.command_id = ? AND dc.company_code = ?
    ORDER BY dcd.command_detail_id ASC
");
$stmt2->bind_param("ii", $command_id, $company_code);
$stmt2->execute();
$result = $stmt2->get_result();

$datName = $command_dat['command_name'];

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Create Command Dat Detail</title>
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
            width: 20%;
        }

        .lebar-kolom2 {
            width: 20%;
        }

        .lebar-kolom3 {
            width: 20%;
        }

        .lebar-kolom4 {
            width: 20%;
        }

        .lebar-kolom5 {
            width: 20%;
        }
    </style>
</head>

<body>
    <div class="wrapper">
        <?php include '../../../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include '../../../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">dat :: <?= $datName ?></h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/command/dat/detail/create?command_id=<?= $command_id ?>">
                                        <button type="button" class="btn btn-primary btn-block px-4 py-2.8">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Create New Command
                                        </button>
                                    </a>
                                    <button type="button" class="btn btn-secondary"
                                        onclick="window.location.href='admin/command/dat/list'">Back</button>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableCommandDatDetail"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">Data Name</th>
                                                    <th class="text-center lebar-kolom2">Data Type</th>
                                                    <th class="text-center lebar-kolom3">Data Length (bytes)</th>
                                                    <th class="text-center lebar-kolom4">Default Value</th>
                                                    <th class="text-center lebar-kolom5">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php
                                                $totalBytes = 0;
                                                if ($result->num_rows > 0) {
                                                    while ($row = mysqli_fetch_assoc($result)) {
                                                        $totalBytes += (int) $row['data_len'];
                                                        ?>
                                                        <tr>
                                                            <td class="text-center"><?php echo $row['data_name']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['data_type']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['data_len']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['data_default']; ?>
                                                            </td>
                                                            <td class="text-center">
                                                                <a href="admin/command/dat/detail/edit?command_id=<?= $command_id ?>&command_detail_id=<?= $row['command_detail_id'] ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
                                                                </a>
                                                                <button class="btn btn-danger btn-sm delete-btn"
                                                                    data-id="<?php echo $row['command_detail_id']; ?>">
                                                                    <i class="fas fa-trash-alt"></i>
                                                                </button>
                                                            </td>
                                                        </tr>
                                                        <?php
                                                    }
                                                } else {
                                                    echo '<tr><td colspan="5" class="text-center">No Data Found</td></tr>';
                                                }
                                                ?>
                                            </tbody>
                                            <tfoot>
                                                <tr>
                                                    <th class="text-center">Total</th>
                                                    <th></th>
                                                    <th class="text-center"><?= $totalBytes ?> bytes</th>
                                                    <th></th>
                                                    <th></th>
                                                </tr>
                                            </tfoot>
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
            $('#tableCommandDatDetail').on('click', '.delete-btn', function (e) {
                e.preventDefault();
                var detailId = $(this).data('id');

                swal({
                    title: "Hapus Command Detail?",
                    text: "Command Detail akan dihapus selamanya.",
                    icon: "warning",
                    buttons: {
                        cancel: {
                            text: "Batal",
                            visible: true,
                            className: "btn btn-secondary"
                        },
                        confirm: {
                            text: "Ya, Hapus",
                            className: "btn btn-danger"
                        }
                    },
                    dangerMode: true,
                }).then(function (willDelete) {
                    if (!willDelete) return;

                    $.ajax({
                        url: 'admin/command/dat/detail/list?command_id=<?= $command_id ?>',
                        type: 'POST',
                        data: { hard_delete_id: detailId },
                        dataType: 'json',
                        success: function (res) {
                            if (res.status === "success") {
                                swal({
                                    icon: "success",
                                    title: "Berhasil!",
                                    text: res.message || "Command Detail berhasil dihapus.",
                                    buttons: false,
                                    timer: 1200
                                }).then(function () {
                                    window.location.replace("admin/command/dat/detail/list?command_id=<?= $command_id ?>");
                                });
                            } else {
                                swal({
                                    icon: "error",
                                    title: "Gagal!",
                                    text: res.message || "Gagal menghapus command detail."
                                });
                            }
                        },
                        error: function (xhr) {
                            swal({
                                icon: "error",
                                title: "Server Error",
                                text: xhr.responseText || "Terjadi kesalahan pada server."
                            });
                        }
                    });
                });
            });
        });
    </script>
</body>

</html>