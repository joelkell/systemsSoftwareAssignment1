# Systems Software Assignment 1

## Introduction:
A e-commerce company needs to track changes made to its static html website. Problems have occurred in the past where changes were made to the website incorrectly and it wasn’t possible to track who made the changes. The company’s CTO has listed the functionality they would like to include in their new business model to offer transparency and accountability for all changes made to the website. How the website management currently works: Users have an account on a Debian server, they can login and make changes to the website. All changes will be made under their user accounts. The Debian server is running Apache as the webserver. All changes made to the site /var/www/html appear on the website instantly.

## What the CTO wants:
The CTO has offered a list of desired functionality for the new website management model:
1. The company will have an internal Intranet site that is a duplicate copy of the live website. Staff can make changes to the Intranet 
version of the site and see the changes before it goes live. (This will help prevent content issues and page availability issues for users of 
the site).
2. The website content should be backed up every night.
3. The changes made to the Intranet version of the site needs to documented. The username of the user, the page they modified and the 
timestamp should be recorded.
4. The live site needs to be updated based on the changes made to the Intranet site. This should happen during the night. There are a 
large number of files on the website (5000+), only the files that have changed should be copied to the live site folder.
5. No changes should be allowed to be made to the site while the backup/transfer is happening.
6. If a change needs to be urgently made to the live site, it should be possible to make the changes. (Users shouldn’t have write access to 
the new website folder)

## Project Requirements
1. Create a daemon to continually manage the operation of the requirements listed by the CTO above.
2. Identify new or modified site content and log details of who made the changes, this should be generated as a text file report and stored 
on the server.
3. Maintain a list of site updates (file)
4. Update the live site every night (After the backup completes).
5. When the backup/transfer begins no user should be able to modify and site content.
6. It must be possible to ask the daemon to backup and transfer at any time.
7. A message queue should be setup to allow all processes to report in on completion of a task. (success or failure)
8. Error logging and reporting should be included (log to file)
9. Create a makefile to manage the creation of the executables.

## General Assumptions
1. The company only has one server
2. Backups can be made to a different folder on the server
3. The Intranet and Live site should be two sub directories of /var/www/html