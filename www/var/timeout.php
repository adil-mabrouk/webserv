#!/usr/bin/php-cgi
<?php
header("Content-Type: text/plain");

$duration = 60; // Run for 60 seconds (should timeout at 30)

error_log("Starting timeout test: will run for {$duration}s");

echo "Starting long-running process...\n";
flush();

for ($i = 1; $i <= $duration; $i++) {
    echo "Second $i / $duration - Still running...\n";
    flush();
    
    sleep(1);
    
    if ($i % 5 == 0) {
        error_log("Timeout test: {$i}s elapsed");
    }
}

echo "Process completed successfully!\n";
error_log("Timeout test completed (this shouldn't happen if timeout is working)");
?>