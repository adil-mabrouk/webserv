#!/usr/bin/env python3

import os
import time
import urllib.parse

# # CGI header (VERY IMPORTANT)
# print("Content-Type: text/plain\n")

# Get query string
query = os.environ.get("QUERY_STRING", "")

# Parse query string
params = urllib.parse.parse_qs(query)

# Extract name
name = params.get("name", [""])[0]

# Output
print(f"Hello {name}")

time.sleep(100)