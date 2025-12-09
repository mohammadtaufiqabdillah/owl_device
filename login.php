<?php
require_once 'config_base.php';
require_once 'connection.php';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'];
    $password = $_POST['password'];

    $stmt = $conn->prepare("SELECT * FROM device_user WHERE username = ? OR user_email = ?");
    $stmt->bind_param("ss", $username, $username);
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows === 1) {
        $user = $result->fetch_assoc();

        if (password_verify($password, $user['password'])) {
            if ($user['status'] == 1) {
                echo "
                <script src='assets/js/plugin/sweetalert/sweetalert.min.js'></script>
                <script>
                document.addEventListener('DOMContentLoaded', function() {
                    swal({
                        title: 'Akun Belum Diverifikasi!',
                        text: 'Silakan cek email kamu untuk verifikasi akun.',
                        icon: 'warning',
                        button: 'OK'
                    }).then(() => {
                        window.location = 'login';
                    });
                });
                </script>";
                exit;
            }
            $_SESSION['user_id'] = $user['user_id'];
            $_SESSION['username'] = $user['username'];
            $_SESSION['user_name'] = $user['user_name'];
            $_SESSION['user_email'] = $user['user_email'];
            $_SESSION['company_code'] = $user['company_code'];
            $_SESSION['user_profile_picture'] = $user['user_profile_picture'] ?? "assets/img/profile.jpg";
            header('Location: admin/device/list');
            exit;
        } else {
            echo "
            <script src='assets/js/plugin/sweetalert/sweetalert.min.js'></script>
            <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Password Salah!',
                    text: 'Silakan coba lagi.',
                    icon: 'error',
                    button: false,
                    timer: 1200
                });
            });
            </script>";
        }
    } else {
        echo "
        <script src='assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal('User Tidak Ditemukan!', 'Pastikan username atau email benar.', 'error');
        });
        </script>";
    }
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Login</title>
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
        .input-group .form-control,
        .input-group .input-group-text,
        .btn-primary {
            height: 45px;
            font-size: 15px;
        }

        .btn-primary {
            width: 100%;
            height: 45px;
            font-size: 15px;
            font-weight: 500;
            border-radius: 6px;
            margin-top: 2px;
        }

        .input-group-text {
            background-color: #f1f3f5;
            border: none;
        }

        form .input-group {
            margin-bottom: 12px;
        }

        form button {
            margin-top: 4px;
        }
    </style>

</head>

<body style="background-color: #e9edf2;">
    <div class="d-flex justify-content-center align-items-center" style="height: 100vh;">
        <div class="card shadow" style="width: 380px; border-radius: 12px;">
            <div class="card-header text-center bg-white border-0">
                <a href="login" class="h3 d-flex justify-content-center align-items-center gap-2"
                    style="text-decoration: none;">
                    <img src="assets/img/OWLlogo.png" alt="OWL Device" class="rounded-circle"
                        style="background-color: black; padding: 8px; opacity: .8; height: 50px; width: 50px;">
                    <strong>OWL</strong> Device
                </a>
            </div>
            <div class="card-body text-center">
                <p class="mb-4">Silahkan login terlebih dahulu</p>
                <form method="POST">
                    <div class="input-group mb-3">
                        <input type="text" class="form-control" placeholder="Username / Email" name="username" required>
                        <span class="input-group-text">
                            <i class="fas fa-user"></i>
                        </span>
                    </div>

                    <div class="input-group mb-3">
                        <input type="password" class="form-control" placeholder="Password" name="password"
                            id="passwordInput" required>
                        <span class="input-group-text" id="togglePassword" onclick="togglePassword()"
                            style="cursor: pointer;">
                            <i class="fas fa-eye"></i>
                        </span>
                    </div>

                    <button type="submit" class="btn btn-primary btn-block">Sign In</button>
                    <p class="text-center mt-3 mb-0">
                        Tidak ingat password? <a href="forgot_password">Reset Password</a>
                    </p>
                    <p class="text-center mt-3 mb-0">
                        Belum punya akun? <a href="register">Daftar Sekarang</a>
                    </p>
                </form>
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
        function togglePassword() {
            const input = document.getElementById('passwordInput');
            const icon = document.getElementById('togglePassword').querySelector('i');
            if (input.type === 'password') {
                input.type = 'text';
                icon.classList.remove('fa-eye');
                icon.classList.add('fa-eye-slash');
            } else {
                input.type = 'password';
                icon.classList.remove('fa-eye-slash');
                icon.classList.add('fa-eye');
            }
        }
    </script>
</body>

</html>