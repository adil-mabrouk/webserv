#!/bin/bash

# CGI headers (mandatory)
echo -e "Content-Type: text/html; charset=UTF-8\r"
echo -e "\r"

FACTS=(
  "1337 is part of the international 42 Network founded in France."
  "At 1337, there are no teachers, no classes, and no traditional exams."
  "Students at 1337 learn through peer-to-peer learning and real projects."
  "1337 uses a project-based evaluation system conducted by other students."
  "The curriculum at 1337 focuses on low-level programming, C, C++, and system concepts."
)

RANDOM_FACT=${FACTS[$RANDOM % ${#FACTS[@]}]}
CURRENT_DATE=$(date)
REQUEST_METHOD=${REQUEST_METHOD:-UNKNOWN}

cat <<EOF
<!DOCTYPE html>
<html>
<head>
    <title>Fun Fact About 1337</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            padding: 40px;
        }
        .card {
            background: white;
            padding: 20px;
            border-radius: 8px;
            max-width: 600px;
            margin: auto;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #006233;
        }
        footer {
            margin-top: 20px;
            font-size: 0.9em;
            color: #555;
        }
    </style>
</head>
<body>
    <div class="card">
        <h1>Did You Know?</h1>
        <p>$RANDOM_FACT</p>
        <footer>
            <p><strong>Request method:</strong> $REQUEST_METHOD</p>
            <p><strong>Generated at:</strong> $CURRENT_DATE</p>
        </footer>
    </div>
</body>
</html>
EOF
