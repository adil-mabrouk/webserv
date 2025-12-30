#!/usr/bin/perl
use strict;
use warnings;

print "Content-Type: text/html\r\n\r\n";

print <<HTML;
<!DOCTYPE html>
<html>
<head>
    <title>Perl CGI Test</title>
</head>
<body>
    <h1>Hello from Perl CGI!</h1>
    <p>Your Perl CGI is working correctly.</p>
</body>
</html>
HTML
