<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
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

    $stmt = $conn->prepare("UPDATE device_type SET is_deleted = 1 WHERE device_type_id = ? AND company_code = ? AND is_deleted = 0");
    $stmt->bind_param("ii", $id, $company_code);
    $stmt->execute();

    if ($stmt->affected_rows > 0) {
        echo json_encode([
            'status' => 'success',
            'message' => 'Device berhasil dihapus (soft delete).'
        ]);
    } else {
        echo json_encode([
            'status' => 'error',
            'message' => 'Device tidak ditemukan atau sudah dihapus.'
        ]);
    }
    $stmt->close();
    exit;
}
$category_id = isset($_GET['category_id']) && is_numeric($_GET['category_id']) ? intval($_GET['category_id']) : null;

if ($category_id) {
    $query = "
        SELECT ec.*,
        dc.description AS category_name
        FROM device_type ec
        JOIN device_category dc ON ec.category_id = dc.category_id
        WHERE ec.company_code = ?
        AND ec.category_id = ?
        AND ec.is_deleted = 0
        ORDER BY ec.device_type_code DESC";
    $stmt = $conn->prepare($query);
    $stmt->bind_param("ii", $company_code, $category_id);
} else {
    $query = "
        SELECT ec.*,
        dc.description AS category_name
        FROM device_type ec
        JOIN device_category dc ON ec.category_id = dc.category_id
        WHERE ec.company_code = ?
        AND ec.is_deleted = 0
        ORDER BY ec.device_type_code DESC";
    $stmt = $conn->prepare($query);
    $stmt->bind_param("i", $company_code);
}


$stmt->execute();
$result = $stmt->get_result();
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="/owl_device/">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Devices</title>
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
            width: 1%;
        }

        .lebar-kolom2 {
            width: 25%;
        }

        .lebar-kolom3 {
            width: 24%;
        }

        .lebar-kolom4 {
            width: 25%;
        }

        .lebar-kolom5 {
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
                            <h3 class="fw-bold mb-3">List Device</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/device/create">
                                        <button type="button" class="btn btn-primary btn-block"
                                            style="max-width: 200px;">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Create Device
                                        </button>
                                    </a>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableDevice"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">ID</th>
                                                    <th class="text-center lebar-kolom2">Name Device</th>
                                                    <th class="text-center lebar-kolom3">Namely</th>
                                                    <th class="text-center lebar-kolom4">Category</th>
                                                    <th class="text-center lebar-kolom5">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php
                                                if ($result->num_rows > 0) {
                                                    while ($row = $result->fetch_assoc()) {
                                                        ?>
                                                        <tr>
                                                            <td class="text-center"><?php echo $row['device_type_code']; ?></td>
                                                            <td class="text-center"><?php echo $row['device_name']; ?></td>
                                                            <td class="text-center"><?php echo $row['device_namely']; ?></td>
                                                            <td class="text-center"><?php echo $row['category_name']; ?></td>
                                                            <td class="text-center">
                                                                <a href="admin/device/edit?device_type_id=<?php echo $row['device_type_id']; ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
                                                                </a>
                                                                <a href="admin/device/detail?device_type_id=<?php echo $row['device_type_id']; ?>"
                                                                    class="btn btn-warning btn-sm">
                                                                    <i class="fas fa-info-circle"></i>
                                                                </a>
                                                                <button class="btn btn-success btn-sm generate_btn"
                                                                    data-id="<?= $row['device_type_id'] ?>"><i
                                                                        class="fas fa-file-archive"></i>
                                                                </button>
                                                                <button class="btn btn-danger btn-sm soft-delete-btn"
                                                                    data-id="<?php echo $row['device_type_id']; ?>">
                                                                    <i class="fas fa-trash"></i>
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
            $('#tableDevice').on('click', '.soft-delete-btn', function (e) {
                e.preventDefault();
                var userId = $(this).data('id');

                swal({
                    title: "Hapus Device?",
                    text: "Device akan dihapus (soft delete) dan disembunyikan dari list jadi is_deleted = 1 apabila ingin tampil is_deleted = 0.",
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
                        url: "admin/device/list",
                        method: "POST",
                        data: { soft_delete_id: userId },
                        dataType: "json",
                        success: function (res) {
                            if (res.status === "success") {
                                swal({
                                    icon: "success",
                                    title: "Berhasil!",
                                    text: res.message || "Device berhasil dihapus.",
                                    buttons: false,
                                    timer: 1200
                                }).then(function () {
                                    window.location.replace("/owl_device/admin/device/list");
                                });
                            } else {
                                swal({
                                    icon: "error",
                                    title: "Gagal!",
                                    text: res.message || "Gagal menghapus device."
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

            var table = $('#tableDevice').DataTable({
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

            $(document).on("click", ".generate_btn", function () {
                const id = $(this).data("id");

                swal({
                    title: "Generating...",
                    text: "Please wait while the firmware template is being generated.",
                    buttons: false,
                    closeOnClickOutside: false,
                    closeOnEsc: false
                });

                $.ajax({
                    url: "admin/device/generate",
                    method: "GET",
                    data: { device_type_id: id },
                    dataType: "json",

                    success: function (res) {
                        swal.close();

                        if (res.status === "success") {

                            window.location.href = res.zip;

                            swal({
                                icon: "success",
                                title: "Generate Success",
                                text: "ZIP file berhasil dibuat.",
                                buttons: false,
                                timer: 1200
                            });

                        } else {
                            swal({
                                icon: "error",
                                title: "Generate Failed",
                                text: res.message || "Unknown error"
                            });
                        }
                    },

                    error: function (xhr) {
                        swal.close();
                        swal({
                            icon: "error",
                            title: "Server Error",
                            text: xhr.responseText
                        });
                    }

                });
            });
        });
    </script>
</body>

</html>