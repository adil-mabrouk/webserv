def render(title, content):
    print("Content-Type: text/html\r\n\r")
    print(f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>{title}</title>
<style>
:root {{
    --bg: #ECEFF4;
    --card: #E5E9F0;
    --border: #D8DEE9;
    --text: #2E3440;
    --primary: #5E81AC;
    --primary-hover: #81A1C1;
    --error: #BF616A;
}}

* {{
    box-sizing: border-box;
    font-family: system-ui, sans-serif;
}}

body {{
    background: var(--bg);
    min-height: 100vh;
    display: flex;
    align-items: center;
    justify-content: center;
}}

.container {{
    background: var(--card);
    border: 1px solid var(--border);
    border-radius: 14px;
    padding: 3rem;
    width: 100%;
    max-width: 520px;
    text-align: center;
}}

h1 {{
    margin-bottom: 2rem;
    color: var(--text);
}}

input {{
    width: 100%;
    padding: 0.9rem;
    margin-bottom: 1.2rem;
    border-radius: 8px;
    border: 1px solid var(--border);
    font-size: 1rem;
}}

button {{
    width: 100%;
    padding: 0.9rem;
    border-radius: 8px;
    border: none;
    background: var(--primary);
    color: white;
    font-weight: 600;
    cursor: pointer;
    font-size: 1rem;
}}

button:hover {{
    background: var(--primary-hover);
}}

.error {{
    color: var(--error);
    margin-bottom: 1rem;
}}
</style>
</head>
<body>
<div class="container">
{content}
</div>
</body>
</html>""")
