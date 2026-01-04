#!/usr/bin/env python3

import cgi
import cgitb
import os
import uuid
from http import cookies

cgitb.enable()

SESSION_DIR = "/tmp/cgi_sessions"
os.makedirs(SESSION_DIR, exist_ok=True)

# Read cookies
cookie = cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))

# Get or create session ID
if "session_id" in cookie:
    session_id = cookie["session_id"].value
else:
    session_id = str(uuid.uuid4())

session_file = os.path.join(SESSION_DIR, session_id)

# Load session data
visits = 0
if os.path.exists(session_file):
    with open(session_file, "r") as f:
        visits = int(f.read())

visits += 1

# Save session data
with open(session_file, "w") as f:
    f.write(str(visits))

# Set cookie
cookie["session_id"] = session_id
cookie["session_id"]["path"] = "/"

# Output headers
print("Content-Type: text/html\r")
print(cookie.output(), end="\r\n")
print("\r")

# HTML response
print(f"""
<html>
<head><title>CGI Session Test</title></head>
<body>
    <h1>CGI Session Test</h1>
    <p>Session ID: {session_id}</p>
    <p>Visit count: {visits}</p>
</body>
</html>
""")
