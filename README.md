# Proyecto-IMTC
**IoT Project for Autonomous Firefighting and Fire Detection System**

## Structure
This repo consists of the following folders:
1. **Scripts**: This folder is used for programming ESP32 (robot, cloud, detection modules).
2. **KiCad**: Electronic design projects for robot and detection modules, footprints, datasheet, BOM, etc.
3. **Docs**: Word, ppt, excel documents.
4. **Workflow**: Algorithms designed (diagrams, etc).
5. **Test**: Save simple codes for testing components.

**Commits**: 
For better organization, set commits as:
- chore: Updates of tasks
- Test: Testing components, repetitive code useful
- feat: Adding new feature on the project
   
## Step by Step Guide

### Setup
1. **Install Git Bash**
   - Download and install Git Bash from: https://gitforwindows.org/

### Configure Git
   - Set your username and email in Git Bash or command prompt.
       ```bash
        git config --global user.name "Your Name"
        git config --global user.email "your.email@example.com"

**Note** You can refer to this YouTube tutorial for guidance. (https://www.youtube.com/watch?v=8JJ101D3knE)

### Clone Repository
1. **Get the Repository Link**
   - Go to Proyecto-IMTC GitHub Repository.
   - Click on the green "Code" button and copy the HTTP link.

2. **Clone the Repository**
   - Open File Explorer and navigate to the folder where you want to save the repository.
   - Open Git Bash in the selected directory and run the following command:
     ```bash
     git clone <HTTP_LINK>

### Using Command Prompt 
You can use git -h to see more details about each Git instruction. 

## Commits and Create New Branches
### 1. Create a New Branch
   - Open the command prompt, locate the repository folder and run the following command:
     ```bash
     git branch <Name of the new branch>

### 2. **Checkout**
   - Change the main branch to new branch using the following command:
     ```bash
     git checkout <Name of the new branch>
   Then, you can save your work on this branch and commit changes.

### 3. **Commit changes**
   - Add modified files using:
     ```bash
        git add <name of the file> #for a specific file
        git add . #for all updated files
   - Make a commit 
     ```bash
        git commit -m <"name of commit">

### 4. **Push Branch to Remote**
   - At this point, your local changes are stashed temporarily, but to save your changes on the branch, you should push the branch to remote repository, using:
      ```bash
      git push origin <Name of the new branch>

### 5. **Create a PR**
   - Make a PR to merge your changes into main branch.
