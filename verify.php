<?php
include 'connection.php';

if (isset($_GET['token'])) {
    $token = $_GET['token'];
    $stmt = $conn->prepare("SELECT * FROM device_user WHERE verification_token=? AND status=1");
    $stmt->bind_param("s", $token);
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows === 1) {
        $stmt = $conn->prepare("UPDATE device_user SET status=2, verification_token=NULL WHERE verification_token=?");
        $stmt->bind_param("s", $token);
        if ($stmt->execute()) {
            echo "<script>alert('Akun berhasil diverifikasi! Silahkan login.'); window.location='login';</script>";
        } else {
            echo "<script>alert('Terjadi kesalahan saat memverifikasi akun. Silahkan coba lagi.'); window.location='login';</script>";
        }
    } else {
        echo "<script>alert('Token tidak valid atau akun sudah diverifikasi!'); window.location='login';</script>";
    }
} else {
    echo "<script>alert('Token verifikasi tidak ditemukan!');";
}