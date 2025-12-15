<?php
require_once '../../config_base.php';
require_once '../../connection.php';

header('Content-Type: application/json');

function fail($msg, $http_code = 500)
{
    http_response_code($http_code);
    echo json_encode(['status' => 'error', 'message' => $msg]);
    exit;
}

if (!isset($_GET['device_type_id'])) {
    fail("Parameter device_type_id required", 400);
}

$device_type_id = intval($_GET['device_type_id']);
if ($device_type_id <= 0) {
    fail("device_type_id invalid", 400);
}

function rrmdir($dir)
{
    if (!is_dir($dir))
        return;
    $items = array_diff(scandir($dir), ['.', '..']);
    foreach ($items as $item) {
        $path = $dir . DIRECTORY_SEPARATOR . $item;
        if (is_dir($path))
            rrmdir($path);
        else
            @unlink($path);
    }
    @rmdir($dir);
}

function rcopy($src, $dst)
{
    if (!is_dir($src))
        return false;
    if (!is_dir($dst) && !mkdir($dst, 0777, true))
        return false;
    $items = array_diff(scandir($src), ['.', '..']);
    foreach ($items as $item) {
        $s = $src . DIRECTORY_SEPARATOR . $item;
        $d = $dst . DIRECTORY_SEPARATOR . $item;
        if (is_dir($s)) {
            if (!rcopy($s, $d))
                return false;
        } else {
            if (!copy($s, $d))
                return false;
        }
    }
    return true;
}

function sanitize_c_identifier($s)
{
    $s = preg_replace('/[^a-zA-Z0-9_]/', '_', $s);
    if (preg_match('/^[0-9]/', $s))
        $s = '_' . $s;
    return $s;
}

function writeOrFail($path, $content)
{
    $dir = dirname($path);
    if (!is_dir($dir) && !mkdir($dir, 0777, true))
        fail("Failed to create dir for $path");
    if (file_put_contents($path, $content) === false)
        fail("Failed to write file $path");
}

function apply_replacements($content, $map)
{
    return str_replace(array_keys($map), array_values($map), $content);
}

$stmt = $conn->prepare("
    SELECT dt.*, dc.description AS category_name
    FROM device_type dt
    LEFT JOIN device_category dc ON dt.category_id = dc.category_id
    WHERE dt.device_type_id = ? LIMIT 1
");
if (!$stmt)
    fail("DB prepare failed: " . $conn->error);
$stmt->bind_param("i", $device_type_id);
$stmt->execute();
$res = $stmt->get_result();
$device = $res->fetch_assoc();
$stmt->close();

if (!$device)
    fail("Device not found", 404);

$DEVICE_NAME = $device['device_name'] ?? "device_{$device_type_id}";
$CATEGORY_NAME = $device['category_name'] ?? '';
$COMPANY_CODE = $device['company_code'] ?? '';
$DEVICE_TYPE_ID = $device['device_type_id'] ?? $device_type_id;
$DEVICE_TYPE_CODE = $device['device_type_code'] ?? '';
$safe_device_folder = preg_replace('/[^a-zA-Z0-9_\-\.]/', '_', $DEVICE_NAME);

$stmt = $conn->prepare("
    SELECT dvc.command_id, dvc.command_code, dvc.command_name, dvc.command_type
    FROM device_vs_command dv
    INNER JOIN device_command dvc 
    ON dv.command_id = dvc.command_id
    AND dvc.company_code = ?
    AND dvc.is_deleted = 0
    WHERE dv.device_type_id = ?
    AND dv.company_code = ?
    ORDER BY dvc.command_id ASC
");
if (!$stmt)
    fail("DB prepare failed: " . $conn->error);
$stmt->bind_param("iii", $COMPANY_CODE, $device_type_id, $COMPANY_CODE);
$stmt->execute();
$rs = $stmt->get_result();

$commands = [];
while ($r = $rs->fetch_assoc()) {
    $commands[] = $r;
}
$stmt->close();

$cmd_details_map = [];
$detail_stmt = $conn->prepare("
    SELECT command_detail_id, command_id, data_name, data_type, data_len, data_default
    FROM device_command_detail
    WHERE command_id = ?
    ORDER BY command_detail_id ASC
");
if (!$detail_stmt)
    fail("DB prepare detail failed: " . $conn->error);

foreach ($commands as $c) {
    $cid = intval($c['command_id']);
    $detail_stmt->bind_param("i", $cid);
    $detail_stmt->execute();
    $dres = $detail_stmt->get_result();
    $rows = [];
    while ($dr = $dres->fetch_assoc())
        $rows[] = $dr;
    $cmd_details_map[$cid] = $rows;
}
$detail_stmt->close();

$set_cmd = array_values(array_filter($commands, fn($x) => ($x['command_type'] ?? '') === 'set'));
$exe_cmd = array_values(array_filter($commands, fn($x) => ($x['command_type'] ?? '') === 'exe'));
$dat_cmd = array_values(array_filter($commands, fn($x) => ($x['command_type'] ?? '') === 'dat'));

$scriptDir = realpath(__DIR__);
if ($scriptDir === false) {
    fail("Cannot resolve script directory __DIR__: " . __DIR__);
}
$projectRoot = realpath($scriptDir . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . '..');
if ($projectRoot === false) {
    $projectRoot = realpath(rtrim($_SERVER['DOCUMENT_ROOT'] ?? '', DIRECTORY_SEPARATOR));
}

if ($projectRoot === false) {
    fail("Cannot resolve project root. DOCUMENT_ROOT=" . ($_SERVER['DOCUMENT_ROOT'] ?? '(null)') . " __DIR__=" . __DIR__);
}

$projectRoot = rtrim(str_replace(['\\', '/'], DIRECTORY_SEPARATOR, $projectRoot), DIRECTORY_SEPARATOR);

$TEMPLATE_DIR = __DIR__ . '/../../templates/';
$OUTPUT_PARENT = __DIR__ . '/../../generated/';
$OUTPUT_DIR = $OUTPUT_PARENT . $safe_device_folder . DIRECTORY_SEPARATOR;

if (!is_dir($TEMPLATE_DIR)) {
    fail("Templates dir not found: {$TEMPLATE_DIR}\n(Project root detected as: {$projectRoot})");
}

if (is_dir($OUTPUT_DIR))
    rrmdir($OUTPUT_DIR);
if (!mkdir($OUTPUT_DIR, 0777, true) && !is_dir($OUTPUT_DIR))
    fail("Failed to create output dir: {$OUTPUT_DIR}");

if (!rcopy($TEMPLATE_DIR, $OUTPUT_DIR)) {
    rrmdir($OUTPUT_DIR);
    fail("Failed to copy templates from {$TEMPLATE_DIR} to {$OUTPUT_DIR}");
}

$lib_dir = $OUTPUT_DIR . 'lib' . DIRECTORY_SEPARATOR;
if (!is_dir($lib_dir) && !mkdir($lib_dir, 0777, true))
    fail("Cannot create lib dir");

function moveIntoLib($sourceRoot, $folderName, $libDir)
{
    $src = rtrim($sourceRoot, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR . $folderName;
    if (!is_dir($src))
        return;
    $dst = rtrim($libDir, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR . $folderName;
    if (is_dir($dst))
        rrmdir($dst);
    if (!@rename($src, $dst)) {
        if (!rcopy($src, $dst))
            return;
        rrmdir($src);
    }
}

$items = array_diff(scandir($OUTPUT_DIR), ['.', '..', 'src', 'lib']);
foreach ($items as $it) {
    if (is_dir($OUTPUT_DIR . $it))
        moveIntoLib($OUTPUT_DIR, $it, $lib_dir);
}

$replacements = [
    '{{DEVICE_NAME}}' => $DEVICE_NAME,
    '{{CATEGORY_NAME}}' => $CATEGORY_NAME,
    '{{COMPANY_CODE}}' => $COMPANY_CODE,
    '{{DEVICE_TYPE_ID}}' => $DEVICE_TYPE_ID,
    '{{DEVICE_TYPE_CODE}}' => $DEVICE_TYPE_CODE,
    '{{DEVICE_NAME_SAFE}}' => $safe_device_folder,
    '{{DEVICE_NAME_UPPER}}' => strtoupper($DEVICE_NAME)
];

function build_setup_map($arr, $detailsMap)
{
    $map = [];
    foreach ($arr as $c) {
        $cid = $c['command_id'];
        $details = $detailsMap[$cid] ?? [];
        $keys = [];
        $data_types = [];
        $data_lens = [];
        $defaults = [];

        foreach ($details as $d) {
            $keys[] = $d['data_name'];
            $data_types[] = $d['data_type'];
            $data_lens[] = intval($d['data_len']);

            $decoded = @json_decode($d['data_default'], true);
            if ($decoded === null && strtolower(trim($d['data_default'])) !== 'null') {
                $decoded = $d['data_default'];
            }

            $dtype = strtolower(trim($d['data_type'] ?? ''));
            if (in_array($dtype, ['string', 'object'])) {
                if (is_array($decoded))
                    $defaults[] = $decoded;
                else
                    $defaults[] = (string) $decoded;
            } else {
                $defaults[] = $decoded;
            }
        }

        $map[$c['command_name']] = [
            'commandId' => $c['command_code'],
            'key' => array_values($keys),
            'data_type' => array_values($data_types),
            'data_len' => array_values($data_lens),
            'default' => array_values($defaults),
            'queue' => $c['command_name'] . 'queue',
            'path' => '/' . $c['command_name']
        ];
    }
    return $map;
}

$map_set = build_setup_map($set_cmd, $cmd_details_map);
$map_exe = build_setup_map($exe_cmd, $cmd_details_map);
$map_dat = build_setup_map($dat_cmd, $cmd_details_map);

function build_id_map($arr)
{
    $m = [];
    foreach ($arr as $c)
        $m[(string) $c['command_code']] = $c['command_name'];
    return $m;
}

$map_set_id = build_id_map($set_cmd);
$map_exe_id = build_id_map($exe_cmd);
$map_dat_id = build_id_map($dat_cmd);

function build_cstring($varName, $assoc)
{
    $json = json_encode($assoc, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
    return "const char* {$varName} = R\"(\n{$json}\n)\";\n";
}

$ch_src_dir = $lib_dir . 'commandHandler' . DIRECTORY_SEPARATOR . 'src' . DIRECTORY_SEPARATOR;
if (!is_dir($ch_src_dir) && !mkdir($ch_src_dir, 0777, true))
    fail("Cannot create ch src dir");

writeOrFail($ch_src_dir . 'setupSetJson.h', build_cstring('setupSetJson', $map_set));
writeOrFail($ch_src_dir . 'setupExeJson.h', build_cstring('setupExeJson', $map_exe));
writeOrFail($ch_src_dir . 'setupDatJson.h', build_cstring('setupDatJson', $map_dat));

writeOrFail($ch_src_dir . 'commandIdSetJson.h', build_cstring('commandIdSetJson', $map_set_id));
writeOrFail($ch_src_dir . 'commandIdExeJson.h', build_cstring('commandIdExeJson', $map_exe_id));
writeOrFail($ch_src_dir . 'commandIdDatJson.h', build_cstring('commandIdDatJson', $map_dat_id));

$struct_text = "";
$queue_decl_text = "";
$queue_init_text = "";
$queue_send_blocks = "";
$load_struct_functions = "";
$load_struct_prototypes = "";

foreach ($commands as $c) {
    $rawname = $c['command_name'] ?? ('cmd' . $c['command_id']);
    $name = sanitize_c_identifier($rawname);

    $struct_text .= "struct {$name}_struct {\n";

    $details = $cmd_details_map[intval($c['command_id'])] ?? [];

    if (empty($details)) {
        $struct_text .= "    // no fields defined for {$name}\n";
    } else {
        foreach ($details as $d) {
            $d_type = trim($d['data_type'] ?? '');
            $d_len = intval($d['data_len'] ?? 0);
            $d_name = sanitize_c_identifier($d['data_name'] ?: 'field');

            if ($d_type === 'string' || $d_type === 'object') {
                $len = max(1, $d_len);
                $struct_text .= "    char {$d_name}[{$len}];\n";
            } else {
                $type = $d_type ?: 'uint8_t';
                $struct_text .= "    {$type} {$d_name};\n";
            }
        }
    }

    $struct_text .= "};\n\n";

    if (($c['command_type'] ?? '') === 'set') {
        $queue_decl_text .= "extern QueueHandle_t {$name}Queue;\n";
        $queue_init_text .= "    {$name}Queue = xQueueCreate(1, 1);\n";

        $queueNameStr = $c['command_name'] . 'queue';
        $cmp = "strcmp(queueName, \"{$queueNameStr}\") == 0";
        $queue_send_blocks .= "if ({$cmp}) {\n    if ({$name}Queue != NULL) {\n        xQueueOverwrite({$name}Queue, &newValues);\n    }\n}\nelse ";
    }

    if (in_array(($c['command_type'] ?? ''), ['set', 'exe', 'dat'])) {
        $fn = "bool CommandHandler::loadStruct({$name}_struct &structData, bool internal, bool loadJson, JsonDocument *docResult) {\n";
        $fn .= "    JsonDocument doc;\n";
        $fn .= "    if (!loadJson) {\n";
        $fn .= "        bool hasValue = false;\n";
        $fn .= "        if (internal && loadSettingsInternal(\"{$c['command_name']}\", doc)) {\n";

        foreach ($details as $d) {
            $d_name = sanitize_c_identifier($d['data_name'] ?: 'field');
            $type = strtolower($d['data_type'] ?? '');
            if ($type === 'string' || $type === 'object') {
                $fn .= "            strcpy(structData.{$d_name}, doc[\"data\"][\"{$d['data_name']}\"]);\n";
            } else {
                $fn .= "            structData.{$d_name} = doc[\"data\"][\"{$d['data_name']}\"];\n";
            }
        }

        $fn .= "            return true;\n";
        $fn .= "        }\n";
        $fn .= "        if (!internal && loadSettings(\"{$c['command_name']}\", doc)) {\n";

        foreach ($details as $d) {
            $d_name = sanitize_c_identifier($d['data_name'] ?: 'field');
            $type = strtolower($d['data_type'] ?? '');
            if ($type === 'string' || $type === 'object') {
                $fn .= "            strcpy(structData.{$d_name}, doc[\"data\"][\"{$d['data_name']}\"]);\n";
            } else {
                $fn .= "            structData.{$d_name} = doc[\"data\"][\"{$d['data_name']}\"];\n";
            }
        }

        $fn .= "            return true;\n";
        $fn .= "        }\n";

        $fn .= "        if (!hasValue) {\n";
        for ($i = 0; $i < count($details); $i++) {
            $d = $details[$i];
            $d_name = sanitize_c_identifier($d['data_name'] ?: 'field');
            $type = strtolower($d['data_type'] ?? '');
            if ($type === 'string' || $type === 'object') {
                $fn .= "            strcpy(structData.{$d_name}, setupDoc[\"{$c['command_name']}\"][\"default\"][{$i}]);\n";
            } else {
                $fn .= "            structData.{$d_name} = setupDoc[\"{$c['command_name']}\"][\"default\"][{$i}];\n";
            }
        }
        $fn .= "            return true;\n";
        $fn .= "        }\n";

        $fn .= "        return false;\n";
        $fn .= "    }\n\n";

        $fn .= "    if (loadJson && docResult != nullptr) {\n";
        $fn .= "        (*docResult)[\"command\"] = \"{$c['command_name']}\";\n";

        foreach ($details as $d) {
            $d_name = sanitize_c_identifier($d['data_name'] ?: 'field');
            $fn .= "        (*docResult)[\"data\"][\"{$d['data_name']}\"] = structData.{$d_name};\n";
        }

        $fn .= "        return false;\n";
        $fn .= "    }\n\n";
        $fn .= "    return false;\n";
        $fn .= "}\n\n";

        $load_struct_functions .= $fn;
        $load_struct_prototypes .= "bool loadStruct({$name}_struct &structData, bool internal=false, bool loadJson=false, JsonDocument *docResult = nullptr);\n\n";
    }
}

$queue_send_blocks = preg_replace('/\s*else\s*$/', "", $queue_send_blocks);

$possible_cpp_paths = [
    $lib_dir . 'commandHandler/src/commandHandlerQueue.cpp',
    $OUTPUT_DIR . 'commandHandler/src/commandHandlerQueue.cpp'
];
foreach ($possible_cpp_paths as $p) {
    if (file_exists($p)) {
        $content = file_get_contents($p);
        $map = [
            '{{QUEUE_DECLARATION}}' => $queue_decl_text,
            '{{QUEUE_DECLARATIONS}}' => $queue_decl_text,
            '{{QUEUE_DECLARETIONS}}' => $queue_decl_text,
            '{{QUEUE_INITIALIZATIONS}}' => $queue_init_text,
            '{{QUEUE_SEND_BLOCKS}}' => $queue_send_blocks,
            '{{LOAD_STRUCT_FUNCTIONS}}' => $load_struct_functions,
            '{{STRUCT_DEFINITIONS}}' => $struct_text
        ];
        $new = apply_replacements($content, $map);
        if ($new !== $content)
            file_put_contents($p, $new);
    }
}

$possible_h_paths = [
    $lib_dir . 'commandHandler/src/commandHandlerQueue.h',
    $OUTPUT_DIR . 'commandHandler/src/commandHandlerQueue.h'
];
foreach ($possible_h_paths as $p) {
    if (file_exists($p)) {
        $content = file_get_contents($p);
        $map = [
            '{{STRUCT_DEFINITIONS}}' => $struct_text,
            '{{QUEUE_DECLARATIONS}}' => $queue_decl_text,
            '{{QUEUE_DECLARETIONS}}' => $queue_decl_text,
            '{{STRUCTS_DEFINITIONS}}' => $struct_text
        ];
        $new = apply_replacements($content, $map);
        if ($new !== $content)
            file_put_contents($p, $new);
    }
}

$possible_ch_paths = [
    $lib_dir . 'commandHandler/src/commandHandler.h',
    $OUTPUT_DIR . 'commandHandler/src/commandHandler.h'
];
foreach ($possible_ch_paths as $p) {
    if (file_exists($p)) {
        $content = file_get_contents($p);
        $map = ['{{LOAD_STRUCT_PROTOTYPES}}' => $load_struct_prototypes];
        $new = apply_replacements($content, $map);
        if ($new !== $content)
            file_put_contents($p, $new);
    }
}

$text_exts = ['h', 'hpp', 'c', 'cpp', 'hxx', 'cxx', 'ino', 'txt', 'json', 'md', 'php'];
$it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($OUTPUT_DIR));
foreach ($it as $file) {
    if (!$file->isFile())
        continue;
    $path = $file->getRealPath();
    $ext = strtolower(pathinfo($path, PATHINFO_EXTENSION));
    if (!in_array($ext, $text_exts))
        continue;
    $content = file_get_contents($path);
    $new = apply_replacements($content, $replacements);
    if ($new !== $content)
        file_put_contents($path, $new);
}

$meta = [
    'device_name' => $DEVICE_NAME,
    'device_type_id' => $DEVICE_TYPE_ID,
    'device_type_code' => $DEVICE_TYPE_CODE,
    'generated_at' => date('c'),
    'commands' => array_map(fn($c) => $c['command_name'] ?? $c['command_id'], $commands)
];

if (file_exists($OUTPUT_DIR . 'generated_info.json')) {
    @unlink($OUTPUT_DIR . 'generated_info.json');
}

$platformio_ini_content = <<<EOT
[env:esp32doit-devkit-v1]
platform = espressif32 @6.11.0
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
board_build.partitions = min_spiffs.csv
lib_deps =
    adafruit/RTClib@^1.14.1
    256dpi/MQTT @2.5.2
    bblanchon/ArduinoJson@^7.3.0
    h2zero/NimBLE-Arduino@^2.1.0
    mikalhart/TinyGPSPlus
    jgromes/RadioLib
;build_flags =
;    -D CORE_DEBUG_LEVEL=5

;[env:esp32-s3-devkitc-1-n16r8v]
;platform = espressif32 @6.11.0
;board = esp32-s3-devkitc-1-n16r8v
;framework = arduino
;monitor_speed = 115200
;monitor_filters = esp32_exception_decoder
;build_flags =
;    -DCORE_DEBUG_LEVEL=2
;    -DBOARD_HAS_PSRAM
;    -DARDUINO_USB_MODE=1
;    -DARDUINO_USB_CDC_ON_BOOT=1
;    -DCONFIG_SPIRAM_USE_MALLOC=1
;    -DCONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=4096
;lib_deps =
;    adafruit/RTClib@^1.14.1
;    256dpi/MQTT @2.5.2
;    bblanchon/ArduinoJson@^7.3.0
;    h2zero/NimBLE-Arduino@^2.1.0
;    mikalhart/TinyGPSPlus
;    jgromes/RadioLib
EOT;

writeOrFail($OUTPUT_DIR . 'platformio.ini', $platformio_ini_content);

$zipname = $safe_device_folder . '.zip';
$zipfull = $OUTPUT_PARENT . $zipname;
if (file_exists($zipfull))
    @unlink($zipfull);

$zip = new ZipArchive();
if ($zip->open($zipfull, ZipArchive::CREATE) !== TRUE)
    fail("Cannot create zip at $zipfull");

$files = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($OUTPUT_DIR), RecursiveIteratorIterator::LEAVES_ONLY);
$rootLen = strlen(realpath($OUTPUT_DIR)) + 1;

foreach ($files as $file) {
    if (!$file->isFile())
        continue;
    $filePath = $file->getRealPath();
    $relativePath = substr($filePath, $rootLen);
    $relativePath = str_replace(DIRECTORY_SEPARATOR, '/', $relativePath);
    $zip->addFile($filePath, $relativePath);
}

$zip->close();

$download = 'admin/device/download?file=' . $zipname;

echo json_encode([
    'status' => 'success',
    'zip' => $download,
    'generated_dir' => "<generated/{$safe_device_folder}/",
    'info' => $meta
]);
exit;
?>