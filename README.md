This is a modular C++ calculator project with separate source files for core operations and reusable components.
Project Setup and Contribution Guidelines
 ```bash
1. Clone the repository
First, clone this repository to your local machine:
git clone https://github.com/shujaaking/modular_calculator.git
cd modular_calculator

2. Switch to the master branch and pull latest changes

Always make sure your local master branch is up to date:

git checkout master
git pull origin master

Important: Never make direct changes or commits to the master branch.
Use your own branch for all development work.

3. Create your own branch

Create a new branch for your work.
Use a descriptive name such as feature-addition, bugfix-division, or update-ui:

git checkout -b your-branch-name


Example:

git checkout -b feature-improvements

4. Make your changes

Edit, build, and test your updates.
When you’re ready to save your work:

git add .
git commit -m "Describe your change briefly"
Push your branch to GitHub:
git push -u origin your-branch-name

5. Keeping your branch updated

If others have made changes to master, update your branch with the latest code before merging:

git checkout master
git pull origin master
git checkout your-branch-name
git merge master

Resolve any conflicts, test your code again, and push the updates.
