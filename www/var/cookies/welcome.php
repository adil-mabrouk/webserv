<?php
// session_start();

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
    $users[] = $data;
}
fclose($file);

$valid_token = false;
foreach ($users as $index => $user) {
    if (isset($user[2]) && $user[2] == $token_id && !empty($token_id)) {
        $valid_token = true;
        echo "Welcome, " . htmlspecialchars($user[0]) . "!<br>";
        echo "Your unique token ID: " . htmlspecialchars($user[2]) . "<br>";
        echo "<a href='logout.php?index=$index'>Logout</a>";
        exit();
    }
}

if (!$valid_token) {
    header('Location: login.php?msg=Invalid_token_ID!');
    exit();
}

if (isset($_GET['index'])) {
    $index = $_GET['index'];
    $users[$index][2] = '';
    $file = fopen('data.csv', 'w');
    foreach ($users as $user) {
        fputcsv($file, $user);
    }
    fclose($file);
    setcookie('token_id', '', time() - 3600, '/');
    echo "You have logged out successfully. <a href='login.php'>Login again</a>";
    exit();
}
?>
