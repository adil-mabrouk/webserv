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
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Welcome</title>
        <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600&display=swap" rel="stylesheet">
        <style>
            :root {{
                --ig-background: #fafafa;
                --ig-primary: #0095f6;
                --ig-primary-hover: #1877f2;
                --ig-text-dark: #262626;
                --ig-text-light: #8e8e8e;
                --ig-border: #dbdbdb;
                --ig-white: #ffffff;
            }}

            * {{
                margin: 0;
                padding: 0;
                box-sizing: border-box;
                font-family: 'Inter', system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
            }}

            body {{
                background-color: var(--ig-background);
                display: flex;
                justify-content: center;
                align-items: center;
                min-height: 100vh;
                padding: 1rem;
            }}

            .greeting-container {{
                background-color: var(--ig-white);
                border: 1px solid var(--ig-border);
                border-radius: 12px;
                width: 100%;
                max-width: 400px;
                padding: 2rem;
                text-align: center;
                box-shadow: 0 1px 3px rgba(0,0,0,0.05);
            }}

            .greeting-logo {{
                font-family: 'Inter', sans-serif;
                font-size: 2.5rem;
                font-weight: 600;
                color: var(--ig-text-dark);
                margin-bottom: 1.5rem;
            }}

            .greeting-page h1 {{
                font-size: 1.5rem;
                color: var(--ig-text-dark);
                margin-bottom: 1rem;
            }}

            .message-box {{
                background-color: var(--ig-primary);
                color: var(--ig-white);
                padding: 1rem;
                border-radius: 8px;
                margin-bottom: 1.5rem;
                font-size: 0.95rem;
            }}

            .btn-primary {{
                display: inline-block;
                width: 100%;
                padding: 0.75rem;
                background-color: var(--ig-primary);
                color: var(--ig-white);
                border: none;
                border-radius: 8px;
                font-weight: 600;
                text-decoration: none;
                transition: background-color 0.2s ease;
            }}

            .btn-primary:hover {{
                background-color: var(--ig-primary-hover);
            }}
        </style>
    </head>
    <body>
        <div class="greeting-container">
            <div class="greeting-logo">Webserv</div>
            <h1>Hello, {session.name}!</h1>
            <div class="message-box">
                <p>You have successfully logged in.</p>
            </div>
            <a href="/cgi-bin/login.py" class="btn-primary">Return to Home</a>
        </div>
    </body>
    </html>
    """)

def printLogin():
    # print("Content-Type: text/html\r\n\r\n")
    print("""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Webserv Login</title>
            <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600&display=swap" rel="stylesheet">
            <style>
                :root {
                    --ig-background: #fafafa;
                    --ig-primary: #0095f6;
                    --ig-primary-hover: #1877f2;
                    --ig-text-dark: #262626;
                    --ig-text-light: #8e8e8e;
                    --ig-border: #dbdbdb;
                    --ig-white: #ffffff;
                }

                * {
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                    font-family: 'Inter', system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
                }

                body {
                    background-color: var(--ig-background);
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    min-height: 100vh;
                    padding: 1rem;
                }

                .login-container {
                    background-color: var(--ig-white);
                    border: 1px solid var(--ig-border);
                    border-radius: 12px;
                    width: 100%;
                    max-width: 400px;
                    padding: 2rem;
                    text-align: center;
                    box-shadow: 0 1px 3px rgba(0,0,0,0.05);
                }

                .login-logo {
                    font-family: 'Inter', sans-serif;
                    font-size: 3rem;
                    font-weight: 600;
                    color: var(--ig-text-dark);
                    margin-bottom: 2rem;
                }

                .form-group {
                    margin-bottom: 1rem;
                }

                .form-group input {
                    width: 100%;
                    padding: 0.75rem;
                    border: 1px solid var(--ig-border);
                    border-radius: 6px;
                    font-size: 0.95rem;
                    background-color: var(--ig-background);
                    color: var(--ig-text-dark);
                }

                .form-group input:focus {
                    outline: none;
                    border-color: var(--ig-primary);
                }

                .login-button {
                    width: 100%;
                    padding: 0.75rem;
                    background-color: var(--ig-primary);
                    color: var(--ig-white);
                    border: none;
                    border-radius: 8px;
                    font-weight: 600;
                    cursor: pointer;
                    transition: background-color 0.2s ease;
                }

                .login-button:hover {
                    background-color: var(--ig-primary-hover);
                }

                .register-link {
                    margin-top: 1rem;
                    font-size: 0.9rem;
                    color: var(--ig-text-light);
                }

                .register-link a {
                    color: var(--ig-primary);
                    text-decoration: none;
                    font-weight: 600;
                }

                .register-link a:hover {
                    text-decoration: underline;
                }
            </style>
        </head>
        <body>
            <main class="login-container">
                <div class="login-logo">Webserv</div>
                <form action="/cgi-bin/login.py" method="POST">
                    <div class="form-group">
                        <input type="text" placeholder="Username" name="username" required>
                    </div>
                    <div class="form-group">
                        <input type="password" placeholder="Password" name="password" required>
                    </div>
                    <button type="submit" class="login-button">Log In</button>
                    <p class="register-link">Don't have an account? <a href="/cgi-bin/register.html">Sign up</a></p>
                </form>
            </main>
        </body>
        </html>
    """)

def printUserMsg(msg):
    msg_class = "success" if "success" in msg.lower() else "error"
    # print("Content-Type: text/html\r\n\r\n")
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Webserv - Message</title>
        <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600&display=swap" rel="stylesheet">
        <style>
            :root {{
                --ig-background: #fafafa;
                --ig-primary: #0095f6;
                --ig-primary-hover: #1877f2;
                --ig-text-dark: #262626;
                --ig-text-light: #8e8e8e;
                --ig-border: #dbdbdb;
                --ig-white: #ffffff;
                --ig-success: #2ecc71;
                --ig-error: #e74c3c;
            }}

            * {{
                margin: 0;
                padding: 0;
                box-sizing: border-box;
                font-family: 'Inter', system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
            }}

            body {{
                background-color: var(--ig-background);
                display: flex;
                flex-direction: column;
                justify-content: center;
                align-items: center;
                min-height: 100vh;
                padding: 1rem;
            }}

            .message-box {{
                background-color: var(--ig-white);
                border: 1px solid var(--ig-border);
                border-radius: 12px;
                width: 100%;
                max-width: 400px;
                padding: 2rem;
                text-align: center;
                box-shadow: 0 1px 3px rgba(0,0,0,0.05);
            }}

            .message-text {{
                margin-bottom: 1.5rem;
                color: var(--ig-text-dark);
                font-size: 1rem;
            }}

            .success {{
                color: var(--ig-success);
            }}

            .error {{
                color: var(--ig-error);
            }}

            .btn-primary {{
                display: inline-block;
                width: 100%;
                padding: 0.75rem;
                background-color: var(--ig-primary);
                color: var(--ig-white);
                border: none;
                border-radius: 8px;
                font-weight: 600;
                text-decoration: none;
                transition: background-color 0.2s ease;
            }}

            .btn-primary:hover {{
                background-color: var(--ig-primary-hover);
            }}
        </style>
    </head>
    <body>
        <div class="message-box">
            <div class="message-text {msg_class}">
                <p>{msg}</p>
            </div>
            <a href="/cgi-bin/login.py" class="btn-primary">Return to Login</a>
        </div>
    </body>
    </html>
    """)


def authUser(name, password):
    if os.path.exists('user_database'):
        with open('user_database', 'rb') as f:
            database = pickle.load(f)
            if name in database.user_pass and database.user_pass[name] == password:
                session = Session(database.user_firstname[name])
                return session
            else:
                return None
    else:
        return None

def setLongTermCookie(cookies_obj, key, value):
    """Set a cookie with a far future expiration date"""
    cookies_obj[key] = value
    # Set cookie to expire in 30 days
    expiration = datetime.datetime.now() + datetime.timedelta(days=30)
    cookies_obj[key]["expires"] = expiration.strftime("%a, %d %b %Y %H:%M:%S GMT")
    cookies_obj[key]["path"] = "/"  # Make cookie available for entire domain

def handleLogin():
    username = form.getvalue('username')
    password = form.getvalue('password')
    firstname = form.getvalue('firstname')
    
    if username is None:
        printLogin()
    elif firstname is None:
        session = authUser(username, password)
        if session is None:
            printUserMsg("Failed To Login, Username or Password is wrong!")
        else:
            # Initialize or reset the cookies
            cookies_obj = cookies.SimpleCookie()

            # Set a persistent "SID" cookie with session ID
            setLongTermCookie(cookies_obj, "SID", session.getSid())
            
            # Output the cookie headers
            # print("HTTP/1.1 200 OK")
            print(cookies_obj.output(), end="\r\n\r\n")
            # print("Content-Type: text/html\r\n\r\n")

            # Print the greeting page
            printGreetingPage(session)
    else:
        # Registration process - without username check
        try:
            database = None
            if os.path.exists('user_database'):
                with open('user_database', 'rb') as f:
                    database = pickle.load(f)
            else:
                database = UserDataBase()
                
            # Add new user without checking if username exists
            database.addUser(username, password, firstname)
            printUserMsg("Account registered successfully!")
        except Exception as e:
            # Debug output
            print(f"Registration error: {str(e)}", file=sys.stderr)
            printUserMsg(f"Error during registration: {str(e)}") 

def checkSessionCookie():
    """Check if user has a valid session cookie and return the session if found"""
    if 'HTTP_COOKIE' in os.environ:
        cookies_obj = cookies.SimpleCookie()
        cookies_obj.load(os.environ['HTTP_COOKIE'])

        if "SID" in cookies_obj:
            sid = cookies_obj["SID"].value
            session_file = 'sessions/session_' + sid
            
            # Check if the session file exists
            if os.path.exists(session_file):
                with open(session_file, 'rb') as f:
                    return pickle.load(f)
    return None

def main():
    # Create sessions directory if it doesn't exist
    if not os.path.exists('sessions'):
        os.makedirs('sessions')
        
    global form
    # Load CGI form
    form = cgi.FieldStorage()

    # First check if there's a valid session cookie
    session = checkSessionCookie()
    if session:
        # User has a valid session, show greeting page
        printGreetingPage(session)
        return

    # No valid session, check if we're processing a login/registration
    if form.getvalue('username') is not None:
        handleLogin()
    else:
        # No session cookie and no login attempt, show login page
        printLogin()

if __name__ == "__main__":
    main()
