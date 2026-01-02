<?php

// session_start();

// session_unset();
// session_destroy();
$cookie_env = getenv('COOKIE');

$token_id = null;
if ($cookie_env) {
    parse_str(str_replace('; ', '&', $cookie_env), $cookies);
    if (isset($cookies['token_id'])) {
        $token_id = $cookies['token_id'];
    }
}

$file = fopen('data.csv', 'r');
$users = [];
while (($data = fgetcsv($file, 0, ",", '"', "\\")) !== FALSE) {
    if ($token_id !== null && isset($data[2]) && $data[2] === $token_id) {
        $data[2] = '';
    }
    $users[] = $data;
}
fclose($file);

$file = fopen('data.csv', 'w');
foreach ($users as $user) {
    fputcsv($file, $user, ',', '"', '\\');
}
fclose($file);

setcookie('token_id', '', time() - 3600, '/');

header("Location: login.php");
exit();
?>
