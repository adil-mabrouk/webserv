<?php
	// PHP logic
	$title = "My First PHP Page";
	$username = "Adil";
	$year = date("Y");
?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title><?php echo $title; ?></title>
	<style>
		body {
			font-family: Arial, sans-serif;
			background-color: #f4f4f4;
			padding: 40px;
		}
		.container {
			background: white;
			padding: 20px;
			border-radius: 8px;
			max-width: 600px;
			margin: auto;
			box-shadow: 0 0 10px rgba(0,0,0,0.1);
		}
		h1 {
			color: #333;
		}
	</style>
</head>
<body>

	<div class="container">
		<h1>Welcome <?php echo $username; ?> 👋</h1>

		<p>This page is generated using <strong>PHP</strong> and <strong>HTML</strong>.</p>

		<p>Current year: <?php echo $year; ?></p>

		<?php if ($username === "Adil"): ?>
			<p>Status: <span style="color: green;">Authorized user</span></p>
		<?php else: ?>
			<p>Status: <span style="color: red;">Guest</span></p>
		<?php endif; ?>

		<hr>

		<footer>
			<p>&copy; <?php echo $year; ?> - My PHP Website</p>
		</footer>
	</div>

</body>
</html>
