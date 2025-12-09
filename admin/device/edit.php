<?php
require_once '../../config_base.php';
require_once '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'] ?? null;

if (!isset($_GET['device_type_id'])) {
    header('Location: ../device/list');
    exit;
}

$device_type_id = intval($_GET['device_type_id']);

$deviceUploadDir = __DIR__ . '/../../assets/upload/device/';
$otaUploadDir = __DIR__ . '/../../assets/upload/ota/';
if (!is_dir($deviceUploadDir))
    mkdir($deviceUploadDir, 0755, true);
if (!is_dir($otaUploadDir))
    mkdir($otaUploadDir, 0755, true);

$deviceMaxSize = 5 * 1024 * 1024; // 5MB
$otaMaxSize = 50 * 1024 * 1024;   // 50MB
$allowedImageExt = ['jpg', 'jpeg', 'png', 'gif', 'webp'];
$allowedOtaExt = ['bin', 'zip', 'gz', 'tar', 'rar'];

$stmt = $conn->prepare("
    SELECT dt.*, cat.description AS category_name
    FROM device_type dt
    LEFT JOIN device_category cat ON dt.category_id = cat.category_id
    WHERE dt.device_type_id = ? AND dt.company_code = ? AND dt.is_deleted = 0
");
$stmt->bind_param("ii", $device_type_id, $company_code);
$stmt->execute();
$result = $stmt->get_result();
$device_type = $result->fetch_assoc();

if (!$device_type) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Data tidak ditemukan',
                text: 'Device Type yang dimaksud tidak tersedia.',
                icon: 'error',
                button: false,
                timer: 1200,
            }).then(() => {
                window.location.href = '../device/list';
            });
        });
    </script>";
    exit;
}

$success = $error = '';

function safe_basename($name)
{
    return preg_replace('/[^A-Za-z0-9._-]/', '_', $name);
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $device_name = trim($_POST['device_name'] ?? '');
    $category_id = intval($_POST['category_id'] ?? 0);
    $device_namely = trim($_POST['device_namely'] ?? '');
    $device_hardware = trim($_POST['device_hardware'] ?? '');
    $device_firmware = trim($_POST['device_firmware'] ?? '');
    $device_hardware_build = trim($_POST['device_hardware_build'] ?? '');
    $device_firmware_build = trim($_POST['device_firmware_build'] ?? '');
    $description = trim($_POST['description'] ?? '');

    $device_image_path = $device_type['device_image'];
    $device_ota_path = $device_type['device_otaurl'];

    if ($device_name === '' || $category_id <= 0 || $device_namely === '') {
        $error = "Nama device, kategori, dan 'namely' wajib diisi.";
    }

    if (!$error) {
        $chk = $conn->prepare("SELECT 1 FROM device_type WHERE device_name COLLATE utf8mb4_general_ci = ? AND company_code = ? AND device_type_id != ? LIMIT 1");
        $chk->bind_param("sii", $device_name, $company_code, $device_type_id);
        $chk->execute();
        if ($chk->get_result()->num_rows > 0) {
            $error = "Nama device sudah digunakan oleh device lain (termasuk yang dihapus).";
        }
    }

    if (!$error) {
        if (!empty($_FILES['device_image_file']) && $_FILES['device_image_file']['error'] !== UPLOAD_ERR_NO_FILE) {
            $f = $_FILES['device_image_file'];

            if ($f['error'] !== UPLOAD_ERR_OK) {
                $error = "Gagal upload device image.";
            } else {
                $ext = strtolower(pathinfo($f['name'], PATHINFO_EXTENSION));
                if (!in_array($ext, $allowedImageExt)) {
                    $error = "Ekstensi device image tidak diizinkan.";
                } else {
                    $newName = 'device_' . $company_code . '_' . time() . '_' . rand(1000, 9999) . '.' . $ext;
                    $target = $deviceUploadDir . $newName;

                    if (!move_uploaded_file($f['tmp_name'], $target)) {
                        $error = "Gagal menyimpan device image.";
                    } else {
                        $device_image_path = 'assets/upload/device/' . $newName;

                        $oldImage = $device_type['device_image'] ?? '';
                        if ($oldImage && strpos($oldImage, 'assets/upload/device/') === 0) {

                            $oldFull = realpath(__DIR__ . '/../../' . $oldImage);

                            if ($oldFull && file_exists($oldFull)) {
                                @unlink($oldFull);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!$error) {
        if (!empty($_FILES['device_ota_file']) && $_FILES['device_ota_file']['error'] !== UPLOAD_ERR_NO_FILE) {
            $f = $_FILES['device_ota_file'];

            if ($f['error'] !== UPLOAD_ERR_OK) {
                $error = "Gagal upload OTA.";
            } else {
                $ext = strtolower(pathinfo($f['name'], PATHINFO_EXTENSION));
                if (!in_array($ext, $allowedOtaExt)) {
                    $error = "Ekstensi OTA tidak diizinkan.";
                } else {
                    $newName = 'ota_' . $company_code . '_' . time() . '_' . rand(1000, 9999) . '.' . $ext;
                    $target = $otaUploadDir . $newName;

                    if (!move_uploaded_file($f['tmp_name'], $target)) {
                        $error = "Gagal menyimpan OTA.";
                    } else {
                        $device_ota_path = 'assets/upload/ota/' . $newName;

                        $oldOta = $device_type['device_otaurl'] ?? '';
                        if ($oldOta && strpos($oldOta, 'assets/upload/ota/') === 0) {

                            $oldFull = realpath(__DIR__ . '/../../' . $oldOta);

                            if ($oldFull && file_exists($oldFull)) {
                                @unlink($oldFull);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!$error) {
        $update = $conn->prepare("
            UPDATE device_type
            SET category_id = ?, device_name = ?, device_namely = ?, device_hardware = ?, device_firmware = ?, device_hardware_build = ?, device_firmware_build = ?, device_otaurl = ?, device_image = ?, description = ?, updated_by = ?
            WHERE device_type_id = ? AND company_code = ?
        ");
        if (!$update) {
            $error = "Prepare update gagal: " . $conn->error;
        } else {
            $update->bind_param(
                "isssssssssiii",
                $category_id,
                $device_name,
                $device_namely,
                $device_hardware,
                $device_firmware,
                $device_hardware_build,
                $device_firmware_build,
                $device_ota_path,
                $device_image_path,
                $description,
                $user_id,
                $device_type_id,
                $company_code
            );

            if ($update->execute()) {
                $success = "Device berhasil diperbarui.";
            } else {
                if (isset($newName) && isset($target) && file_exists($target)) {
                    @unlink($target);
                }
                $error = "Terjadi kesalahan saat menyimpan data: " . $update->error;
            }
        }
    }

    if ($success) {
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Berhasil',
                text: '" . addslashes($success) . "',
                icon: 'success',
                button: false,
                timer: 1300
            }).then(() => { window.location.href = '../device/list'; });
        });
        </script>";
        exit;
    } elseif ($error) {
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal('Gagal!', '" . addslashes($error) . "', 'error');
        });
        </script>";
    }
    $stmt = $conn->prepare("SELECT * FROM device_type WHERE device_type_id = ? AND company_code = ?");
    $stmt->bind_param("ii", $device_type_id, $company_code);
    $stmt->execute();
    $device_type = $stmt->get_result()->fetch_assoc();
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <base href="<?php echo BASE_URL; ?>">
    <meta charset="utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Edit Device</title>
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
        #previewDeviceImage {
            max-width: 250px;
            max-height: 180px;
            display: block;
            border-radius: 10px;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.12);
            object-fit: contain;
        }

        .preview-box {
            width: 100%;
            height: 240px;
            border: 2px dashed #ced4da;
            border-radius: 12px;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-top: 10px;
            background: #f8f9fa;
            overflow: hidden;
            padding: 10px;
        }

        .small-muted {
            font-size: 0.85rem;
            color: #6c757d;
        }
    </style>
</head>

<body>
    <div class="wrapper">
        <?php include __DIR__ . '/../../includes/sidebar.php'; ?>
        <div class="main-panel">
            <?php include __DIR__ . '/../../includes/navbar.php'; ?>
            <div class="content">
                <div class="page-inner">
                    <div class="d-flex align-items-left flex-column pt-2 pb-4">
                        <h3 class="fw-bold mb-3">Edit Device</h3>
                    </div>

                    <div class="card card-round">
                        <div class="card-header">
                            <div class="card-title">Edit Device Data</div>
                        </div>

                        <form method="POST" enctype="multipart/form-data">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="device_name">Device Name</label>
                                    <input type="text" class="form-control" id="device_name" name="device_name"
                                        value="<?= htmlspecialchars($device_type['device_name'] ?? '') ?>" required>
                                </div>

                                <div class="form-group">
                                    <label for="category_id">Category</label>
                                    <select name="category_id" id="category_id" class="form-control" required>
                                        <option value="">-- Select Category --</option>
                                        <?php
                                        $res = $conn->prepare("SELECT category_id, description FROM device_category WHERE company_code = ?");
                                        $res->bind_param("i", $company_code);
                                        $res->execute();
                                        $cats = $res->get_result();
                                        while ($cat = $cats->fetch_assoc()) {
                                            $sel = ($cat['category_id'] == $device_type['category_id']) ? 'selected' : '';
                                            echo "<option value='" . intval($cat['category_id']) . "' $sel>" . htmlspecialchars($cat['description']) . "</option>";
                                        }
                                        ?>
                                    </select>
                                </div>

                                <div class="form-group">
                                    <label for="device_namely">Device Namely</label>
                                    <input type="text" class="form-control" id="device_namely" name="device_namely"
                                        value="<?= htmlspecialchars($device_type['device_namely'] ?? '') ?>" required>
                                </div>

                                <div class="form-group">
                                    <label for="device_hardware">Device Hardware</label>
                                    <input type="text" class="form-control" id="device_hardware" name="device_hardware"
                                        value="<?= htmlspecialchars($device_type['device_hardware'] ?? '') ?>" required>
                                </div>

                                <div class="form-group">
                                    <label for="device_firmware">Device Firmware</label>
                                    <input type="text" class="form-control" id="device_firmware" name="device_firmware"
                                        value="<?= htmlspecialchars($device_type['device_firmware'] ?? '') ?>" required>
                                </div>

                                <div class="form-group">
                                    <label for="device_hardware_build">Device Hardware Build</label>
                                    <input type="date" class="form-control" id="device_hardware_build"
                                        name="device_hardware_build"
                                        value="<?= htmlspecialchars($device_type['device_hardware_build'] ?? '') ?>"
                                        required>
                                </div>

                                <div class="form-group">
                                    <label for="device_firmware_build">Device Firmware Build</label>
                                    <input type="date" class="form-control" id="device_firmware_build"
                                        name="device_firmware_build"
                                        value="<?= htmlspecialchars($device_type['device_firmware_build'] ?? '') ?>"
                                        required>
                                </div>

                                <div class="form-group">
                                    <label for="deviceImageFile">Device Image (optional)</label>
                                    <input type="file" name="device_image_file" id="deviceImageFile"
                                        class="form-control" accept=".jpg,.jpeg,.png,.webp,.gif">
                                    <div class="preview-box" id="imagePreviewBox">
                                        <?php
                                        $currentImage = $device_type['device_image'] ?? '';
                                        $previewSrc = '';
                                        if ($currentImage) {
                                            if (strpos($currentImage, 'http://') === 0 || strpos($currentImage, 'https://') === 0) {
                                                $previewSrc = $currentImage;
                                            } elseif (strpos($currentImage, 'assets/upload/device/') === 0) {
                                                $previewSrc = $currentImage;
                                            } else {
                                                $previewSrc = 'assets/upload/device/' . $currentImage;
                                            }
                                        }
                                        if ($previewSrc) {
                                            $previewFullPath = $_SERVER['DOCUMENT_ROOT'] . '/' . $previewSrc;
                                            $v = (file_exists($previewFullPath) ? filemtime($previewFullPath) : time());
                                            echo '<img id="previewDeviceImage" src="' . htmlspecialchars($previewSrc) . '?v=' . $v . '" style="max-height:100%; max-width:100%; object-fit:contain;">';
                                        } else {
                                            echo '<span id="previewPlaceholder" style="color:#999;">Preview Image</span><img id="previewDeviceImage" style="display:none;">';
                                        }
                                        ?>
                                    </div>

                                    <?php if ($currentImage): ?>
                                        <div class="mt-2">
                                            <strong>Current File:</strong>
                                            <?= htmlspecialchars(basename($currentImage)) ?>
                                        </div>
                                    <?php endif; ?>
                                </div>

                                <div class="form-group">
                                    <label for="deviceOtaFile">OTA File</label>
                                    <input type="file" name="device_ota_file" id="deviceOtaFile" class="form-control"
                                        accept=".bin,.zip,.tar,.gz,.rar">
                                    <div class="mt-2">
                                        <?php
                                        $currentOta = $device_type['device_otaurl'] ?? '';
                                        if ($currentOta) {
                                            echo '<strong>Current OTA File:</strong> ' . htmlspecialchars(basename($currentOta));
                                        } else {
                                            echo '<span style="color:#999;">No OTA file available</span>';
                                        }
                                        ?>
                                    </div>
                                </div>

                                <div class="form-group">
                                    <label for="description">Description</label>
                                    <textarea class="form-control" id="description" name="description" rows="3"
                                        required><?= htmlspecialchars($device_type['description'] ?? '') ?></textarea>
                                </div>
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
        (function () {
            const fileInput = document.getElementById('deviceImageFile');
            const previewImg = document.getElementById('previewDeviceImage');
            const placeholder = document.getElementById('previewPlaceholder');

            const originalSrc = previewImg ? previewImg.src : '';

            fileInput.addEventListener('change', function (e) {
                const f = e.target.files && e.target.files[0];

                if (!f) {
                    if (originalSrc && originalSrc !== window.location.href) {
                        previewImg.src = originalSrc;
                        previewImg.style.display = 'block';
                        if (placeholder) placeholder.style.display = 'none';
                    } else {
                        if (previewImg) previewImg.style.display = 'none';
                        if (placeholder) placeholder.style.display = 'block';
                    }
                    return;
                }

                if (!f.type.startsWith('image/')) {
                    alert('File harus berupa gambar.');
                    fileInput.value = '';
                    return;
                }

                const reader = new FileReader();
                reader.onload = function (ev) {
                    if (previewImg) {
                        previewImg.src = ev.target.result;
                        previewImg.style.display = 'block';
                    }
                    if (placeholder) placeholder.style.display = 'none';
                };
                reader.readAsDataURL(f);
            });
        })();
    </script>
</body>

</html>