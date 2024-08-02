import cgi
import cgitb

cgitb.enable()  # Enables CGI error reporting

print("<!DOCTYPE html>")
print("<html lang='en'>")
print()  # End of headers

form = cgi.FieldStorage()

print("<head>")
print("<meta charset='UTF-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("<link rel='stylesheet' href='../../styles.css'>")
print("<title>Simple CGI</title>")
print("</head>")
print("<body>")
print("<h1>Simple CGI Script</h1>")

if form:
    for key in form.keys():
        if key == "favorite":
            print(f"<p>Absolute favorite: {form.getvalue(key)}</p>")
        else:
            print(f"<p>Worst food evaaah: {form.getvalue(key)}</p>")
    print('<div class="memebox">')
    print(f'<img class="student" src="../assets/food.jpg" alt="Patrick enjoying his meal" width="400">')
    print("</div>")
else:
    print("<p>No data received.</p>")

print("</body>")
print("</html>")


