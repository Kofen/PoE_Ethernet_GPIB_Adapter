from flask import Flask
import random

app = Flask(__name__)


@app.route('/')
def home():
    # read the file to string
    with open("index.html", "r") as file:
        content = file.read()
    return content


@app.route('/cnx')
def cnx():
    return str(random.randint(0, 4))


@app.route('/ex0/<string:inst>/<string:cmd>')
def ex(inst: str, cmd: str):
    print("ex/" + inst + "/" + cmd)
    
    # This doesn't work with flask, since it is disturbed by the ? in the command
    if cmd.endswith("?"):
        return "bla"
    else:
        # workaround for flask:
        if cmd == "*IDN":
            return "bla"
        elif cmd == "SYST:ERR":
            return "0,\"No error\""
        else:        
            return ""


@app.route('/ex1/<string:inst>/<string:cmd>')
def send(inst: str, cmd: str):
    print("send/" + inst + "/" + cmd)
    return ""


@app.route('/ex2/<string:inst>')
def read(inst: str):
    print("read/" + inst)
    return "bla"


@app.route('/fnd')
def status():
    s = ""
    nr = random.randint(0, 5)
    nr = random.randint(1, 5)
    for i in range(nr):
        nr += random.randint(1, 5)
        if nr > 30:
            break
        j = "gpib," + str(nr)
        s += f"<option value=\"{j}\">{j}</option>"
    return s


if __name__ == '__main__':
    app.run(debug=False)
# To run the server, use the command: python mockserver.py
# The server will be available at http://127.0.0.1:5000
