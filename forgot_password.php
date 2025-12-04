<?php
include 'connection.php';
include 'config_email.php';
require 'vendor/autoload.php';
use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

$success = $error = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $user_email = trim($_POST['email']);

    $stmt = $conn->prepare("SELECT  user_id , user_name FROM device_user WHERE user_email = ?");
    $stmt->bind_param("s", $user_email);
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows === 1) {
        $user = $result->fetch_assoc();
        $user_id = $user['user_id'];
        $user_name = $user['user_name'];

        function generate_password($length = 10)
        {
            $chars = 'ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz23456789!@#$%*-';
            $pw = '';
            $max = strlen($chars) - 1;
            for ($i = 0; $i < $length; $i++) {
                $pw .= $chars[random_int(0, $max)];
            }
            return $pw;
        }
        $new_password = generate_password(10);
        $new_password_hash = password_hash($new_password, PASSWORD_DEFAULT);

        $u = $conn->prepare("UPDATE device_user SET password = ? WHERE user_id = ?");
        $u->bind_param("si", $new_password_hash, $user_id);
        $updated = $u->execute();

        if ($updated) {
            $mail = new PHPMailer(true);
            try {
                $mail->isSMTP();
                $mail->Host = 'smtp.gmail.com';
                $mail->SMTPAuth = true;
                $mail->Username = $email_sender;
                $mail->Password = $email_password;
                $mail->SMTPSecure = PHPMailer::ENCRYPTION_STARTTLS;
                $mail->Port = 587;

                $mail->setFrom($email_sender, $email_name);
                $mail->addAddress($user_email, $user_name);

                $mail->isHTML(true);
                $mail->Subject = 'Reset Password - OWL Device';
                $mail->Body = "
                        <div style='font-family:arial,sans-serif;line-height:1.4'>
                          <h3>Halo {$user_name},</h3>
                          <p>Kami telah mereset password akun Anda. Silakan gunakan password baru di bawah ini untuk masuk, lalu segera ubah password di pengaturan akun:</p>
                          <p style='font-size:16px;padding:10px;background:#f4f4f4;border-radius:6px;display:inline-block'>
                            <strong>{$new_password}</strong>
                          </p>
                          <p>Catatan keamanan: setelah login, segera ganti password dengan yang mudah Anda ingat namun kuat.</p>
                          <p>Jika Anda tidak meminta perubahan ini, segera hubungi admin.</p>
                          <hr>
                          <p style='font-size:12px;color:#666'>OWL Device</p>
                        </div>
                    ";

                $mail->send();

                $success = 'Password baru telah dikirim ke email Anda. Silakan cek inbox (atau folder spam).';
            } catch (Exception $e) {
                $error = 'Gagal mengirim email. Silakan coba lagi nanti atau hubungi admin.';
                error_log("PHPMailer error on forgot_password: " . $mail->ErrorInfo);
            }
        } else {
            $error = 'Gagal menyimpan password ke database. Silakan coba lagi nanti.';
        }
    } else {

        $success = 'Jika email terdaftar, password baru telah dikirimkan.';
    }
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <?php include_once dirname(__FILE__) . '/config_base.php'; ?>
    <base href="<?=  $base ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Forgot Password</title>
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
        .btn-primary {
            width: 100%;
            height: 45px;
            font-size: 15px;
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
                <a href="forgot_password" class="h3 d-flex justify-content-center align-items-center gap-2"
                    style="text-decoration: none;">
                    <img src="assets/img/OWLlogo.png" alt="OWL Device" class="rounded-circle"
                        style="background-color: black; padding: 8px; opacity: .8; height: 50px; width: 50px;">
                    <strong>OWL</strong> Device
                </a>
            </div>
            <div class="card-body text-center">
                <p class="mb-4">Silahkan masukkan email terlebih dahulu</p>
                <form method="POST" id="forgotPassword">
                    <div class="input-group mb-3">
                        <input type="email" class="form-control" placeholder="Masukkan Email" name="email" required>
                        <span class="input-group-text">
                            <i class="fas fa-envelope"></i>
                        </span>
                    </div>
                    <button type="submit" class="btn btn-primary btn-block">Kirim Password Baru</button>
                    <p class="text-center mt-3 mb-0">
                        Sudah ingat passwordnya? <a href="login">Kembali ke Login</a>
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
                    document.getElementById('forgotPassword').reset();
                });
            <?php endif; ?>
        });
    </script>
</body>

</html>