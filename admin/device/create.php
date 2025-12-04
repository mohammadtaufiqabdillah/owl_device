<?php
include '../../connection.php';

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$user_id = $_SESSION['user_id'];
$company_code = $_SESSION['company_code'] ?? null;

$success = $error = '';

$deviceUploadDir = __DIR__ . '/../../assets/upload/device/';
$otaUploadDir = __DIR__ . '/../../assets/upload/ota/';
$deviceMaxSize = 5 * 1024 * 1024;
$otaMaxSize = 50 * 1024 * 1024;
$allowedImageExt = ['jpg', 'jpeg', 'png', 'gif', 'webp'];
$allowedOtaExt = ['bin', 'zip', 'gz', 'tar', 'rar'];

if (!is_dir($deviceUploadDir)) {
    mkdir($deviceUploadDir, 0755, true);
}
if (!is_dir($otaUploadDir)) {
    mkdir($otaUploadDir, 0755, true);
}

function safe_basename($name)
{
    $name = preg_replace('/[^A-Za-z0-9._-]/', '_', $name);
    return $name;
}

if (!$company_code) {
    echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            swal({
                title: 'Tidak Bisa Membuat Device!',
                text: 'Kamu belum tergabung atau belum membuat perusahaan (PT).',
                icon: 'warning',
                button: false,
                timer: 1200,
            }).then(() => window.location.replace('../companies/list'));
        });
    </script>";
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $device_name = trim($_POST['deviceName'] ?? '');
    $category_id = intval($_POST['category_id'] ?? 0);
    $device_namely = trim($_POST['deviceNamely'] ?? '');
    $device_hardware = trim($_POST['deviceHardware'] ?? '');
    $device_firmware = trim($_POST['deviceFirmware'] ?? '');
    $device_hardware_build = trim($_POST['deviceHardwareBuild'] ?? '');
    $device_firmware_build = trim($_POST['deviceFirmwareBuild'] ?? '');
    $description = trim($_POST['descriptionDevice'] ?? '');
    $deviceImagePath = null;
    $otaFilePath = null;

    if ($device_name === '' || $category_id <= 0 || $device_namely === '') {
        $error = "Nama device, kategori, dan 'namely' wajib diisi.";
    }

    if (!$error) {
        $check = $conn->prepare("SELECT 1 FROM device_type WHERE company_code = ? AND device_name COLLATE utf8mb4_general_ci = ? LIMIT 1");
        $check->bind_param("is", $company_code, $device_name);
        $check->execute();
        $check->store_result();
        if ($check->num_rows > 0) {
            $error = "Nama device sudah ada (termasuk yang sudah dihapus).";
        }
        $check->close();
    }

    if (!$error) {
        if (!empty($_FILES['deviceImageFile']) && $_FILES['deviceImageFile']['error'] !== UPLOAD_ERR_NO_FILE) {
            $f = $_FILES['deviceImageFile'];
            if ($f['error'] !== UPLOAD_ERR_OK) {
                $error = "Gagal upload device image (error code {$f['error']}).";
            } else {
                if ($f['size'] > $deviceMaxSize) {
                    $error = "Ukuran device image maksimal " . ($deviceMaxSize / 1024 / 1024) . " MB.";
                } else {
                    $ext = strtolower(pathinfo($f['name'], PATHINFO_EXTENSION));
                    if (!in_array($ext, $allowedImageExt)) {
                        $error = "Ekstensi device image tidak diizinkan. (" . implode(',', $allowedImageExt) . ")";
                    } else {
                        $newName = 'device_' . $company_code . '_' . time() . '_' . rand(1000, 9999) . '.' . $ext;
                        $target = $deviceUploadDir . $newName;
                        if (!move_uploaded_file($f['tmp_name'], $target)) {
                            $error = "Gagal menyimpan file device image.";
                        } else {
                            $deviceImagePath = 'assets/upload/device/' . $newName;
                        }
                    }
                }
            }
        } else {
            $deviceImageUrl = trim($_POST['deviceImage'] ?? '');
            if ($deviceImageUrl !== '') {
                if (filter_var($deviceImageUrl, FILTER_VALIDATE_URL)) {
                    $deviceImagePath = $deviceImageUrl;
                } else {
                    $error = "Device image - URL tidak valid.";
                }
            }
        }
    }

    if (!$error) {
        if (!empty($_FILES['otaFile']) && $_FILES['otaFile']['error'] !== UPLOAD_ERR_NO_FILE) {
            $f = $_FILES['otaFile'];
            if ($f['error'] !== UPLOAD_ERR_OK) {
                $error = "Gagal upload OTA file (error code {$f['error']}).";
            } else {
                if ($f['size'] > $otaMaxSize) {
                    $error = "Ukuran OTA maksimal " . ($otaMaxSize / 1024 / 1024) . " MB.";
                } else {
                    $ext = strtolower(pathinfo($f['name'], PATHINFO_EXTENSION));
                    if (!in_array($ext, $allowedOtaExt)) {
                        $error = "Ekstensi OTA tidak diizinkan. (" . implode(',', $allowedOtaExt) . ")";
                    } else {
                        $newName = 'ota_' . $company_code . '_' . time() . '_' . rand(1000, 9999) . '.' . $ext;
                        $target = $otaUploadDir . $newName;
                        if (!move_uploaded_file($f['tmp_name'], $target)) {
                            $error = "Gagal menyimpan file OTA.";
                        } else {
                            $otaFilePath = 'assets/upload/ota/' . $newName;
                        }
                    }
                }
            }
        } else {
            $deviceOtaUrl = trim($_POST['deviceOtaurl'] ?? '');
            if ($deviceOtaUrl !== '') {
                if (filter_var($deviceOtaUrl, FILTER_VALIDATE_URL)) {
                    $otaFilePath = $deviceOtaUrl;
                } else {
                    $otaFilePath = $deviceOtaUrl;
                }
            }
        }
    }

    if (!$error) {
        $query = $conn->prepare("SELECT MAX(device_type_code) AS lastKode FROM device_type WHERE company_code = ?");
        $query->bind_param("i", $company_code);
        $query->execute();
        $res = $query->get_result()->fetch_assoc();
        $nextKode = ($res['lastKode'] ?? 0) + 1;

        $stmt = $conn->prepare("INSERT INTO device_type (company_code, updated_by, category_id, device_type_code, device_name, device_namely, description, device_image, device_hardware, device_firmware, device_hardware_build, device_firmware_build, device_otaurl) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        if (!$stmt) {
            $error = "Prepare statement gagal: " . $conn->error;
        } else {
            $stmt->bind_param(
                "iiiisssssssss",
                $company_code,
                $user_id,
                $category_id,
                $nextKode,
                $device_name,
                $device_namely,
                $description,
                $deviceImagePath,
                $device_hardware,
                $device_firmware,
                $device_hardware_build,
                $device_firmware_build,
                $otaFilePath
            );

            if ($stmt->execute()) {
                $success = "Device berhasil ditambahkan.";
            } else {
                if ($deviceImagePath && strpos($deviceImagePath, 'assets/upload/device/') === 0) {
                    @unlink(__DIR__ . '/../../' . $deviceImagePath);
                }
                if ($otaFilePath && strpos($otaFilePath, 'assets/upload/ota/') === 0) {
                    @unlink(__DIR__ . '/../../' . $otaFilePath);
                }
                $error = "Terjadi kesalahan saat menyimpan data: " . $stmt->error;
            }
        }
    }

    if ($success) {
        echo "<script src='../../assets/js/plugin/sweetalert/sweetalert.min.js'></script>
        <script>
            document.addEventListener('DOMContentLoaded', function() {
                swal({
                    title: 'Berhasil!',
                    text: '" . addslashes($success) . "',
                    icon: 'success',
                    button: false,
                    timer: 1200
                }).then(() => { window.location = './list'; });
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
}
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <?php include_once dirname(__FILE__) . '/config_base.php'; ?>
    <base href="<?= $base ?>">
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <title>Create Device</title>
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
        #previewDeviceImage {
            max-width: 250px;
            max-height: 180px;
            display: none;
            border-radius: 10px;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.15);
            margin-top: 12px;
            object-fit: contain;
        }

        .upload-drop {
            border: 2px dashed #ced4da;
            border-radius: 10px;
            padding: 14px;
            text-align: center;
            cursor: pointer;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            /* center horizontally */
            min-height: 140px;
            /* make room for preview */
            gap: 8px;
            background: #fff;
            transition: background .12s ease;
        }

        .upload-drop .drop-text {
            color: #6c757d;
        }

        .small-muted {
            font-size: 0.85rem;
            color: #6c757d;
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
            <div class="content">
                <div class="page-inner">
                    <div class="d-flex align-items-left flex-column pt-2 pb-4">
                        <h3 class="fw-bold mb-3">Create Device</h3>
                    </div>

                    <div class="card card-round">
                        <div class="card-header">
                            <div class="card-title">Add Device Data</div>
                        </div>
                        <!-- enctype needed for file upload -->
                        <form method="POST" id="formDevice" enctype="multipart/form-data">
                            <div class="card-body">
                                <div class="form-group">
                                    <label for="deviceName">Device Name</label>
                                    <input type="text" class="form-control" id="deviceName" name="deviceName"
                                        placeholder="Enter device name..." required>
                                </div>

                                <div class="form-group">
                                    <label for="category_id">Category</label>
                                    <select name="category_id" id="category_id" class="form-control" required>
                                        <option value="">-- Select Category --</option>
                                        <?php
                                        $res = $conn->prepare("SELECT category_id, description, category_code FROM device_category WHERE company_code = ?");
                                        $res->bind_param("i", $company_code);
                                        $res->execute();
                                        $cats = $res->get_result();
                                        while ($cat = $cats->fetch_assoc()) {
                                            echo "<option value='" . intval($cat['category_id']) . "' data-kode='" . htmlspecialchars($cat['category_code'], ENT_QUOTES) . "'>" . htmlspecialchars($cat['description']) . "</option>";
                                        }
                                        ?>
                                    </select>
                                </div>

                                <div class="form-group">
                                    <label for="deviceNamely">Device Namely</label>
                                    <input type="text" class="form-control" id="deviceNamely" name="deviceNamely"
                                        placeholder="Enter device namely..." required>
                                </div>

                                <div class="form-group">
                                    <label for="deviceHardware">Device Hardware</label>
                                    <input type="text" class="form-control" id="deviceHardware" name="deviceHardware"
                                        placeholder="Enter device hardware..." required>
                                </div>

                                <div class="form-group">
                                    <label for="deviceFirmware">Device Firmware</label>
                                    <input type="text" class="form-control" id="deviceFirmware" name="deviceFirmware"
                                        placeholder="Enter device firmware..." required>
                                </div>

                                <div class="form-group">
                                    <label for="deviceHardwareBuild">Device Hardware Build</label>
                                    <input type="date" class="form-control" id="deviceHardwareBuild"
                                        name="deviceHardwareBuild" required>
                                </div>

                                <div class="form-group">
                                    <label for="deviceFirmwareBuild">Device Firmware Build</label>
                                    <input type="date" class="form-control" id="deviceFirmwareBuild"
                                        name="deviceFirmwareBuild" required>
                                </div>

                                <div class="form-group">
                                    <label for="otaFile">OTA File</label>
                                    <input type="file" class="form-control" id="otaFile" name="otaFile"
                                        accept=".bin,.zip,.tar,.gz,.rar" required>
                                    <small class="small-muted">Allowed: <?php echo implode(', ', $allowedOtaExt); ?> —
                                        max <?php echo ($otaMaxSize / 1024 / 1024); ?> MB</small>
                                    <div id="otaFilename" class="mt-2"></div>
                                </div>

                                <div class="form-group">
                                    <label>Device Image</label>
                                    <div class="upload-drop" id="dropDeviceImage" role="button" tabindex="0">
                                        <div class="drop-text">Drag & drop image here, or click to choose</div>
                                        <img id="previewDeviceImage" src="#" alt="Preview">
                                        <div class="mt-2 small-muted">Allowed images:
                                            <?php echo implode(', ', $allowedImageExt); ?> — max
                                            <?php echo ($deviceMaxSize / 1024 / 1024); ?> MB
                                        </div>
                                    </div>
                                    <input type="file" class="form-control mt-2" id="deviceImageFile"
                                        name="deviceImageFile" accept="image/*" required>
                                </div>

                                <div class="form-group">
                                    <label for="descriptionDevice">Description</label>
                                    <textarea class="form-control" id="descriptionDevice" name="descriptionDevice"
                                        rows="3" placeholder="Enter description..." required></textarea>
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

    <!-- JS -->
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
            const preview = document.getElementById('previewDeviceImage');
            const dropBox = document.getElementById('dropDeviceImage');

            // Hapus teks drag-drop
            dropBox.innerHTML = `
            <div class="small-muted">Preview will appear here after selecting image</div>
            <img id="previewDeviceImage" src="#" alt="Preview" style="max-width:250px; display:none; border-radius:10px; margin-top:12px; box-shadow:0 2px 6px rgba(0,0,0,0.15);" />
        `;

            // Re-bind preview element
            const previewImg = document.getElementById('previewDeviceImage');

            // Preview ketika memilih file
            fileInput.addEventListener('change', (e) => {
                const file = e.target.files[0];

                if (!file) {
                    // Jika cancel → hilangkan preview
                    previewImg.style.display = 'none';
                    previewImg.src = '#';
                    return;
                }

                if (!file.type.startsWith('image/')) {
                    previewImg.style.display = 'none';
                    previewImg.src = '';
                    return;
                }

                const reader = new FileReader();
                reader.onload = function (ev) {
                    previewImg.src = ev.target.result;
                    previewImg.style.display = 'block';
                };
                reader.readAsDataURL(file);
            });
        })();
    </script>

</body>

</html>