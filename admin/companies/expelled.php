<?php
require_once '../../config_base.php';
require_once '../../connection.php';

header('Content-Type: application/json');

if (!isset($_SESSION['user_id'])) {
    header('Location: ../../login');
    exit;
}

$admin_id = intval($_SESSION['user_id']);
$user_id = isset($_POST['user_id']) ? intval($_POST['user_id']) : 0;

if ($user_id <= 0) {
    echo json_encode(['success' => false, 'message' => 'Invalid user_id']);
    exit;
}

$pt = $conn->prepare("SELECT c.company_code, c.created_by FROM device_company c WHERE c.company_code = (SELECT company_code FROM device_user WHERE user_id = ? LIMIT 1) LIMIT 1");
$pt->bind_param("i", $admin_id);
$pt->execute();
$ptRes = $pt->get_result()->fetch_assoc();

if (!$ptRes) {
    echo json_encode(['success' => false, 'message' => 'Anda tidak tergabung di company manapun']);
    exit;
}

$company_code = intval($ptRes['company_code']);
$created_by = intval($ptRes['created_by']);

if ($created_by !== $admin_id) {
    echo json_encode(['success' => false, 'message' => 'Hanya pemilik PT yang dapat mengeluarkan pengguna']);
    exit;
}

$check = $conn->prepare("SELECT company_code FROM device_user WHERE user_id = ? LIMIT 1");
$check->bind_param("i", $user_id);
$check->execute();
$checkRes = $check->get_result()->fetch_assoc();

if (!$checkRes || intval($checkRes['company_code']) !== $company_code) {
    echo json_encode(['success' => false, 'message' => 'Pengguna tidak ditemukan di perusahaan Anda.']);
    exit;
}

$update = $conn->prepare("UPDATE device_user SET company_code = NULL WHERE user_id = ?");
$update->bind_param("i", $user_id);
$updateRes = $update->execute();
if ($updateRes) {
    echo json_encode(['success' => true]);
} else {
    echo json_encode(['success' => false, 'message' => 'Gagal mengeluarkan pengguna dari perusahaan.']);
}
?>