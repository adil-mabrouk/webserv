#!/usr/local/bin/python3

from http import cookies
import os
import cgi
import time
import hashlib
import pickle
import sys
import datetime


class Session:
    def __init__(self, name):
        self.name = name
        self.sid = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
        with open('sessions/session_' + self.sid, 'wb') as f:
            pickle.dump(self, f)

    def getSid(self):
        return self.sid


class UserDataBase:
    def __init__(self):
        self.user_pass = {}
        self.user_firstname = {}

    def addUser(self, username, password, firstname):
        self.user_pass[username] = password
        self.user_firstname[username] = firstname
        with open('user_database', 'wb') as f:
            pickle.dump(self, f)


def printGreetingPage(session):
    print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Welcome</title>
    <style>
        body {{
            background: #fafafa;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            font-family: Arial, sans-serif;
        }}
        .card {{
            background: white;
            padding: 2rem;
            border-radius: 10px;
            max-width: 400px;
            text-align: center;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }}
        h1 {{
            color: #006233;
        }}
        a {{
            display: inline-block;
            margin-top: 1rem;
            padding: 0.7rem 1rem;
            background: #006233;
            color: white;
            text-decoration: none;
            border-radius: 6px;
        }}
    </style>
</head>
<body>
    <div class="card">
        <h1>Hello, {session.name}!</h1>
        <p>You have successfully logged in.</p>
        <a href="/cgi-bin/login.py">Return</a>
    </div>
</body>
</html>
""")

def printLogin():
    print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>1337 Login</title>
    <style>
        body {
            background: #fafafa;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            font-family: Arial, sans-serif;
        }
        .box {
            background: white;
            padding: 2rem;
            border-radius: 10px;
            width: 350px;
            text-align: center;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        input, button {
            width: 100%;
            padding: 0.7rem;
            margin-top: 0.8rem;
        }
        button {
            background: #006233;
            color: white;
            border: none;
            border-radius: 6px;
        }
    </style>
</head>
<body>
    <div class="box">
        <h2>1337 School</h2>
        <form method="POST">
            <input name="username" placeholder="Username" required>
            <input type="password" name="password" placeholder="Password" required>
            <button type="submit">Log in</button>
        </form>
        <p style="margin-top:1rem;">New student? <a href="/cgi-bin/register.html">Register</a></p>
    </div>
</body>
</html>
""")


def printUserMsg(msg):
    print(f"""
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>1337 Message</title>
</head>
<body style="font-family:Arial; text-align:center; padding-top:50px;">
    <h3>{msg}</h3>
    <a href="/cgi-bin/login.py">Return to login</a>
</body>
</html>
""")


def authUser(name, password):
    if os.path.exists('user_database'):
        with open('user_database', 'rb') as f:
            database = pickle.load(f)
            if name in database.user_pass and database.user_pass[name] == password:
                return Session(database.user_firstname[name])
    return None


def setLongTermCookie(cookies_obj, key, value):
    cookies_obj[key] = value
    expiration = datetime.datetime.now() + datetime.timedelta(days=30)
    cookies_obj[key]["expires"] = expiration.strftime("%a, %d %b %Y %H:%M:%S GMT")
    cookies_obj[key]["path"] = "/"


def handleLogin():
    username = form.getvalue('username')
    password = form.getvalue('password')
    firstname = form.getvalue('firstname')

    if firstname is None:
        session = authUser(username, password)
        if not session:
            printUserMsg("❌ Login failed. Invalid credentials.")
        else:
            cookies_obj = cookies.SimpleCookie()
            setLongTermCookie(cookies_obj, "SID", session.getSid())
            print(cookies_obj.output(), end="\r\n\r\n")
            printGreetingPage(session)
    else:
        database = UserDataBase()
        database.addUser(username, password, firstname)
        printUserMsg("✅ Account created successfully!")


def checkSessionCookie():
    if 'HTTP_COOKIE' in os.environ:
        cookies_obj = cookies.SimpleCookie()
        cookies_obj.load(os.environ['HTTP_COOKIE'])
        if "SID" in cookies_obj:
            sid = cookies_obj["SID"].value
            path = 'sessions/session_' + sid
            if os.path.exists(path):
                with open(path, 'rb') as f:
                    return pickle.load(f)
    return None


def main():
    if not os.path.exists('sessions'):
        os.makedirs('sessions')

    global form
    form = cgi.FieldStorage()

    session = checkSessionCookie()
    if session:
        printGreetingPage(session)
    elif form.getvalue('username'):
        handleLogin()
    else:
        printLogin()


if __name__ == "__main__":
    main()
