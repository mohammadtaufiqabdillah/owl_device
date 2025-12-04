<?php
$rootPath = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
$projectBase = $rootPath . '/owl_device';

include $projectBase . '/connection.php';

if (!isset($_SESSION['user_id'])) {
    header("Location: /owl_device/login");
    exit;
}

$user_id = intval($_SESSION['user_id']);
$company_code = isset($_SESSION['company_code']) ? intval($_SESSION['company_code']) : 0;

$uploadDirRel = "assets/upload/profile/";
$uploadDir = $projectBase . '/' . $uploadDirRel;
if (!is_dir($uploadDir)) {
    mkdir($uploadDir, 0755, true);
}

$allowedExt = ["jpg", "jpeg", "png", "gif", "webp"];
$maxImageSize = 2 * 1024 * 1024; // 2MB

$stmt = $conn->prepare("SELECT * FROM device_user WHERE user_id = ?");
$stmt->bind_param("i", $user_id);
$stmt->execute();
$user = $stmt->get_result()->fetch_assoc();

if (!$user) {
    die("User tidak ditemukan.");
}

$error = "";
$success = "";

if ($_SERVER["REQUEST_METHOD"] === "POST") {

    $user_name = trim($_POST['user_name'] ?? '');
    $username = trim($_POST['username'] ?? '');
    $user_email = trim($_POST['user_email'] ?? '');
    $new_password = trim($_POST['password'] ?? '');
    $current_password = trim($_POST['current_password'] ?? '');

    $newProfileRel = $user['user_profile_picture'] ?? '';

    if ($user_name === "" || $username === "" || $user_email === "") {
        $error = "Semua field wajib diisi kecuali password.";
    }

    if (!$error && !filter_var($user_email, FILTER_VALIDATE_EMAIL)) {
        $error = "Format email tidak valid.";
    }

    if (!$error) {
        $chk = $conn->prepare("SELECT user_id FROM device_user WHERE username = ? AND user_id != ?");
        $chk->bind_param("si", $username, $user_id);
        $chk->execute();
        if ($chk->get_result()->num_rows > 0) {
            $error = "Username sudah digunakan.";
        }
    }

    if (!$error) {
        $chk2 = $conn->prepare("SELECT user_id FROM device_user WHERE user_email = ? AND user_id != ?");
        $chk2->bind_param("si", $user_email, $user_id);
        $chk2->execute();
        if ($chk2->get_result()->num_rows > 0) {
            $error = "Email sudah digunakan.";
        }
    }

    if (!$error && !empty($_FILES['user_profile_picture']['name'])) {
        $file = $_FILES['user_profile_picture'];
        $ext = strtolower(pathinfo($file['name'], PATHINFO_EXTENSION));

        if (!in_array($ext, $allowedExt)) {
            $error = "Format gambar tidak diizinkan. (jpg,jpeg,png,gif,webp)";
        } elseif ($file['size'] > $maxImageSize) {
            $error = "Ukuran gambar maksimal 2MB.";
        } else {
            $newName = "user_" . $user_id . "_" . time() . "." . $ext;
            $target = $uploadDir . $newName;

            if (move_uploaded_file($file['tmp_name'], $target)) {
                if (!empty($user['user_profile_picture'])) {
                    $old = $projectBase . '/' . ltrim($user['user_profile_picture'], '/');
                    if (file_exists($old) && strpos($user['user_profile_picture'], $uploadDirRel) === 0) {
                        @unlink($old);
                    }
                }
                $newProfileRel = $uploadDirRel . $newName;
            } else {
                $error = "Gagal menyimpan file gambar.";
            }
        }
    }

    $changePassword = false;
    if (!$error && $new_password !== "") {
        if ($current_password === "") {
            $error = "Masukkan password lama untuk mengganti password.";
        } elseif (!password_verify($current_password, $user['password'])) {
            $error = "Password lama tidak cocok.";
        } else {
            $changePassword = true;
        }
    }

    if (!$error) {
        if ($changePassword) {
            $hashed = password_hash($new_password, PASSWORD_DEFAULT);
            $sql = "UPDATE device_user 
                    SET user_name=?, username=?, user_email=?, password=?, user_profile_picture=?, updated_at=NOW()
                    WHERE user_id=? ";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("sssssi", $user_name, $username, $user_email, $hashed, $newProfileRel, $user_id);
        } else {
            $sql = "UPDATE device_user 
                    SET user_name=?, username=?, user_email=?, user_profile_picture=?, updated_at=NOW()
                    WHERE user_id=? ";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("ssssi", $user_name, $username, $user_email, $newProfileRel, $user_id);
        }

        if ($stmt->execute()) {
            $_SESSION['user_name'] = $user_name;
            $_SESSION['username'] = $username;
            $_SESSION['user_email'] = $user_email;
            $_SESSION['user_profile_picture'] = $newProfileRel;

            if ($changePassword) {
                session_unset();
                session_destroy();

                echo "<script src='<?= $base ?>assets/js/plugin/sweetalert/sweetalert.min.js'></script>
                <script>
                    document.addEventListener('DOMContentLoaded', function() {
                        swal({
                            title: 'Berhasil!',
                            text: 'Password telah diubah. Silakan login kembali.',
                            icon: 'success',
                            buttons: false,
                            timer: 1200
                        }).then(function(){ window.location.href = '<?= $base ?>login'; });
                    });
                </script>";
                exit;
            }
            echo "<script src='<?= $base ?>assets/js/plugin/sweetalert/sweetalert.min.js'></script>
            <script>
                document.addEventListener('DOMContentLoaded', function() {
                    swal({
                        title: 'Berhasil!',
                        text: 'Profil berhasil diperbarui.',
                        icon: 'success',
                        buttons: false,
                        timer: 1200
                    }).then(function(){ window.location.href = '<?= $base ?>admin/device/list'; });
                });
            </script>";
            exit;
        } else {
            $error = "Gagal memperbarui data: " . $stmt->error;
        }
    }
}

$stmt = $conn->prepare("SELECT * FROM device_user WHERE user_id = ?");
$stmt->bind_param("i", $user_id);
$stmt->execute();
$user = $stmt->get_result()->fetch_assoc();

$profileSrc = !empty($user['user_profile_picture']) ? $user['user_profile_picture'] : "assets/img/profile.jpg";
?>

<!DOCTYPE html>
<html lang="id">

<head>
    <?php include_once dirname(__FILE__) . '/config_base.php'; ?>
    <base href="<?= $base ?>">
    <meta charset="utf-8">
    <title>Account Setting</title>
    <meta content='width=device-width, initial-scale=1.0, shrink-to-fit=no' name='viewport' />
    <link rel="icon" href="assets/img/OWLlogo.png" type="image/x-icon" />

    <!-- Fonts and icons -->
    <script src="assets/js/plugin/webfont/webfont.min.js"></script>
    <script>
        WebFont.load({
            google: { "families": ["Public Sans:300,400,500,600,700"] },
            custom: { "families": ["Font Awesome 5 Solid", "Font Awesome 5 Regular", "Font Awesome 5 Brands", "simple-line-icons"], urls: ['assets/css/fonts.min.css'] },
            active: function () { sessionStorage.fonts = true; }
        });
    </script>

    <!-- CSS Files -->
    <link rel="stylesheet" href="assets/css/bootstrap.min.css">
    <link rel="stylesheet" href="assets/css/plugins.min.css">
    <link rel="stylesheet" href="assets/css/kaiadmin.min.css">

    <style>
        /* gambar profil bulat dan crop */
        .profile-img {
            width: 120px;
            height: 120px;
            border-radius: 50%;
            object-fit: cover;
            display: inline-block;
            border: 3px solid #fff;
        }

        .profile-preview {
            display: flex;
            align-items: center;
            gap: 12px;
        }

        .btn-eye {
            cursor: pointer;
            user-select: none;
        }
    </style>
</head>

<body>
    <?php include $_SERVER['DOCUMENT_ROOT'] . "<?= $base ?>includes/sidebar.php"; ?>
    <div class="main-panel">
        <?php include $_SERVER['DOCUMENT_ROOT'] . "<?= $base ?>includes/navbar.php"; ?>

        <div class="container">
            <div class="page-inner">
                <div class="d-flex align-items-left align-items-md-center flex-column flex-md-row pt-2 pb-4">
                    <div>
                        <h3 class="fw-bold mb-3">Account Setting</h3>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="card card-round">
                            <div class="card-header align-center">
                                <?php if ($error): ?>
                                    <div class="alert alert-danger"><?= htmlspecialchars($error) ?></div>
                                <?php endif; ?>

                                <?php if (!empty($success)): ?>
                                    <div class="alert alert-success"><?= htmlspecialchars($success) ?></div>
                                <?php endif; ?>

                                <form method="POST" enctype="multipart/form-data" id="accountForm">
                                    <div class="mb-3 profile-preview">
                                        <div>
                                            <?php $imgSrc = $profileSrc; ?>
                                            <img src="<?= $imgSrc ?>" alt="Profile" class="profile-img"
                                                id="previewProfile">
                                        </div>
                                        <div style="min-width:240px;">
                                            <label class="form-label">Profile Picture</label>
                                            <input type="file" name="user_profile_picture" accept="image/*"
                                                class="form-control" id="user_profile_picture">
                                            <small class="text-muted">Kosongkan jika tidak ingin mengganti.</small>
                                        </div>
                                    </div>

                                    <div class="mb-3">
                                        <label>Full Name</label>
                                        <input class="form-control" name="user_name" required
                                            value="<?= htmlspecialchars($user['user_name'] ?? '') ?>">
                                    </div>

                                    <div class="mb-3">
                                        <label>Email</label>
                                        <input class="form-control" name="user_email" type="email" required
                                            value="<?= htmlspecialchars($user['user_email'] ?? '') ?>">
                                    </div>

                                    <div class="mb-3">
                                        <label>Username</label>
                                        <input class="form-control" name="username" required
                                            value="<?= htmlspecialchars($user['username'] ?? '') ?>">
                                    </div>

                                    <div class="mb-3">
                                        <label>Password Lama (wajib jika ingin ganti password)</label>
                                        <div class="input-group">
                                            <input type="password" class="form-control" name="current_password"
                                                id="current_password"
                                                placeholder="Masukkan password lama jika ingin mengganti">
                                            <span class="input-group-text btn-eye"
                                                onclick="togglePassword('current_password')">
                                                <i class="fa fa-eye" id="icon_current_password"></i>
                                            </span>
                                        </div>
                                    </div>

                                    <div class="mb-3">
                                        <label>Password Baru (kosongkan jika tidak ingin ubah)</label>
                                        <div class="input-group">
                                            <input type="password" class="form-control" name="password" id="password"
                                                placeholder="Password baru">
                                            <span class="input-group-text btn-eye" onclick="togglePassword('password')">
                                                <i class="fa fa-eye" id="icon_password"></i>
                                            </span>
                                        </div>
                                        <small class="text-muted">Kosongkan jika tidak ingin mengganti password.</small>
                                    </div>

                                    <div class="card-footer d-flex justify-content-end gap-3">
                                        <button type="submit" class="btn btn-primary">Submit</button>
                                        <button type="button" class="btn btn-secondary"
                                            onclick="window.location.href='admin/device/list'">Cancel</button>
                                    </div>
                                </form>

                            </div>
                        </div>
                    </div>
                </div>

            </div>
        </div>
    </div>

    <!-- Core JS -->
    <script src="assets/js/core/jquery-3.7.1.min.js"></script>
    <script src="assets/js/core/popper.min.js"></script>
    <script src="assets/js/core/bootstrap.min.js"></script>

    <!-- jQuery Scrollbar -->
    <script src="assets/js/plugin/jquery-scrollbar/jquery.scrollbar.min.js"></script>

    <!-- SweetAlert -->
    <script src="assets/js/plugin/sweetalert/sweetalert.min.js"></script>

    <!-- Kaiadmin JS -->
    <script src="assets/js/kaiadmin.min.js"></script>

    <script>
        (function () {
            const inp = document.getElementById('user_profile_picture');
            const img = document.getElementById('previewProfile');
            if (!inp) return;
            inp.addEventListener('change', function (e) {
                const f = e.target.files && e.target.files[0];
                if (!f) return;
                if (!f.type.startsWith('image/')) {
                    swal('Gagal', 'File harus berupa gambar.', 'error');
                    inp.value = '';
                    return;
                }
                const reader = new FileReader();
                reader.onload = function (ev) { img.src = ev.target.result; };
                reader.readAsDataURL(f);
            });
        })();

        function togglePassword(id) {
            const el = document.getElementById(id);
            if (!el) return;
            const icon = document.getElementById('icon_' + id);
            if (el.type === 'password') {
                el.type = 'text';
                if (icon) { icon.classList.remove('fa-eye'); icon.classList.add('fa-eye-slash'); }
            } else {
                el.type = 'password';
                if (icon) { icon.classList.remove('fa-eye-slash'); icon.classList.add('fa-eye'); }
            }
        }
    </script>

    <?php if ($error): ?>
        <script>
            document.addEventListener('DOMContentLoaded', function () {
                swal({
                    title: "Gagal",
                    text: <?= json_encode($error) ?>,
                    icon: "error",
                    buttons: false,
                    timer: 1200
                });
            });
        </script>
    <?php endif; ?>

</body>

</html>