#!/usr/bin/env python3
import cgi
import os

# Enable debugging
import cgitb
cgitb.enable()

def save_uploaded_file(fileitem):
    try:
        # Get the filename from the file item
        filename = os.path.basename(fileitem.filename)

        # Specify the path to save the file (in the current working directory)
        if not os.path.exists(os.getcwd() + '/html/upload/uploaded_files/'):
            os.makedirs(os.getcwd() + '/html/upload/uploaded_files/')
        filepath = os.path.join(os.getcwd() + '/html/upload/uploaded_files/', filename)

        # Open the file for writing in binary mode
        with open(filepath, 'wb') as f:
            # Write the file data
            f.write(fileitem.file.read())
        return filepath
    except Exception as e:
        raise RuntimeError(f"Error saving the file: {e}")

def main():
    print("Status: 200 OK\r")
    print("Content-type: text/html; charset=utf-8\r\n\r")

    # Create an instance of the FieldStorage class
    form = cgi.FieldStorage()

    # Get the file item from the form
    fileitem = form['fileToUpload']

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
