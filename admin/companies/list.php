<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$company_code = $_SESSION['company_code'];

$stmt = $conn->prepare("SELECT * FROM device_company WHERE company_code = ? ORDER BY company_code DESC");
$stmt->bind_param("i", $company_code);
$stmt->execute();
$result = $stmt->get_result();
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <?php include_once dirname(__FILE__) . '/config_base.php'; ?>
    <base href="<?= $base ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Companies</title>
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

        .lebar-kolom4 {
            width: 15%;
        }

        .lebar-kolom5 {
            width: 15%;
        }

        .lebar-kolom6 {
            width: 15%;
        }
    </style>
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
                    <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                        <div>
                            <h3 class="fw-bold mb-3">List Company</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <div class="card card-round">
                                <div class="card-header align-center">
                                    <a href="admin/companies/create" class="mb-2 mb-md-0">
                                        <button type="button" class="btn btn-primary" style="max-width: 200px;">
                                            <i class="fas fa-plus" style="margin-right: 8px;"></i>Create Company
                                        </button>
                                    </a>
                                    <a href="admin/companies/join" class="mb-2 mb-md-0">
                                        <button type="button" class="btn btn-secondary" style="max-width: 200px;">
                                            <i class="fas fa-user-plus" style="margin-right: 8px;"></i>Join Company
                                        </button>
                                    </a>
                                    <div id="tableCompanies_length" class="dataTables_length"></div>
                                </div>
                                <div class="card-body p-0">
                                    <div class="table-responsive card-padding">
                                        <table id="tableCompanies"
                                            class="table table order-list table-striped table-bordered">
                                            <thead>
                                                <tr>
                                                    <th class="text-center lebar-kolom1">Id</th>
                                                    <th class="text-center lebar-kolom2">Company Name</th>
                                                    <th class="text-center lebar-kolom3">Address</th>
                                                    <th class="text-center lebar-kolom4">Phone</th>
                                                    <th class="text-center lebar-kolom5">Email</th>
                                                    <th class="text-center lebar-kolom6">Action</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <?php
                                                if ($result->num_rows > 0) {
                                                    while ($row = $result->fetch_assoc()) {
                                                        ?>
                                                        <tr>
                                                            <td class="text-center"><?php echo $row['company_code']; ?></td>
                                                            <td class="text-center"><?php echo $row['company_name']; ?></td>
                                                            <td class="text-center"><?php echo $row['company_address']; ?></td>
                                                            <td class="text-center">
                                                                <?php echo $row['company_telephone_number']; ?>
                                                            </td>
                                                            <td class="text-center"><?php echo $row['company_email']; ?></td>
                                                            <td class="text-center">
                                                                <a href="admin/companies/edit?company_id=<?php echo $row['company_id']; ?>"
                                                                    class="btn btn-info btn-sm">
                                                                    <i class="fas fa-edit"></i>
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
            $('#tableCompanies').on('click', '.delete-btn', function (e) {
                e.preventDefault();
                var userId = $(this).data('id');

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
                        window.location.href = 'delete.php?id=' + userId;
                    }
                });
            });

            var table = $('#tableCompanies').DataTable({
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