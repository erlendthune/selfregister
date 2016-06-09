# selfregister
PHP script for self registering Feide (SAML) and traditional Canvas users to multiple courses and course groups.

The register.html file is an example of how to use the functionality. The variables in inc/vars.inc must be customized for your Canvas server. The content of that file should also be protected from external users as it contains an access token to your web server. On the web server, you can obtain this by giving the inc folder the following permissions: drwx------ and the content of the inc directory the following permissions: -rw-------
