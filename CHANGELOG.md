## CHANGELOG

- ***When adding content to this document, make sure to create a section for each module 
if the changes only impact a single module and that section does not already exist in
the document. When changes affect the entire software, make sure to add them in the 
core section.***

- ***When possible please provide the number of the pull request(s) containing the 
changes in the following format: PR #1234***

### LORIS 23.0 (Released: ??)


#### Core
- Menus are now maintained by modules and no longer in the SQL database (PR #5839)
- Very old instruments relying on QuickForm may have issues due to code changes (PR #4928)
- Unix user permissions have been updated which may affect access to files. New
documentation for file permissions has been added to the README.md file (PR #5323)

#### Modules 

##### Issue Tracker
- The issue_tracker module now has the feature of uploading attachments to new or existing issues.

##### Battery Manager
 - New module created to manage the entries in the test_battery table of the database.
 This allows projects to modify their instrument battery without requiring backend access.
 (PR #4221)

##### Module Manager
 - New module created to manage the status of installed modules (PR #6015)

##### Electrophysiology Browser
 - New module created to view electrophysiology data within LORIS. (PR #5230)


#### Clean Up
- New tool for detection of multiple first visits for a candidate (prevents a database
 exception). It is recommended to run this tool for existing projects (PR #5270)
