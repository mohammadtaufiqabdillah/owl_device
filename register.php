<?php
require_once 'config_base.php';
include 'connection.php';
include 'config_email.php';
require 'vendor/autoload.php';
use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

$success = $error = '';

//Kalau mau local aktifin aja komen yang disebelah false terus komen falsenya hapus
$isLocal = false;  //($_SERVER['HTTP_HOST'] === 'localhost' || $_SERVER['HTTP_HOST'] === '127.0.0.1');

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = trim($_POST['username']);
    $user_email = trim($_POST['user_email']);
    $user_name = trim($_POST['user_name']);
    $password_input = $_POST['password'];
    $repassword = $_POST['repassword'];

    if (empty($username) || empty($user_email) || empty($user_name) || empty($password_input) || empty($repassword)) {
        $error = "Semua field wajib diisi!";
    } elseif (!filter_var($user_email, FILTER_VALIDATE_EMAIL)) {
        $error = "Format email tidak valid!";
    } elseif ($password_input !== $repassword) {
        $error = "Password dan Re-Password tidak sama!";
    } else {
        $password = password_hash($password_input, PASSWORD_DEFAULT);

        $stmt = $conn->prepare("SELECT user_id FROM device_user WHERE username = ? OR user_email = ?");
        $stmt->bind_param("ss", $username, $user_email);
        $stmt->execute();
        $stmt->store_result();

        if ($stmt->num_rows > 0) {
            $error = "Username atau email sudah digunakan!";
        } else {
            $token = bin2hex(random_bytes(32));
            $status = 1;

            $query = "INSERT INTO device_user (company_code, username, user_email, user_name, password, verification_token, status, created_at, updated_at)
                      VALUES (NULL, ?, ?, ?, ?, ?, ?, NOW(), NOW())";
            $stmt = $conn->prepare($query);
            $stmt->bind_param("sssssi", $username, $user_email, $user_name, $password, $token, $status);

            if ($stmt->execute()) {
                $verify_link = "http://" . $_SERVER['HTTP_HOST'] . "<?= $base ?>/verify?token=" . $token;

                if ($isLocal) {
                    $logMessage = "=== EMAIL SIMULASI (LOCAL) ===\nKepada: $user_email\nSubjek: Verifikasi Akun OWL Device\nLink: $verify_link\n\n";
                    file_put_contents(__DIR__ . '/mock_emails.log', $logMessage, FILE_APPEND);
                    $success = "Akun berhasil dibuat! (Email disimpan ke mock_emails.log)";
                } else {
                    $mail = new PHPMailer(true);
                    try {
                        $mail->isSMTP();
                        $mail->Host = 'smtp.gmail.com';
                        $mail->SMTPAuth = true;
                        $mail->Username = $email_sender;
                        $mail->Password = $email_password;
                        $mail->SMTPSecure = 'tls';
                        $mail->Port = 587;

                        $mail->setFrom($email_sender, $email_name);
                        $mail->addAddress($user_email, $user_name);
                        $mail->isHTML(true);
                        $mail->Subject = 'Verifikasi Akun OWL Device';
                        $mail->Body = "
                            <h3>Halo, $user_name!</h3>
                            <p>Terima kasih telah mendaftar di OWL Device.</p>
                            <p>Klik link berikut untuk memverifikasi akun Anda:</p>
                            <p><a href='$verify_link'>$verify_link</a></p>
                            <p>Salam,<br>OWL Device</p>
                        ";
                        $mail->send();
                        $success = "Akun berhasil dibuat! Cek email Anda untuk verifikasi.";
                    } catch (Exception $e) {
                        $error = "Gagal mengirim email verifikasi: {$mail->ErrorInfo}";
                    }
                }
            } else {
                $error = "Terjadi kesalahan saat menyimpan data.";
            }
        }
    }
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Register</title>
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
            font-weight: 500;
            border-radius: 6px;
            margin-top: 2px;
        }
    </style>
</head>

<body style="background-color: #e9edf2;">
    <div class="d-flex justify-content-center align-items-center" style="height: 100vh;">
        <div class="card shadow" style="width: 380px; border-radius: 12px;">
            <div class="card-header text-center bg-white border-0">
                <a href="register" class="h3 d-flex justify-content-center align-items-center gap-2"
                    style="text-decoration: none;">
                    <img src="assets/img/OWLlogo.png" alt="OWL Device" class="rounded-circle"
                        style="background-color: black; padding: 8px; opacity: .8; height: 50px; width: 50px;">
                    <strong>OWL</strong> Device
                </a>
            </div>
            <div class="card-body">
                <p class="login-box-msg text-center">Silahkan isi form dengan benar</p>

                <form id="registerForm" method="POST" onsubmit="return validateForm()">
                    <div class="input-group mb-3">
                        <input type="text" class="form-control" placeholder="Username" id="usernameInput"
                            name="username" required>
                    </div>
                    <div class="input-group mb-3">
                        <input type="text" class="form-control" placeholder="Email" id="emailInput" name="user_email"
                            required>
                    </div>
                    <div class="input-group mb-3">
                        <input type="text" class="form-control" placeholder="Nama Lengkap" id="namaInput"
                            name="user_name" required>
                    </div>
                    <div class="input-group mb-3">
                        <input type="password" class="form-control" placeholder="Password" id="passwordInput"
                            name="password" required>
                        <span class="input-group-text" id="togglePassword" onclick="togglePassword()"
                            style="cursor: pointer;">
                            <i class="fas fa-eye"></i>
                        </span>
                    </div>
                    <div class="input-group mb-3">
                        <input type="password" class="form-control" placeholder="Ulangi Password" id="repasswordInput"
                            name="repassword" required>
                        <span class="input-group-text" id="toggleRePassword" onclick="toggleRePassword()"
                            style="cursor: pointer;">
                            <i class="fas fa-eye"></i>
                        </span>
                    </div>

                    <button type="submit" class="btn btn-primary btn-block">Sign Up</button>
                    <p class="text-center mt-3 mb-0">
                        Sudah punya akun? <a href="login">Login Sekarang</a>
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

        function toggleRePassword() {
            const input = document.getElementById('repasswordInput');
            const icon = document.getElementById('toggleRePassword').querySelector('i');
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

        function validateForm() {
            const pass = document.getElementById('passwordInput').value.trim();
            const repass = document.getElementById('repasswordInput').value.trim();

            if (pass !== repass) {
                swal({
                    title: "Gagal!",
                    text: "Password dan Re-Password tidak sama!",
                    icon: "error",
                    button: "OK"
                }).then(() => {
                    document.getElementById('registerForm').reset();
                });
                return false;
            }

            return true;
        }

        document.addEventListener('DOMContentLoaded', function () {
            <?php if (!empty($success)): ?>
                swal({
                    title: "Berhasil!",
                    text: <?= json_encode($success) ?>,
                    icon: "success",
                    buttons: false,
                    timer: 2000
                }).then(() => {
                    window.location.href = "login";
                });
            <?php elseif (!empty($error)): ?>
                swal({
                    title: "Gagal!",
                    text: <?= json_encode($error) ?>,
                    icon: "error",
                    button: "OK"
                }).then(() => {
                    document.getElementById('registerForm').reset();
                });
            <?php endif; ?>
        });
    </script>
</body>

</html>