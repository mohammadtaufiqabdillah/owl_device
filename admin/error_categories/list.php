<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$company_code = $_SESSION['company_code'];

$stmt = $conn->prepare("SELECT * FROM device_error_category WHERE company_code = ? ORDER BY error_category_code DESC");
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
    <title>Error Categories</title>
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
            width: 10%;
        }

        .lebar-kolom2 {
            width: 70%;
        }

        .lebar-kolom3 {
            width: 20%;
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
                            <h3 class="fw-bold mb-3">List Error Category</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/error_categories/create">
                                        <button type="button" class="btn btn-primary btn-block px-4 py-2.8">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Create Error Category
                                        </button>
                                    </a>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableErrorCategories"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">Id</th>
                                                    <th class="text-center lebar-kolom2">Error Category Name</th>
                                                    <th class="text-center lebar-kolom3">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php
                                                if ($result->num_rows > 0) {
                                                    while ($row = mysqli_fetch_assoc($result)) {
                                                        ?>
                                                        <tr>
                                                            <td class="text-center"><?php echo $row['error_category_code']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['description']; ?>
                                                            </td>
                                                            <td class="text-center">
                                                                <a href="admin/error_categories/edit?error_category_id=<?php echo $row['error_category_id']; ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
                                                                </a>
                                                                <a href="admin/error_code/list?error_category_id=<?php echo $row['error_category_id']; ?>"
                                                                    class="btn btn-warning btn-sm">
                                                                    <i class="fas fa-info-circle"></i>
                                                                </a>
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
            $('#tableErrorCategories').on('click', '.delete-btn', function (e) {
                e.preventDefault();
                var commandId = $(this).data('id');

                Swal.fire({
                    title: 'Apakah Anda yakin?',
                    text: 'Data pengguna ini akan dihapus!',
                    icon: 'warning',
                    showCancelButton: true,
                    confirmButtonColor: '#d33',
                    cancelButtonColor: '#3085d6',
                    confirmButtonText: 'Ya, hapus!',
                    cancelButtonText: 'Batal'
                }).then((result) => {
                    if (result.isConfirmed) {
                        window.location.href = 'delete.php?id=' + commandId;
                    }
                });
            });

            var table = $('#tableErrorCategories').DataTable({
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