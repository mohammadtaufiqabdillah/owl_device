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
    echo json_encode(['success' => false, 'message' => 'Invalid User ID.']);
    exit;
}

$pt = $conn->prepare("SELECT c.company_code, c.created_by FROM device_company c WHERE c.company_code = (SELECT company_code FROM device_user WHERE user_id = ? LIMIT 1) LIMIT 1");
$pt->bind_param("i", $admin_id);
$pt->execute();
$ptResult = $pt->get_result()->fetch_assoc();

if (!$ptResult) {
    echo json_encode(['success' => false, 'message' => 'Anda tidak tergabung di PT manapun.']);
    exit;
}

$company_code = $ptResult['company_code'];
$creator_id = $ptResult['created_by'];

if ($creator_id !== $admin_id) {
    echo json_encode(['success' => false, 'message' => 'Hanya pembuat PT yang dapat menyetujui permintaan bergabung.']);
    exit;
}

$check = $conn->prepare("SELECT company_request_code FROM device_user WHERE user_id = ? LIMIT 1");
$check->bind_param("i", $user_id);
$check->execute();
$checkResult = $check->get_result()->fetch_assoc();

if (!$checkResult || $checkResult['company_request_code'] !== $company_code) {
    echo json_encode(['success' => false, 'message' => 'Pengguna tidak memiliki permintaan bergabung yang valid.']);
    exit;
}

$update = $conn->prepare("UPDATE device_user SET company_code = ?, company_request_code = NULL WHERE user_id = ?");
$update->bind_param("ii", $company_code, $user_id);
$upd = $update->execute();

if ($upd) {
    echo json_encode(['success' => true, 'message' => 'Permintaan bergabung disetujui.']);
} else {
    echo json_encode(['success' => false, 'message' => 'Gagal menyetujui permintaan bergabung.']);
}
?>