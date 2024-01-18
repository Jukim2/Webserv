#!/usr/bin/env python3
import cgi
import os

# Enable debugging
import cgitb
cgitb.enable()

def save_uploaded_file(fileitem):
    # Get the filename from the file item
    filename = os.path.basename(fileitem.filename)
    
    # Specify the path to save the file (in the current working directory)
    filepath = os.path.join(os.getcwd(), filename)

    # Open the file for writing in binary mode
    with open(filepath, 'wb') as f:
        # Write the file data
        f.write(fileitem.file.read())

    return filepath

def main():
    print("Content-type: text/html\n")

    # Create an instance of the FieldStorage class
    form = cgi.FieldStorage()

    # Get the file item from the form
    fileitem = form['file']

    # Check if the file was uploaded
    if fileitem.filename:
        try:
            # Save the uploaded file
            filepath = save_uploaded_file(fileitem)
            print(f"File '{fileitem.filename}' uploaded successfully and saved at: {filepath}")
        except Exception as e:
            print(f"Error saving the file: {e}")
    else:
        print("No file uploaded.")

if __name__ == "__main__":
    main()