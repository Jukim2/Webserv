#!/usr/bin/perl
use strict;
use warnings;

print "Content-type: text/html\n\n";

use CGI;
my $cgi = CGI->new;

my $name    = $cgi->param('name');
my $email   = $cgi->param('email');
my $message = $cgi->param('message');

if ($name && $email && $message) {
    # Perform any additional processing or validation as needed
    # For now, let's just display the submitted data
    print "<h2>Form Submitted Successfully!</h2>";
    print "<p>Name: $name</p>";
    print "<p>Email: $email</p>";
    print "<p>Message: $message</p>";
} else {
    # If the form is not submitted, redirect to the contact page
    print "Location: contact.html";
}
