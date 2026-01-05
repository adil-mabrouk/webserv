#!/usr/bin/env python3

import cgi, os, pickle, time, hashlib
from http import cookies

USERS = "users.db"
SESSIONS = "sessions"


# ---------- HTML ----------

def page(title, body):
    print("Content-Type: text/html\r\n\r")
    print(f"""<!DOCTYPE html>
<html>
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
    --cyan: #88C0D0;
    --error: #BF616A;
}}

* {{
    font-family: system-ui, sans-serif;
    box-sizing: border-box;
}}

body {{
    background: var(--bg);
    min-height: 100vh;
    display: flex;
    align-items: center;
    justify-content: center;
}}

.box {{
    background: var(--card);
    border: 1px solid var(--border);
    border-radius: 14px;
    padding: 3rem;
    width: 520px;
    text-align: center;
}}

h1 {{
    margin-bottom: 1.5rem;
}}

input {{
    width: 100%;
    padding: 0.9rem;
    margin-bottom: 1rem;
    border-radius: 8px;
    border: 1px solid var(--border);
}}

button {{
    width: 100%;
    padding: 0.9rem;
    background: var(--primary);
    border: none;
    color: white;
    font-weight: 600;
    border-radius: 8px;
    cursor: pointer;
}}

button:hover {{
    background: var(--cyan);
}}

a {{
    color: var(--primary);
    text-decoration: none;
}}

.error {{
    color: var(--error);
    margin-bottom: 1rem;
}}
</style>
</head>
<body>
<div class="box">
{body}
</div>
</body>
</html>""")


# ---------- Storage ----------

def load_users():
    if os.path.exists(USERS):
        return pickle.load(open(USERS, "rb"))
    return {}


def save_users(users):
    pickle.dump(users, open(USERS, "wb"))


# ---------- Sessions ----------

def create_session(username):
    sid = hashlib.sha1(str(time.time()).encode()).hexdigest()
    pickle.dump(username, open(f"{SESSIONS}/{sid}", "wb"))
    return sid


def get_session():
    if "HTTP_COOKIE" not in os.environ:
        return None
    c = cookies.SimpleCookie(os.environ["HTTP_COOKIE"])
    if "SID" not in c:
        return None
    path = f"{SESSIONS}/{c['SID'].value}"
    if os.path.exists(path):
        return pickle.load(open(path, "rb"))
    return None


# ---------- Pages ----------

def login_page(error=""):
    page("Login", f"""
<h1>Login</h1>
{f'<div class="error">{error}</div>' if error else ''}
<form method="post">
    <input name="username" placeholder="Username" required>
    <input type="password" name="password" placeholder="Password" required>
    <button name="action" value="login">Log in</button>
</form>
<p><a href="?page=signup">Create an account</a></p>
""")


def signup_page(error=""):
    page("Sign up", f"""
<h1>Sign up</h1>
{f'<div class="error">{error}</div>' if error else ''}
<form method="post">
    <input name="username" placeholder="Username" required>
    <input type="password" name="password" placeholder="Password" required>
    <button name="action" value="signup">Sign up</button>
</form>
<p><a href="?page=login">Back to login</a></p>
""")


def home_page(user):
    page("Home", f"""
<h1>Welcome back 👋</h1>
<p><strong>{user}</strong></p>
<form method="post">
    <button name="logout" value="1">Log out</button>
</form>
""")


# ---------- Main ----------

def main():
    os.makedirs(SESSIONS, exist_ok=True)
    form = cgi.FieldStorage()

    # Logout
    if form.getvalue("logout"):
        print("Set-Cookie: SID=; Max-Age=0\n")
        login_page()
        return

    # Already signed in
    user = get_session()
    if user:
        home_page(user)
        return

    action = form.getvalue("action")

    if action == "login":
        users = load_users()
        u, p = form.getvalue("username"), form.getvalue("password")

        if users.get(u) == p:
            sid = create_session(u)
            c = cookies.SimpleCookie()
            c["SID"] = sid
            c["SID"]["path"] = "/"
            print(c.output())
            home_page(u)
        else:
            login_page("Unknown username or password")

    elif action == "signup":
        users = load_users()
        u, p = form.getvalue("username"), form.getvalue("password")

        if u in users:
            signup_page("Username already exists")
        else:
            users[u] = p
            save_users(users)
            sid = create_session(u)
            c = cookies.SimpleCookie()
            c["SID"] = sid
            c["SID"]["path"] = "/"
            print(c.output())
            home_page(u)

    else:
        if os.environ.get("QUERY_STRING") == "page=signup":
            signup_page()
        else:
            login_page()


if __name__ == "__main__":
    main()

