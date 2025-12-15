<?php
require_once '../../config_base.php';
require_once '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$userData = $conn->query("SELECT company_code FROM device_user WHERE user_id = $user_id")->fetch_assoc();
$companyCodeUser = $userData['company_code'] ?? null;

if (empty($companyCodeUser)) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Akses Ditolak',
                text: 'Kamu belum tergabung di PT manapun. Silakan bergabung atau buat PT terlebih dahulu.',
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

$ptStmt = $conn->prepare("SELECT created_by FROM device_company WHERE company_code = ? ");
$ptStmt->bind_param("i", $companyCodeUser);
$ptStmt->execute();
$result = $ptStmt->get_result();
$ptRes = $result->fetch_assoc();

$creator = intval($ptRes['created_by'] ?? 0);

if ($creator != $user_id) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Akses Ditolak',
                text: 'Hanya pembuat PT yang dapat mengelola daftar anggota.',
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

$ReqStmt = $conn->prepare("SELECT  user_id, user_name, company_request_code FROM device_user WHERE company_request_code = ? ORDER BY user_id ASC");
$ReqStmt->bind_param("i", $companyCodeUser);
$ReqStmt->execute();
$result = $ReqStmt->get_result();
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>List Join Company</title>
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
            width: 5%;
        }

        .lebar-kolom2 {
            width: 25%;
        }

        .lebar-kolom3 {
            width: 25%;
        }
    </style>
</head>

<body>
    <div class="wrapper">
        <?php include '../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include '../../includes/navbar.php'; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">List Join Company</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableListJoinCompany"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">User Id</th>
                                                    <th class="text-center lebar-kolom2">Name User</th>
                                                    <th class="text-center lebar-kolom3">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php
                                                if ($result->num_rows > 0) {
                                                    while ($row = $result->fetch_assoc()) {
                                                        $uid = intval($row['user_id']);
                                                        $uname = htmlspecialchars($row['user_name'] ?? '—');
                                                        ?>
                                                        <tr id="req-row-<?php echo $uid; ?>">
                                                            <td class="text-center"><?php echo $uid; ?></td>
                                                            <td class="text-center"><?php echo $uname; ?></td>
                                                            <td class="text-center">
                                                                <button type="button" class="btn btn-success btn-sm approve-btn"
                                                                    data-id="<?php echo $uid; ?>">
                                                                    <i class="fas fa-check-circle"></i>
                                                                </button>
                                                                <button type="button" class="btn btn-danger btn-sm reject-btn"
                                                                    data-id="<?php echo $uid; ?>">
                                                                    <i class="fas fa-window-close"></i>
                                                                </button>
                                                            </td>
                                                        </tr>
                                                        <?php
                                                    }
                                                } else {
                                                    echo '<tr><td colspan="3" class="text-center">Tidak ada permintaan bergabung.</td></tr>';
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
            $('#tableListJoinCompany').DataTable({
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

            $(document).on("click", ".approve-btn", function () {
                var userId = $(this).data("id");
                swal({
                    title: "Approve Permintaan?",
                    text: "Pengguna akan ditambahkan ke dalam PT ini.",
                    icon: "warning",
                    buttons: ['Batal', 'Ya, Approve!'],
                    dangerMode: false,
                }).then(function (confirmed) {
                    if (confirmed) {
                        $.post("./admin/companies/approve", { user_id: userId }).done(function (res) {
                            try {
                                var response = (typeof res === 'object') ? res : JSON.parse(res);
                                if (response.success) {
                                    $('#req-row-' + userId).remove();
                                    swal({
                                        title: "Berhasil",
                                        text: "Permintaan bergabung disetujui.",
                                        icon: "success",
                                        buttons: false,
                                        timer: 1200
                                    }).then(() => {
                                        location.reload();
                                    });
                                } else {
                                    swal({
                                        title: "Gagal",
                                        text: response.message || "Terjadi kesalahan saat menyetujui permintaan.",
                                        icon: "error",
                                        buttons: false,
                                        timer: 1200
                                    });
                                }
                            } catch (e) {
                                swal({
                                    title: "Gagal",
                                    text: "Respon tidak valid dari server.",
                                    icon: "error",
                                    buttons: false,
                                    timer: 1200
                                });
                            }
                        });
                    }
                });
            });

            $(document).on("click", ".reject-btn", function () {
                var userId = $(this).data("id");
                swal({
                    title: "Tolak Permintaan?",
                    text: "Pengguna tidak akan ditambahkan ke dalam PT ini.",
                    icon: "warning",
                    buttons: ['Batal', 'Ya, Tolak!'],
                    dangerMode: true,
                }).then(function (confirmed) {
                    if (confirmed) {
                        $.post("./admin/companies/reject", { user_id: userId }).done(function (res) {
                            try {
                                var response = (typeof res === 'object') ? res : JSON.parse(res);
                                if (response.success) {
                                    $('#req-row-' + userId).remove();
                                    swal({
                                        title: "Berhasil",
                                        text: "Permintaan bergabung ditolak.",
                                        icon: "success",
                                        buttons: false,
                                        timer: 1200
                                    }).then(() => {
                                        location.reload();
                                    });
                                } else {
                                    swal({
                                        title: "Gagal",
                                        text: response.message || "Terjadi kesalahan saat menolak permintaan.",
                                        icon: "error",
                                        buttons: false,
                                        timer: 1200
                                    });
                                }
                            } catch (e) {
                                swal({
                                    title: "Gagal",
                                    text: "Respon tidak valid dari server.",
                                    icon: "error",
                                    buttons: false,
                                    timer: 1200
                                });
                            }
                        });
                    }
                });
            });
        });
    </script>
</body>

</html>