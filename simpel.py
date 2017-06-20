import requests
import math
import json
from bs4 import BeautifulSoup

URL = "https://mijn.simpel.nl"

headers = {
        "User-Agent": "Personal crawler, contact harm@mindshards.com",
        }

def post(data):
    print("POSTing %s to %s" % (data, ip))
    requests.post("http://" + ip + "/", data=data)

# Get the CSRF token from the homepage
home = requests.get(URL)
soup = BeautifulSoup(home.text, 'html.parser')
csrf = soup.find(attrs={"name": "login_csrf"})['value']

# Get the username and password
f = open('secrets', 'r')
username = f.readline().rstrip()
password = f.readline().rstrip()
ip = f.readline().rstrip()

# Login, this yields a 303 Status
login = requests.post(
        URL,
        data = {"login_csrf": csrf ,
            "username": username,
            "password": password,
            "login": 1
            },
        cookies = home.cookies,
        headers = headers,
        allow_redirects = False
        )

# Check if the login succeeded
isSuccess = requests.get(URL, 
        cookies=home.cookies,
        headers = headers,
        allow_redirects = False)

if isSuccess.status_code == 302:
    # The browser follows the 302 and fetches the URL yet again. We don't need
    # to go through that page to get the usage data.
    alotment_page = requests.get(URL + "/verbruik/json", cookies=home.cookies)
    alotment = json.loads(alotment_page.text)
    post(math.floor(int(alotment["data"]["amount"])/1024))

