<?php
include '../../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../../login');
    exit;
}

$company_code = $_SESSION['company_code'];

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['soft_delete_id'])) {
    header('Content-Type: application/json');

    $id = intval($_POST['soft_delete_id']);
    if ($id <= 0) {
        echo json_encode([
            'status' => 'error',
            'message' => 'ID tidak valid.'
        ]);
        exit;
    }

    $stmt = $conn->prepare("UPDATE device_command SET is_deleted = 1 WHERE command_id = ? AND company_code = ? AND is_deleted = 0");
    $stmt->bind_param("ii", $id, $company_code);
    $stmt->execute();

    if ($stmt->affected_rows > 0) {
        echo json_encode([
            'status' => 'success',
            'message' => 'Command Exe berhasil dihapus (soft delete).'
        ]);
    } else {
        echo json_encode([
            'status' => 'error',
            'message' => 'Command Exe tidak ditemukan atau sudah dihapus.'
        ]);
    }
    $stmt->close();
    exit;
}
$stmt = $conn->prepare("SELECT * FROM device_command WHERE company_code = ? AND command_type = 'exe' AND is_deleted = 0 ORDER BY command_code DESC");
$stmt->bind_param("i", $company_code);
$stmt->execute();
$result = $stmt->get_result();

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="/owl_device/">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Command Exe</title>
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
        <?php $rootPath = $_SERVER['DOCUMENT_ROOT'];
        include $rootPath . "/owl_device/includes/sidebar.php"; ?>
        <div class="main-panel">
            <?php $rootPath = $_SERVER['DOCUMENT_ROOT'];
            include $rootPath . "/owl_device/includes/navbar.php"; ?>
            <div class="container">
                <div class="page-inner">
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">List Command Exe</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/command/exe/create">
                                        <button type="button" class="btn btn-primary btn-block px-4 py-2.8">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Create Command
                                            Exe
                                        </button>
                                    </a>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableCommandExe"
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
                                                                <a href="admin/command/exe/edit?command_id=<?php echo $row['command_id']; ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
                                                                </a>
                                                                <a href="admin/command/exe/detail/list?command_id=<?php echo $row['command_id']; ?>"
                                                                    class="btn btn-warning btn-sm">
                                                                    <i class="fas fa-info-circle"></i>
                                                                </a>
                                                                <button class="btn btn-danger btn-sm soft-delete-btn"
                                                                    data-id="<?php echo $row['command_id']; ?>">
                                                                    <i class="fas fa-trash-alt"></i>
                                                                </button>
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
            $('#tableCommandExe').on('click', '.soft-delete-btn', function (e) {
                e.preventDefault();
                var userId = $(this).data('id');

                swal({
                    title: "Hapus Command Exe?",
                    text: "Command Exe akan dihapus (soft delete) dan disembunyikan dari list jadi is_deleted = 1 apabila ingin tampil is_deleted = 0.",
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
                        url: "admin/command/exe/list",
                        method: "POST",
                        data: { soft_delete_id: userId },
                        dataType: "json",
                        success: function (res) {
                            if (res.status === "success") {
                                swal({
                                    icon: "success",
                                    title: "Berhasil!",
                                    text: res.message || "Command Exe berhasil dihapus.",
                                    buttons: false,
                                    timer: 1200
                                }).then(function () {
                                    window.location.replace("/owl_device/admin/command/exe/list");
                                });
                            } else {
                                swal({
                                    icon: "error",
                                    title: "Gagal!",
                                    text: res.message || "Gagal menghapus Command Exe."
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

            var table = $('#tableCommandExe').DataTable({
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
                order: [[0, 'desc']],
                responsive: true,
            });

            $("table.order-list").on("click", ".ibtnEdit", function (event) {
                var idToEdit = 123;
                window.location.href = 'edit.php?id=' + idToEdit;
            });

        });
    </script>
</body>

</html>