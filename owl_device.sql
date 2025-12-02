-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Waktu pembuatan: 02 Des 2025 pada 07.21
-- Versi server: 10.4.32-MariaDB
-- Versi PHP: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `owl_device`
--

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_category`
--

CREATE TABLE `device_category` (
  `category_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `category_code` int(11) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_category`
--

INSERT INTO `device_category` (`category_id`, `company_code`, `category_code`, `description`, `updated_by`) VALUES
(1, 100, 1, 'Attendance Solutions', 1),
(2, 100, 2, 'Environmental Monitoring', 1),
(3, 5514, 1, 'Another Category', 2),
(7, 80, 1, 'Access & Attendance Solutions', 3),
(8, 80, 2, 'Environmental Monitoring', 3),
(9, 100, 3, 'Environment & Factory Monitoring', 1),
(10, 80, 3, 'Environment & Factory Monitoring', 3),
(11, 91, 4, 'IoT Connectivity Devices', 4);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_command`
--

CREATE TABLE `device_command` (
  `command_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `command_type` enum('set','exe','dat') NOT NULL,
  `command_code` int(11) DEFAULT NULL,
  `command_name` varchar(60) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `is_main` tinyint(1) DEFAULT NULL,
  `custom_class` varchar(255) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  `is_deleted` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_command`
--

INSERT INTO `device_command` (`command_id`, `company_code`, `command_type`, `command_code`, `command_name`, `description`, `is_main`, `custom_class`, `updated_by`, `is_deleted`) VALUES
(1, 100, 'dat', 1, 'testing_company_proof', NULL, NULL, NULL, 1, 0),
(2, 5514, 'dat', 1, 'command_data', NULL, NULL, NULL, 2, 0),
(4, 5514, 'dat', 3, 'command_data', NULL, NULL, NULL, 2, 0),
(5, 5514, 'dat', 4, 'tyjhg', NULL, NULL, NULL, 2, 0),
(6, 100, 'exe', 2, 'exe_proof', NULL, NULL, NULL, 1, 0),
(7, 100, 'exe', 3, 'exe_proof', NULL, NULL, NULL, 1, 0),
(8, 100, 'dat', 2, 'testing_company_proof', NULL, NULL, NULL, 1, 0),
(9, 100, 'dat', 3, 'testing_company_proof', NULL, 0, NULL, 1, 0),
(10, 100, 'dat', 4, 'uy', NULL, NULL, NULL, 1, 0),
(11, 100, 'exe', 4, 'exe_proof', NULL, NULL, NULL, 1, 0),
(12, 100, 'set', 1, 'set_proof', NULL, NULL, NULL, 1, 0),
(13, 100, 'set', 2, '0', '', 1, '', 1, 0),
(14, 100, 'set', 3, '0', '', 1, '', 1, 0),
(15, 100, 'set', 4, 'ma', '', 1, '', 1, 0),
(16, 100, 'set', 5, 'io', NULL, 1, NULL, 1, 0),
(17, 80, 'set', 1, 'ma', '', 0, NULL, 3, 0),
(18, 100, 'dat', 5, 'pom_data', NULL, 1, NULL, 1, 0),
(19, 100, 'exe', 5, 'pom_data', NULL, 0, NULL, 1, 0),
(20, 100, 'dat', 6, 'testing', NULL, 0, NULL, 1, 0),
(21, 100, 'exe', 6, 'restart', NULL, 1, NULL, 1, 0),
(22, 100, 'set', 6, 'wifi', NULL, 1, NULL, 1, 0),
(23, 100, 'exe', 7, 'exe_proof', NULL, 1, NULL, 1, 0),
(24, 100, 'dat', 7, 'testing', NULL, 0, NULL, 1, 0),
(25, 100, 'dat', 8, 'pom', NULL, 1, NULL, 1, 0),
(26, 80, 'dat', 1, 'pom_data', NULL, 1, NULL, 3, 0),
(27, 80, 'exe', 1, 'restart', NULL, 1, NULL, 3, 0),
(28, 80, 'set', 2, 'wifi', NULL, 0, NULL, 3, 0),
(29, 80, 'dat', 2, 'testing', NULL, 1, NULL, 3, 0),
(30, 80, 'exe', 2, 'testing', NULL, 1, NULL, 3, 0),
(31, 80, 'set', 3, 'testing', NULL, 0, NULL, 3, 0),
(32, 100, 'set', 7, 'mqtt', NULL, 1, NULL, 1, 0),
(33, 100, 'set', 8, 'interval', NULL, 1, NULL, 1, 0),
(34, 100, 'set', 9, 'testing', NULL, 0, NULL, 1, 0),
(35, 100, 'set', 10, 'restart', NULL, 0, NULL, 1, 0),
(36, 100, 'set', 11, 'stb_bandul_setup', NULL, 0, NULL, 1, 0),
(37, 100, 'set', 12, 'stb_mekanikal_setup', NULL, 0, NULL, 1, 0),
(38, 91, 'set', 1, 'wifi', NULL, 0, NULL, 4, 0),
(39, 91, 'set', 2, 'mqtt', NULL, 0, NULL, 4, 0),
(40, 91, 'set', 7, 'use_key', NULL, 0, NULL, 4, 0),
(41, 91, 'set', 22, 'stb_bandul_setup', NULL, 0, NULL, 4, 0),
(42, 91, 'set', 25, 'stb_mekanikal_setup', NULL, 0, NULL, 4, 0),
(43, 91, 'exe', 1, 'restart', NULL, 0, NULL, 4, 0),
(44, 91, 'set', 3, 'interval', NULL, 0, NULL, 4, 0),
(45, 91, 'set', 4, 'utcoffset', NULL, 0, NULL, 4, 0),
(46, 91, 'set', 5, 'last_epoch', NULL, 0, NULL, 4, 0),
(47, 91, 'set', 6, 'key', NULL, 0, NULL, 4, 0),
(48, 91, 'set', 8, 'sn', NULL, 0, NULL, 4, 0),
(49, 91, 'set', 9, 'otaurl', NULL, 0, NULL, 4, 0),
(50, 91, 'set', 10, 'api_ver1', NULL, 0, NULL, 4, 0),
(51, 91, 'set', 11, 'api_ver2', NULL, 0, NULL, 4, 0),
(52, 91, 'set', 12, 'adv_ble', NULL, 0, NULL, 4, 0),
(53, 91, 'set', 13, 'radio_mode', NULL, 0, NULL, 4, 0),
(54, 91, 'set', 14, 'radio_param_1', NULL, 0, NULL, 4, 0),
(55, 91, 'set', 15, 'radio_param_2', NULL, 0, NULL, 4, 0),
(56, 91, 'set', 16, 'radio_param_3', NULL, 0, NULL, 4, 0),
(57, 91, 'set', 17, 'radio_param_4', NULL, 0, NULL, 4, 0),
(58, 91, 'set', 18, 'license', NULL, 0, NULL, 4, 0),
(59, 91, 'set', 19, 'suspend', NULL, 0, NULL, 4, 0),
(60, 91, 'set', 20, 'hierarchy_setup', NULL, 0, NULL, 4, 0),
(61, 91, 'set', 21, 'fingerprint_setup', NULL, 0, NULL, 4, 0),
(62, 91, 'set', 23, 'pom_setup', NULL, 0, NULL, 4, 0),
(63, 91, 'set', 24, 'stb_controller_setup', NULL, 0, NULL, 4, 0),
(64, 91, 'set', 26, 'tracker_setup', NULL, 0, NULL, 4, 0),
(65, 91, 'set', 27, 'stb_controller_sub_setup', NULL, 0, NULL, 4, 0),
(66, 91, 'set', 28, 'touch_calibration', NULL, 0, NULL, 4, 0),
(67, 91, 'set', 29, 'esp_now', NULL, 0, NULL, 4, 0),
(68, 91, 'set', 30, 'functional_flag', NULL, 0, NULL, 4, 0),
(69, 91, 'dat', 1, 'pom_data', NULL, 0, NULL, 4, 0),
(70, 91, 'dat', 2, 'radio_pooling_data', NULL, 0, NULL, 4, 0),
(71, 91, 'dat', 3, 'bandul_data', NULL, 0, NULL, 4, 0),
(72, 91, 'dat', 4, 'logger_gps_data', NULL, 0, NULL, 4, 0),
(73, 91, 'dat', 5, 'logger_vehicle_gps_data', NULL, 0, NULL, 4, 0),
(74, 91, 'dat', 6, 'radio_broadcast_data', NULL, 0, NULL, 4, 0),
(75, 91, 'dat', 7, 'radio_join_data', NULL, 0, NULL, 4, 0),
(76, 91, 'dat', 8, 'radio_response_data', NULL, 0, NULL, 4, 0),
(77, 91, 'dat', 9, 'stb_notification', NULL, 0, NULL, 4, 0),
(78, 91, 'dat', 10, 'stb_sounding_data', NULL, 0, NULL, 4, 0),
(79, 91, 'dat', 11, 'soil_data', NULL, 0, NULL, 4, 0),
(80, 91, 'dat', 12, 'stb_data_air', NULL, 0, NULL, 4, 1),
(81, 91, 'exe', 2, 'file_read', NULL, 0, NULL, 4, 0),
(82, 91, 'exe', 3, 'file_write', NULL, 0, NULL, 4, 0),
(83, 91, 'exe', 4, 'file_append', NULL, 0, NULL, 4, 0),
(84, 91, 'exe', 5, 'file_delete', NULL, 0, NULL, 4, 0),
(85, 91, 'exe', 6, 'pom_receive', NULL, 0, NULL, 4, 0),
(86, 91, 'exe', 7, 'query_sql', NULL, 0, NULL, 4, 0),
(87, 91, 'exe', 8, 'fingerprint_register', NULL, 0, NULL, 4, 0),
(88, 91, 'exe', 9, 'backup_template_fp', NULL, 0, NULL, 4, 0),
(89, 91, 'exe', 10, 'restore_template_fp', NULL, 0, NULL, 4, 0),
(90, 91, 'exe', 11, 'delete_template_fp', NULL, 0, NULL, 4, 0),
(91, 91, 'exe', 12, 'admin_fp', NULL, 0, NULL, 4, 0),
(92, 91, 'exe', 13, 'get_info', NULL, 0, NULL, 4, 0),
(93, 91, 'exe', 14, 'factory_reset', NULL, 0, NULL, 4, 0),
(94, 91, 'exe', 15, 'fingerprint_scan', NULL, 0, NULL, 4, 0),
(95, 91, 'exe', 16, 'stb_move_to', NULL, 0, NULL, 4, 0),
(96, 91, 'exe', 17, 'stb_move_detection', NULL, 0, NULL, 4, 0),
(97, 91, 'exe', 18, 'stb_sounding', NULL, 0, NULL, 4, 0),
(98, 91, 'exe', 19, 'stb_sounding_all', NULL, 0, NULL, 4, 0),
(99, 91, 'exe', 20, 'save_ir', NULL, 0, NULL, 4, 0),
(100, 91, 'exe', 21, 'execute_ir', NULL, 0, NULL, 4, 0),
(101, 91, 'exe', 22, 'get_ir_list', NULL, 0, NULL, 4, 0),
(102, 91, 'exe', 23, 'schedule_daily', NULL, 0, NULL, 4, 0),
(103, 91, 'exe', 24, 'schedule_monthly', NULL, 0, NULL, 4, 0),
(104, 91, 'exe', 25, 'file_list', NULL, 0, NULL, 4, 0),
(105, 91, 'exe', 26, 'get_schedule', NULL, 0, NULL, 4, 0),
(106, 91, 'exe', 27, 'command_to_bandul', NULL, 0, NULL, 4, 0),
(107, 91, 'exe', 28, 'soil_measure', NULL, 0, NULL, 4, 0),
(108, 91, 'exe', 29, 'message_from_gateway', NULL, 0, NULL, 4, 0),
(109, 91, 'exe', 30, 'message_from_node', NULL, 0, NULL, 4, 0),
(110, 91, 'exe', 31, 'command_radio', NULL, 0, NULL, 4, 0),
(111, 91, 'exe', 32, 'command_to_mekanikal', NULL, 0, NULL, 4, 0),
(112, 91, 'exe', 33, 'get_bandul_data', NULL, 0, NULL, 4, 0),
(113, 91, 'exe', 34, 'get_data', NULL, 0, NULL, 4, 0),
(114, 91, 'exe', 35, 'update_mekanikal', NULL, 0, NULL, 4, 0),
(115, 91, 'exe', 36, 'upload_data', NULL, 0, NULL, 4, 0),
(116, 91, 'exe', 37, 'check_data', NULL, 0, NULL, 4, 0),
(117, 91, 'exe', 38, 'clear_schedule', NULL, 0, NULL, 4, 0),
(118, 91, 'exe', 39, 'stb_check_air', NULL, 0, NULL, 4, 0),
(119, 80, 'dat', 3, 'testing_company_proof', NULL, 0, NULL, 3, 0),
(120, 80, 'dat', 4, 'restart', NULL, 0, NULL, 3, 0);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_command_detail`
--

CREATE TABLE `device_command_detail` (
  `command_detail_id` int(11) NOT NULL,
  `command_id` int(11) DEFAULT NULL,
  `data_name` varchar(255) DEFAULT NULL,
  `data_type` varchar(255) DEFAULT NULL,
  `data_len` int(11) DEFAULT NULL,
  `data_default` varchar(255) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_command_detail`
--

INSERT INTO `device_command_detail` (`command_detail_id`, `command_id`, `data_name`, `data_type`, `data_len`, `data_default`, `description`, `updated_by`) VALUES
(1, 4, 'test', 'string', 130, 'LLXSKJ', 'KJDFLK', 2),
(2, 18, 'transaksi_id', 'uint32_t', 4, '0', NULL, 1),
(3, 18, 'liter', 'float', 4, '0.0', NULL, 1),
(4, 18, 'plat', 'string', 15, 'AA 4567 ZZZ', NULL, 1),
(5, 18, 'karyawan_id', 'uint32_t', 4, '0', NULL, 1),
(6, 18, 'stock', 'float', 4, '100', NULL, 1),
(7, 18, 'date', 'string', 23, '2025 09 06 55:66:89', NULL, 1),
(8, 21, 'state', 'bool', 1, 'true', NULL, 1),
(9, 22, 'ssid', 'string', 25, '87654321', NULL, 1),
(10, 22, 'password', 'string', 25, 'Devices', NULL, 1),
(11, 20, 'liter', 'string', 12, '0', NULL, 1),
(12, 26, 'transaksi_id', 'string', 21, 'Devices', NULL, 3),
(13, 27, 'transaksi_id', 'string', 131, 'dqwdsa', NULL, 3),
(14, 29, 'transaksi_id', 'string', 321, 'dsada', NULL, 3),
(15, 32, 'host', 'string', 125, '8.215.79.132', NULL, 1),
(16, 32, 'port', 'uint16_t', 2, '1883', NULL, 1),
(17, 32, 'username', 'string', 25, 'user_mqtt', NULL, 1),
(18, 32, 'password', 'string', 25, 'Pa55w0rd', NULL, 1),
(19, 35, 'state', 'bool', 1, 'false', NULL, 1),
(20, 36, 'bandul_id', 'uint32_t', 4, '0', NULL, 1),
(21, 36, 'bandul_lenght', 'float', 4, '21.5', NULL, 1),
(22, 37, 'mekanikal_id', 'uint32_t', 4, '0', NULL, 1),
(23, 37, 'maxrange', 'float', 4, '1500', NULL, 1),
(24, 33, 'get_data', 'uint32_t', 4, '60', NULL, 1),
(25, 33, 'upload_data', 'uint32_t', 4, '600', NULL, 1),
(26, 33, 'standby', 'uint32_t', 4, '60', NULL, 1),
(27, 38, 'ssid', 'string', 25, 'Devices', NULL, 4),
(28, 38, 'password', 'string', 25, '87654321', NULL, 4),
(29, 39, 'host', 'string', 125, '8.215.79.132', NULL, 4),
(30, 39, 'port', 'uint16_t', 2, '1883', NULL, 4),
(31, 39, 'username', 'string', 25, 'user_mqtt', NULL, 4),
(32, 39, 'password', 'string', 25, 'Pa55w0rd', NULL, 4),
(33, 49, 'otaurl', 'string', 125, 'http://103.150.191.136/owl_inventory/produksi/firmware.php', NULL, 4),
(34, 78, 'tinggi_minyak_dari_atas', 'float', 4, '0', NULL, 4),
(35, 78, 'tinggi_minyak_dari_bawah', 'float', 4, '0', NULL, 4),
(36, 78, 'tinggi_udara_dari_atas', 'float', 4, '0', NULL, 4),
(37, 78, 'tinggi_udara_dari_bawah', 'float', 4, '0', NULL, 4),
(38, 78, 'suhu', 'float', 4, '0', NULL, 4),
(39, 78, 'baterai_bandul', 'float', 4, '0', NULL, 4),
(40, 78, 'status', 'bool', 1, 'true', NULL, 4),
(41, 78, 'error_code', 'uint32_t', 4, '0', NULL, 4),
(42, 78, 'average_1', 'float', 4, '0', NULL, 4),
(43, 78, 'average_2', 'float', 4, '0', NULL, 4),
(44, 78, 'speed_naik', 'int16_t', 2, '0', NULL, 4),
(45, 78, 'speed_turun', 'int16_t', 2, '10', NULL, 4),
(46, 78, 'mejaukur', 'float', 4, '0', NULL, 4),
(47, 78, 'minyak', 'float', 4, '0', NULL, 4),
(48, 78, 'air', 'float', 4, '0', NULL, 4),
(49, 78, 'lumpur', 'float', 4, '0', NULL, 4),
(50, 78, 'tinggi_meja', 'float', 4, '0', NULL, 4),
(51, 78, 'deviasi', 'float', 4, '0', NULL, 4),
(52, 78, 'v_charge', 'float', 4, '0', NULL, 4),
(53, 78, 'suhu_rtc', 'float', 4, '0', NULL, 4),
(54, 78, 'suhu_bandul', 'float', 4, '0', NULL, 4),
(55, 78, 'suhu_probe', 'float', 4, '0', NULL, 4),
(56, 78, 'suhu_mek', 'float', 4, '0', NULL, 4),
(57, 78, 'recorded_at', 'uint32_t', 4, '10000', NULL, 4),
(58, 69, 'transaksi_id', 'uint32_t', 4, '0', NULL, 4),
(59, 69, 'liter', 'float', 4, '0', NULL, 4),
(60, 69, 'plat', 'string', 15, 'AA 4567 ZZZ', NULL, 4),
(61, 69, 'karyawan_id', 'uint32_t', 4, '0', NULL, 4),
(62, 69, 'stock', 'float', 4, '100', NULL, 4),
(63, 69, 'date', 'string', 23, '2025 09 06 55:66:89', NULL, 4);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_company`
--

CREATE TABLE `device_company` (
  `company_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `company_name` varchar(60) DEFAULT NULL,
  `company_address` varchar(255) DEFAULT NULL,
  `company_email` varchar(250) DEFAULT NULL,
  `company_telephone_number` varchar(15) DEFAULT NULL,
  `company_token` varchar(64) NOT NULL,
  `updated_by` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_company`
--

INSERT INTO `device_company` (`company_id`, `company_code`, `company_name`, `company_address`, `company_email`, `company_telephone_number`, `company_token`, `updated_by`) VALUES
(1, 100, 'PT. Prof Of Concept', 'oiaj', 'akj@fam.com', 'faoij', '72fc51354facbb78', 1),
(2, 5514, 'Anothera', 'jakj', 'owk@alkjf.com', 'dajkl', 'f697ed145c12b3c2', 2),
(3, 80, 'uy', '', '', '', '915e8d407e77470f', 3),
(4, 1, 'OWL', NULL, NULL, NULL, 'bfc3720f44b3d532', 5),
(5, 91, 'Cari Bug', NULL, NULL, NULL, '7eb1b9b4ccf5fce2', 4);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_error_category`
--

CREATE TABLE `device_error_category` (
  `error_category_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `error_category_code` int(11) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_error_category`
--

INSERT INTO `device_error_category` (`error_category_id`, `company_code`, `error_category_code`, `description`, `updated_by`) VALUES
(1, 100, 1, 'Fingerprint Sensor', 1),
(2, 100, 2, 'RTC', 1),
(3, 100, 3, 'Memory', 1),
(4, 100, 4, 'Display', 1),
(5, 80, 1, 'Fingerprint Sensor', 3),
(6, 80, 2, 'RTC', 3);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_error_code`
--

CREATE TABLE `device_error_code` (
  `error_code_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `error_code` int(11) DEFAULT NULL,
  `error_source` tinyint(4) NOT NULL,
  `error_category` int(11) NOT NULL,
  `error_number` int(11) NOT NULL,
  `code_error` int(11) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  `is_deleted` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_error_code`
--

INSERT INTO `device_error_code` (`error_code_id`, `company_code`, `error_code`, `error_source`, `error_category`, `error_number`, `code_error`, `description`, `updated_by`, `is_deleted`) VALUES
(1, 100, NULL, 1, 2, 1, 10201, '- Jam tidak terdeteksi. - IC RTC bermasalah. - Hubungi teknisi untuk perbaikan.', 1, 0),
(2, 100, NULL, 1, 3, 1, 10301, '- SD Card tidak terdeteksi. - Plug/unPlug SD Card. - Hubungi Teknisi untuk perbaikan.', 1, 0),
(3, 100, NULL, 1, 4, 1, 10401, '- Display gagal. - Hubungi teknisi untuk perbaikan.', 1, 0),
(4, 80, NULL, 1, 6, 1, 10201, '- Jam tidak terdeteksi. - IC RTC bermasalah. - Hubungi teknisi untuk perbaikan.', 3, 0);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_type`
--

CREATE TABLE `device_type` (
  `device_type_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `category_id` int(11) DEFAULT NULL,
  `device_type_code` int(11) DEFAULT NULL,
  `device_name` varchar(255) DEFAULT NULL,
  `device_namely` varchar(255) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `device_image` varchar(255) DEFAULT NULL,
  `device_hardware` varchar(15) DEFAULT NULL,
  `device_firmware` varchar(15) DEFAULT NULL,
  `device_hardware_build` date DEFAULT NULL,
  `device_firmware_build` date DEFAULT NULL,
  `device_otaurl` varchar(255) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  `is_deleted` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_type`
--

INSERT INTO `device_type` (`device_type_id`, `company_code`, `category_id`, `device_type_code`, `device_name`, `device_namely`, `description`, `device_image`, `device_hardware`, `device_firmware`, `device_hardware_build`, `device_firmware_build`, `device_otaurl`, `updated_by`, `is_deleted`) VALUES
(1, 100, 1, 1, 'FP307-BW-B6.66', 'Singkong Finger', 'discontinue', NULL, '1.0.0', '1.0.0', '2025-11-10', '2025-11-10', '', 1, 0),
(2, 80, 7, 1, 'FP307-BW-B6.66', 'Singkong Finger', 'discontinu', NULL, '1.0.0', '1.0.0', '2025-11-13', '2025-11-12', '1.1.0', 3, 0),
(3, 100, 9, 2, 'STB3-MEKANIKAL', 'Mekanikal Odsou', 'discontinu', 'assets/upload/device/device_100_1763431190_4876.png', '1.0.0', '1.0.0', '2025-11-18', '2025-11-18', 'assets/upload/ota/ota_100_1763431190_5116.zip', 1, 0),
(4, 100, 9, 3, 'STB3-BANDUL', 'SmartBandul', 'discontinu', 'assets/upload/device/device_100_1763436676_1132.png', '1.0.0', '1.0.0', '2025-11-18', '2025-11-18', 'assets/upload/ota/ota_100_1763436722_1065.zip', 1, 0),
(5, 80, 10, 2, 'STB3-MEKANIKAL', 'Mekanikal Odsou', 'discontinu', 'assets/upload/device/device_80_1763436868_4520.jpg', '1.0.0', '1.0.0', '2025-11-18', '2025-11-18', 'assets/upload/ota/ota_80_1763436868_9809.zip', 3, 0),
(6, 91, 11, 1, 'LOG_ALL', 'Internal Device', 'Semua command', 'assets/upload/device/device_91_1764231920_4035.jpg', '1.0.0', '1.0.0', '2025-11-27', '2025-11-27', 'assets/upload/ota/ota_91_1764231920_7966.zip', 4, 0);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_user`
--

CREATE TABLE `device_user` (
  `user_id` int(11) NOT NULL,
  `company_code` int(11) DEFAULT NULL,
  `user_name` varchar(60) NOT NULL,
  `username` varchar(60) NOT NULL,
  `password` varchar(255) NOT NULL,
  `user_email` varchar(60) NOT NULL,
  `user_profile_picture` varchar(255) DEFAULT NULL,
  `verification_token` varchar(64) NOT NULL,
  `status` tinyint(1) NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_user`
--

INSERT INTO `device_user` (`user_id`, `company_code`, `user_name`, `username`, `password`, `user_email`, `user_profile_picture`, `verification_token`, `status`, `created_at`, `updated_at`) VALUES
(1, 100, 'admin', 'admin', '$2y$10$HuaddQgexGeaVeetIKJp4Ox9FTz0E6EkPkK9BafaZfd4gOutMQPdy', 'admin@gmail.com', 'assets/upload/profile/user_1_1764564026.jpeg', '', 2, '2025-11-06 16:47:16', '2025-12-01 11:40:26'),
(2, 5514, 'admin2', 'admin2', '$2y$10$568iuny15iMWEW6EuO5VZuvYr8Cj1uF7A8o3psZiqZU2V6mzXVPRO', 'admin2@gmail.com', '', '', 2, '2025-11-06 17:13:47', '2025-11-06 17:13:47'),
(3, 80, 'admin3', 'admin3', '$2y$10$he37u.HCPie48E/El9DuUOq5gzi.hQsqXSny5ekvwcJjlziZU4bjC', 'admin3@gmail.com', '', '', 2, '2025-11-07 10:08:54', '2025-11-07 10:08:54'),
(4, 91, 'admin4', 'admin4', '$2y$10$VlvqY7C2Zr2cUpSMZvfpg.WTEHmvEeMhFdqfewU/6FHIiwONgkH7i', 'admin4@gmail.com', 'assets/upload/profile/user_4_1764564149.jpeg', '', 2, '2025-11-10 14:18:43', '2025-12-01 11:42:29'),
(5, 1, 'Muhammad Makarimal Akhlak', 'karim', '$2y$10$jdIktMA/5BOsg5/LN8mVT.4VNt7gKYXgNZ1FaLPXwXGNIMRMQKC5W', 'makarimalkarim@gmail.com', '', '', 2, '2025-11-12 16:33:27', '2025-11-12 16:33:27'),
(6, NULL, 'riki', 'riki', '$2y$10$0ki76j2GkMzGjtpl.N.68eeLhKaukyZ/O2JvbbsaZE4r3PTg0HMs6', 'riki@plala.com', '', '859de5e7dd335b7f8eaa488b0a6eb35aec96e910c8891d4ed624026c4dc6bbbe', 1, '2025-11-13 16:56:54', '2025-11-13 16:56:54'),
(10, NULL, 'tfkabdllh', 'abdil', '$2y$10$ucekgXjr36707VzR4ijTr.QyERdu8SwY72YulY6smcc9AE8ciBFmm', 'tfkabdllh@gmail.com', 'assets/upload/profile/user_10_1764564405.jpeg', '', 2, '2025-12-01 11:11:06', '2025-12-01 14:12:44');

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_vs_command`
--

CREATE TABLE `device_vs_command` (
  `device_type_id` int(11) NOT NULL,
  `command_id` int(11) NOT NULL,
  `company_code` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_vs_command`
--

INSERT INTO `device_vs_command` (`device_type_id`, `command_id`, `company_code`) VALUES
(1, 7, 100),
(1, 18, 100),
(1, 21, 100),
(1, 22, 100),
(1, 32, 100),
(1, 34, 100),
(2, 26, 80),
(4, 18, 100),
(4, 22, 100),
(4, 33, 100),
(6, 38, 91),
(6, 39, 91),
(6, 40, 91),
(6, 41, 91),
(6, 42, 91),
(6, 43, 91),
(6, 44, 91),
(6, 45, 91),
(6, 46, 91),
(6, 47, 91),
(6, 48, 91),
(6, 49, 91),
(6, 50, 91),
(6, 51, 91),
(6, 52, 91),
(6, 53, 91),
(6, 54, 91),
(6, 55, 91),
(6, 56, 91),
(6, 57, 91),
(6, 58, 91),
(6, 59, 91),
(6, 60, 91),
(6, 61, 91),
(6, 62, 91),
(6, 63, 91),
(6, 64, 91),
(6, 65, 91),
(6, 66, 91),
(6, 67, 91),
(6, 68, 91),
(6, 69, 91),
(6, 70, 91),
(6, 71, 91),
(6, 72, 91),
(6, 73, 91),
(6, 74, 91),
(6, 75, 91),
(6, 76, 91),
(6, 77, 91),
(6, 78, 91),
(6, 79, 91),
(6, 80, 91),
(6, 81, 91),
(6, 82, 91),
(6, 83, 91),
(6, 84, 91),
(6, 85, 91),
(6, 86, 91),
(6, 87, 91),
(6, 88, 91),
(6, 89, 91),
(6, 90, 91),
(6, 91, 91),
(6, 92, 91),
(6, 93, 91),
(6, 94, 91),
(6, 95, 91),
(6, 96, 91),
(6, 97, 91),
(6, 98, 91),
(6, 99, 91),
(6, 100, 91),
(6, 101, 91),
(6, 102, 91),
(6, 103, 91),
(6, 104, 91),
(6, 105, 91),
(6, 106, 91),
(6, 107, 91),
(6, 108, 91),
(6, 109, 91),
(6, 110, 91),
(6, 111, 91),
(6, 112, 91),
(6, 113, 91),
(6, 114, 91),
(6, 115, 91),
(6, 116, 91),
(6, 117, 91),
(6, 118, 91);

-- --------------------------------------------------------

--
-- Struktur dari tabel `device_vs_error`
--

CREATE TABLE `device_vs_error` (
  `device_type_id` int(11) NOT NULL,
  `error_code_id` int(11) NOT NULL,
  `company_code` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data untuk tabel `device_vs_error`
--

INSERT INTO `device_vs_error` (`device_type_id`, `error_code_id`, `company_code`) VALUES
(1, 1, 100),
(2, 4, 80);

--
-- Indexes for dumped tables
--

--
-- Indeks untuk tabel `device_category`
--
ALTER TABLE `device_category`
  ADD PRIMARY KEY (`category_id`),
  ADD KEY `fk_device_category_company` (`company_code`),
  ADD KEY `fk_device_category_user` (`updated_by`);

--
-- Indeks untuk tabel `device_command`
--
ALTER TABLE `device_command`
  ADD PRIMARY KEY (`command_id`),
  ADD KEY `fk_device_command_company` (`company_code`),
  ADD KEY `fk_device_command_user` (`updated_by`),
  ADD KEY `idx_device_command_is_deleted` (`is_deleted`);

--
-- Indeks untuk tabel `device_command_detail`
--
ALTER TABLE `device_command_detail`
  ADD PRIMARY KEY (`command_detail_id`),
  ADD KEY `fk_device_detail_command` (`command_id`),
  ADD KEY `fk_device_detail_user` (`updated_by`);

--
-- Indeks untuk tabel `device_company`
--
ALTER TABLE `device_company`
  ADD PRIMARY KEY (`company_id`),
  ADD UNIQUE KEY `company_code` (`company_code`),
  ADD KEY `fk_device_company_user` (`updated_by`);

--
-- Indeks untuk tabel `device_error_category`
--
ALTER TABLE `device_error_category`
  ADD PRIMARY KEY (`error_category_id`),
  ADD KEY `fk_device_error_category_company` (`company_code`),
  ADD KEY `fk_device_error_category_user` (`updated_by`);

--
-- Indeks untuk tabel `device_error_code`
--
ALTER TABLE `device_error_code`
  ADD PRIMARY KEY (`error_code_id`),
  ADD KEY `fk_device_error_code_error_category` (`error_category`),
  ADD KEY `fk_device_error_code_company` (`company_code`),
  ADD KEY `fk_device_error_code_user` (`updated_by`),
  ADD KEY `idx_device_error_code_is_deleted` (`is_deleted`);

--
-- Indeks untuk tabel `device_type`
--
ALTER TABLE `device_type`
  ADD PRIMARY KEY (`device_type_id`),
  ADD KEY `fk_device_type_category` (`category_id`),
  ADD KEY `fk_device_type_company` (`company_code`),
  ADD KEY `fk_device_type_user` (`updated_by`),
  ADD KEY `idx_device_type_is_deleted` (`is_deleted`);

--
-- Indeks untuk tabel `device_user`
--
ALTER TABLE `device_user`
  ADD PRIMARY KEY (`user_id`),
  ADD KEY `fk_device_user_company` (`company_code`);

--
-- Indeks untuk tabel `device_vs_command`
--
ALTER TABLE `device_vs_command`
  ADD PRIMARY KEY (`device_type_id`,`command_id`,`company_code`),
  ADD KEY `device_type_id` (`device_type_id`) USING BTREE,
  ADD KEY `command_id` (`command_id`),
  ADD KEY `company_code` (`company_code`);

--
-- Indeks untuk tabel `device_vs_error`
--
ALTER TABLE `device_vs_error`
  ADD PRIMARY KEY (`device_type_id`,`error_code_id`,`company_code`) USING BTREE,
  ADD KEY `company_code` (`company_code`) USING BTREE,
  ADD KEY `error_code_id` (`error_code_id`) USING BTREE,
  ADD KEY `device_type_id1` (`device_type_id`) USING BTREE;

--
-- AUTO_INCREMENT untuk tabel yang dibuang
--

--
-- AUTO_INCREMENT untuk tabel `device_category`
--
ALTER TABLE `device_category`
  MODIFY `category_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12;

--
-- AUTO_INCREMENT untuk tabel `device_command`
--
ALTER TABLE `device_command`
  MODIFY `command_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=121;

--
-- AUTO_INCREMENT untuk tabel `device_command_detail`
--
ALTER TABLE `device_command_detail`
  MODIFY `command_detail_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=68;

--
-- AUTO_INCREMENT untuk tabel `device_company`
--
ALTER TABLE `device_company`
  MODIFY `company_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT untuk tabel `device_error_category`
--
ALTER TABLE `device_error_category`
  MODIFY `error_category_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- AUTO_INCREMENT untuk tabel `device_error_code`
--
ALTER TABLE `device_error_code`
  MODIFY `error_code_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT untuk tabel `device_type`
--
ALTER TABLE `device_type`
  MODIFY `device_type_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- AUTO_INCREMENT untuk tabel `device_user`
--
ALTER TABLE `device_user`
  MODIFY `user_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT untuk tabel `device_vs_error`
--
ALTER TABLE `device_vs_error`
  MODIFY `device_type_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- Ketidakleluasaan untuk tabel pelimpahan (Dumped Tables)
--

--
-- Ketidakleluasaan untuk tabel `device_category`
--
ALTER TABLE `device_category`
  ADD CONSTRAINT `fk_device_category_company` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_device_category_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_command`
--
ALTER TABLE `device_command`
  ADD CONSTRAINT `fk_device_command_company` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_device_command_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_command_detail`
--
ALTER TABLE `device_command_detail`
  ADD CONSTRAINT `fk_device_detail_command` FOREIGN KEY (`command_id`) REFERENCES `device_command` (`command_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_device_detail_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_company`
--
ALTER TABLE `device_company`
  ADD CONSTRAINT `fk_device_company_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_error_category`
--
ALTER TABLE `device_error_category`
  ADD CONSTRAINT `fk_device_error_category_company` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_device_error_category_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_error_code`
--
ALTER TABLE `device_error_code`
  ADD CONSTRAINT `fk_device_error_code_company` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_device_error_code_error_category` FOREIGN KEY (`error_category`) REFERENCES `device_error_category` (`error_category_id`),
  ADD CONSTRAINT `fk_device_error_code_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_type`
--
ALTER TABLE `device_type`
  ADD CONSTRAINT `fk_device_type_category` FOREIGN KEY (`category_id`) REFERENCES `device_category` (`category_id`),
  ADD CONSTRAINT `fk_device_type_company` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_device_type_user` FOREIGN KEY (`updated_by`) REFERENCES `device_user` (`user_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_user`
--
ALTER TABLE `device_user`
  ADD CONSTRAINT `fk_device_user_company` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_vs_command`
--
ALTER TABLE `device_vs_command`
  ADD CONSTRAINT `command_id` FOREIGN KEY (`command_id`) REFERENCES `device_command` (`command_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `commpany_code` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `device_type_id` FOREIGN KEY (`device_type_id`) REFERENCES `device_type` (`device_type_id`) ON UPDATE CASCADE;

--
-- Ketidakleluasaan untuk tabel `device_vs_error`
--
ALTER TABLE `device_vs_error`
  ADD CONSTRAINT `company_code` FOREIGN KEY (`company_code`) REFERENCES `device_company` (`company_code`) ON UPDATE CASCADE,
  ADD CONSTRAINT `device_type_id1` FOREIGN KEY (`device_type_id`) REFERENCES `device_type` (`device_type_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `error_code_id` FOREIGN KEY (`error_code_id`) REFERENCES `device_error_code` (`error_code_id`) ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
