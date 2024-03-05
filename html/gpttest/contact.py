#!/usr/bin/python3
print("Content-type: text/html\n")

import cgi

form = cgi.FieldStorage()

if "name" in form and "email" in form and "message" in form:
    name = form["name"].value
    email = form["email"].value
    message = form["message"].value

    # Perform any additional processing or validation as needed
    # For now, let's just display the submitted data
    print("<h2>Form Submitted Successfully!</h2>")
    print("<p>Name: {}</p>".format(name))
    print("<p>Email: {}</p>".format(email))
    print("<p>Message: {}</p>".format(message))
else:
    # If the form is not submitted, redirect to the contact page
    print("Location: contact.html")
